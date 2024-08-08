#include <string>
#include <string_view>
#include <format>
#include <print>
#include <charconv>
#include <system_error>
#include <complex>
#include <source_location>
#include <type_traits>

#include "reflection.h"


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
