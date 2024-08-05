#include <string>
#include <string_view>
#include <format>
#include <charconv>
#include <system_error>
#include <complex>
#include <source_location>

namespace refl
{
    struct any
    {
	constexpr any(size_t i);
	template<class T>
	constexpr operator T();
    };

    template<class T>
    struct can_construct
    {
	template<size_t n>
	consteval static bool from()
	{
	    return []<size_t... i>(std::index_sequence<i...>)
	    {
		return requires { T{any(i)...,}; };
	    }(std::make_index_sequence<n>());
	}
    };
};

struct A
{
    int m1;
    float m2;
};

struct B
{
    char m1;
    int m2;
    char m3;
};

struct C: A
{
    char m3;
    char m4;
};

int main(int argc, char *argv[])
{
    C c{1, 2.f, 'f', 'v'};
    static_assert(refl::can_construct<C>::from<3>());
    return 0;
}
