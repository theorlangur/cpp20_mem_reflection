#include <string>
#include <string_view>
#include <format>
#include <print>
#include <charconv>
#include <system_error>
#include <complex>
#include <source_location>
#include <type_traits>

namespace refl
{
    struct any
    {
	constexpr any(size_t i);
	template<class T>
	constexpr operator T();
    };

    template<class T>
    struct info
    {
	template<size_t n>
	consteval static bool construct_from()
	{
	    static_assert(std::is_aggregate_v<T>, "Must be aggregatable");
	    return []<size_t... i>(std::index_sequence<i...>)
	    {
		return requires { T{any(i)...,}; };
	    }(std::make_index_sequence<n>());
	}

	template<size_t idx, size_t nested_size, size_t rest>
	consteval static bool construct_nested_array_from()
	{
	    return []<size_t... i, size_t... n, size_t... r>(std::index_sequence<i...>, std::index_sequence<n...>, std::index_sequence<r...>)
	    {
		return requires { T{any(i)..., {any(n)...}, any(r)...}; };
	    }(std::make_index_sequence<idx>(), std::make_index_sequence<nested_size>(), std::make_index_sequence<rest>());
	}

	template<size_t index, size_t sz, size_t rest>
	consteval static size_t get_nested_array_size()
	{
	    if constexpr (construct_nested_array_from<index, sz, rest>() && !construct_nested_array_from<index, sz, rest+1>())
		return sz;
	    else
		return get_nested_array_size<index, sz-1, rest+1>();
	}

	template<size_t n = 0>
	consteval static size_t get_agg_max_count()
	{
	    static_assert(n <= static_cast<size_t>(sizeof(T)));
	    if constexpr (construct_from<n>() && !construct_from<n+1>())
		return n;
	    else
		return get_agg_max_count<n+1>();
	}

	template<size_t i, size_t max>
	consteval static size_t get_members_count_with_nested()
	{
	    if constexpr (i == max)
		return 0;
	    else
		return 1 + get_members_count_with_nested<i + get_nested_array_size<i, max - i, 0>(), max>();
	}

	consteval static size_t get_members_count()
	{
	    return get_members_count_with_nested<0, get_agg_max_count()>();
	}
    };

    template<size_t... idx>
    inline static void for_each_mem_impl2(auto && callback, std::index_sequence<idx...> idxs, auto const&... m)
    {
	(callback(idx, m),...); 
    }

    inline static constexpr void for_each_mem_impl(auto && callback, auto const&... m)
    {
	for_each_mem_impl2(callback, std::make_index_sequence<sizeof...(m)>(), m...);
    }

    template<class... T>
    inline consteval size_t types_size_sum(T &&...)
    {
	return (sizeof(T) + ...);
    }

    template<class T, size_t N>
    struct members;

#define MEMBERS_IMPL(N, ...) \
    template<class T> \
    struct members<T, N> \
    { \
	template<class CB> \
	static constexpr void for_each(T const& v, CB && callback) \
	{ \
	    auto const& [__VA_ARGS__] = v; \
	    for_each_mem_impl(std::forward<CB>(callback), __VA_ARGS__); \
	} \
	template<class CB> \
	static constexpr auto fetch_all(T const& v, CB && callback) \
	{ \
	    auto const& [__VA_ARGS__] = v; \
	    return callback(__VA_ARGS__); \
	} \
	static consteval size_t all_fields_size() \
	{ \
	    auto const& [__VA_ARGS__] = T{}; \
	    return types_size_sum(__VA_ARGS__); \
	} \
    } 

MEMBERS_IMPL(1, f0);
MEMBERS_IMPL(2, f0, f1);
MEMBERS_IMPL(3, f0, f1, f2);
MEMBERS_IMPL(4, f0, f1, f2, f3);
MEMBERS_IMPL(5, f0, f1, f2, f3, f4);
MEMBERS_IMPL(6, f0, f1, f2, f3, f4, f5);
MEMBERS_IMPL(7, f0, f1, f2, f3, f4, f5, f6);
MEMBERS_IMPL(8, f0, f1, f2, f3, f4, f5, f6, f7);
MEMBERS_IMPL(9, f0, f1, f2, f3, f4, f5, f6, f7, f8);

    template<class T, class CB>
    inline constexpr void for_each_mem(T const& v, CB && callback)
    {
	members<T, refl::info<T>::get_members_count()>::for_each(v, std::forward<CB>(callback));
    }

    template<class T>
    inline constexpr auto fetch_all_mem(auto && callback)
    {
	return members<T, refl::info<T>::get_members_count()>::fetch_all(T{}, callback);
    }

    template<class T>
    inline consteval auto get_all_fields_size()
    {
	return members<T, refl::info<T>::get_members_count()>::all_fields_size();
    }
};

struct A
{
    int m1;
    float m2;
};

struct B
{
    char m1;
    char padding1[3];
    int m2;
    char m3;
    char padding2[3];
};

struct C: A
{
    char m3;
    char m4;
    double x;
};

struct D
{
    char m1;
    char padding[7];
    std::string m2;
};

struct E
{
    char m1;
    A a;
    char m3[2];
    std::string m2;
    A b;
};

constexpr size_t fields_size_sum(auto const&... fields)
{
    return (sizeof(fields) + ...);
}

int main(int argc, char *argv[])
{
    A a;
    a.m1 = 3;
    a.m2 = .2f;
    std::println("A: num of fields: {}", refl::info<A>::get_members_count());
    std::println("B: num of fields: {}", refl::info<B>::get_members_count());
    std::println("C: num of fields: {}", refl::info<C>::get_members_count());

    refl::for_each_mem(a, [](size_t idx, auto const& m){
	std::println("A: field {}. val: {}; sizeof={}", idx, m, sizeof(std::remove_cvref_t<decltype(m)>));
    });

    B b;
    b.m1 = 'a';
    b.m2 = 12;
    b.m3 = 'x';
    refl::for_each_mem(b, [](size_t idx, auto const& m){
	std::println("B: field {}. sizeof={}", idx, sizeof(std::remove_cvref_t<decltype(m)>));
    });
    constexpr auto field_size_sum = [](auto... fields){return (sizeof(decltype(fields)) + ...); };
    static_assert(sizeof(A) == refl::get_all_fields_size<A>());
    static_assert(sizeof(B) == refl::get_all_fields_size<B>());
    static_assert(refl::info<D>::construct_nested_array_from<0,1,2>());
    static_assert(refl::info<E>::get_members_count() == 5);
    /*static_assert(refl::info<E>::construct_nested_array_from<1,3,1>());*/
    /*static_assert(sizeof(D) == refl::get_all_fields_size<D>());*/
    //static_assert(sizeof(A) == refl::fetch_all_mem<A>(field_size_sum));
    //static_assert(sizeof(B) == refl::fetch_all_mem<B>(field_size_sum));
    return 0;
}
