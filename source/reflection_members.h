#ifndef REFL_MEMBERS_H_
#define REFL_MEMBERS_H_

#include "reflection_basic.h"

namespace refl
{
    //main temlpate struct 'members'
    //undefined in general case
    template<class T, size_t N>
    struct members
    {
        static_assert(N, "Too many member fields! Need more MEMBERS_IMPL");
    };

    //defines a macro to simplify the definition
    //of template specializations for different amount
    //of member fields of a struct
#define MEMBERS_IMPL(N, ...) \
    template<class T> \
    struct members<T, N> \
    { \
        template<class CB> \
        static constexpr void for_each(T const& v, CB && callback) \
        { \
            auto const& [__VA_ARGS__] = v; \
            for_each::impl(std::forward<CB>(callback), __VA_ARGS__); \
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
MEMBERS_IMPL(10, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9);
MEMBERS_IMPL(11, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10);
MEMBERS_IMPL(12, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11);
MEMBERS_IMPL(13, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12);
MEMBERS_IMPL(14, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13);
MEMBERS_IMPL(15, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14);
MEMBERS_IMPL(16, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15);
MEMBERS_IMPL(17, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16);
MEMBERS_IMPL(18, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17);
MEMBERS_IMPL(19, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18);
MEMBERS_IMPL(20, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19);
}

#endif
