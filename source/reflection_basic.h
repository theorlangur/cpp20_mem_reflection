#ifndef REFLECTION_BASIC_H_
#define REFLECTION_BASIC_H_
#include <type_traits>
#include <utility>

namespace refl
{
    //Idea of this type is to be implicitly-convertible
    //to anything
    struct any
    {
        //decl only
        constexpr any(size_t i) noexcept;
        //decl only
        template<class T> constexpr operator T() noexcept;
    };

    template<class T>
    struct info
    {
        //n - amount of arguments to check
        //an aggregate initialization for
        template<size_t n>
        //determine if a type T is aggregate-constructible with 'n'
        //arguments
        consteval static bool construct_from()
        {
            static_assert(std::is_aggregate_v<T> * (n + 1), "Must be aggregatable");
            //lambda with a variadic NTTP (non-template type parameter) 
            //template just to emulate aggregate initialization
            return []<size_t... i>(std::index_sequence<i...>)
            {
                //using C++20's concepts require mechanism
                //to determine if an aggregate initialization ({...})
                //is possible with i... arguments
                //using previously defined 'any' that's convertible to 
                //anything. It's all compile-time, 
                //no runtime code is generated
                return requires { T{any(i)...,}; };
            }(std::make_index_sequence<n>());
        }

        template<size_t idx, size_t nested_size, size_t rest>
        //determine if a type T is aggregate-constructible 
        //however an argument with index 'idx' is a nested array and
        //the expected nested array size is nested_size and the 'rest'
        //to initialize the rest arguments
        consteval static bool construct_nested_array_from()
        {
            //same C++20's concept require mechanism
            //that basically checks if it's possible
            //to aggregate-initialize:
            //{arg0, arg1, ... arg<idx-1>, {nested0, nested1, ...nested<nested_size>}, rest0, rest1, ...rest<rest>}
            return []<size_t... i, size_t... n, size_t... r>(std::index_sequence<i...>, std::index_sequence<n...>, std::index_sequence<r...>)
            {
                return requires { T{any(i)..., {any(n)...}, any(r)...}; };
            }(std::make_index_sequence<idx>(), std::make_index_sequence<nested_size>(), std::make_index_sequence<rest>());
        }

        //determines the max amount of nested initializer arguments
        //that may be used, hence determine the size of the nested array
        //field
        template<size_t index, size_t sz, size_t rest>
        consteval static size_t get_nested_array_size()
        {
            //this finds a such a 'sz' that it's possible to construct a nested
            //array field with 'sz' elements, but impossible to aggregate-initialize
            //T with reset + 1
            //this allows to correctly handle the case of field of some non-trivially constructible
            //type X with several arguments constructor
            //if field is a nested array, and 'sz' being the actual size, attempt to 
            //build T with rest+1 will fail. If the field is a type that has a user-defined
            //constructor with 'sz' arguments, it'll be possible to build T
            //with 'rest + 1'
            if constexpr (construct_nested_array_from<index, sz, rest>() && !construct_nested_array_from<index, sz, rest+1>())
                return sz;
            else
                //pseudo-recursion: invoking the same template function
                //we're starting from max possible array size and going
                //down, increasing the 'rest' arguments amount
                return get_nested_array_size<index, sz-1, rest+1>();
        }

        //determines the max amount of arguments to aggregate-initialize
        //type T. This will include the nested array fields.
        //given struct S{ int a; char b[3]; float c; }
        //the result will be 1 + 3 + 1 = 5
        template<size_t n = 0>
        consteval static size_t get_agg_max_count()
        {
            static_assert(n <= static_cast<size_t>(sizeof(T)), "Sanity check. The type may not have more normal fields than bytes");
            //the trick: we check that the type T is constructible with 'n'
            //but not constructible with 'n + 1'
            if constexpr (construct_from<n>() && !construct_from<n+1>())
                return n;
            else
                //pseudo-recursion: calling the same template function
                //but with another template argument
                return get_agg_max_count<n+1>();
        }

        //determines the amount of members
        //counting also nested array fields!
        //it's pseudo-recursive and invokes itself (with different template arguments) 
        template<size_t i, size_t max>
        consteval static size_t get_members_count_with_nested()
        {
            if constexpr (i == max)
                return 0;
            else
                //the trick: the 'i' gets moved by whatever 'get_nested_array_size'
                //returns for the field 'i'. it's 1 for normal, non-array fields.
                return 1 + get_members_count_with_nested<i + get_nested_array_size<i, max - i, 0>(), max>();
        }

        //just a convenience method to get the number of fields
        //accounting for nested arrays
        consteval static size_t get_members_count()
        {
            return get_members_count_with_nested<0, get_agg_max_count()>();
        }
    };

    namespace for_each
    {
        //invoke a 'callback' for all given member field references
        //expected calling signature: void (size_t fieldIndex, auto const& field);
        template<size_t... idx>
        inline static void impl2(auto && callback, std::index_sequence<idx...> idxs, auto const&... m)
        {
            (callback(idx, m),...); 
        }

        //invoke a 'callback' for all given member field references
        inline static constexpr void impl(auto && callback, auto const&... m)
        {
            impl2(callback, std::make_index_sequence<sizeof...(m)>(), m...);
        }
    }

    //determine the sum size of all types passed
    //in a variadic template
    template<class... T>
    inline consteval size_t types_size_sum(T &&...)
    {
        return (sizeof(T) + ...);
    }
}
#endif
