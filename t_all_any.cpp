/*! \file t_chain.cpp
 * \brief
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>
#include <vector>
#include "geometry/vec.hpp"
#include "algorithm_x.hpp"
#include "enforce.hpp"

int main(int argc, const char * argv[], const char * envp[])
{
    using namespace std;
    using pnw::is_defined;

    // Pointer to null
    int* p = nullptr;
    enforce_zero(is_defined(p));

    // Pointer stack
    int x = 13;
    int* xp = &x;
    enforce(is_defined(xp));

    enforce(is_defined(vec2f(0, 1)));
    enforce(is_defined(vec2f(1)));

    enforce_zero(all(vec2f(0, 1)));
    enforce(all(vec2f(1, 1),
                vec2f(1, 1)));
    enforce(any(vec2f(0, 1)));
    enforce_zero(any(vec2f(0, 0)));

    enforce_zero(is_defined(0));
    enforce(is_defined(1));
    enforce(is_defined(string("s")));
    enforce_zero(is_defined(string("")));

    typedef vector<int> VI;
    const VI v12 = { 1, 2 };
    const VI v_;
    enforce(is_defined(any(v12, v12)));
    enforce(is_defined(any(v12, v_)));
    enforce(is_defined(any(v_, v12)));
    enforce_zero(is_defined(all(v12, v_)));

    enforce_eq(any(0, 11), 11);
    enforce_eq(any(11, 0), 11);
    enforce_eq(any(1.5, 2.5f), 1.5f);
    enforce_eq(all(1, 0), 0);
    enforce_eq(all(1, 2), 2);
    enforce_eq(all(1.0, 2.0), 2.0);

    enforce(is_defined(all(string("a"),
                           string("b"),
                           string("c"))));

    enforce_zero(is_defined(all(string("a"),
                                string("b"),
                                string(""))));

    const auto r = any(string("a"),
                       string("b"),
                       string(""));
    enforce(is_defined(r));
    enforce_eq(r, string("a"));

    return 0;
}
