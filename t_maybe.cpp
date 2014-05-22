/*! \file t_maybe.cpp
 * \brief
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <complex>
#include "maybe.hpp"
#include "enforce.hpp"

using std::cout;
using std::endl;
using std::hex;
using std::dec;

/*! Test Corner Cases of Saturated Arithmetic.
 */
template<class T, bool X = false>
void test_maybe(int argc, const char * argv[], const char * envp[])
{
    typedef maybe<T,X> M;
    M x;
    cout << "x as default contructed: " << x << endl;

    x = 1;
    cout << "x after initialized to 1: " << x << endl;
    enforce_eq(x, M(1));

    const auto y = x + M(1);
    enforce_eq(y, M(2));

    const auto z = y - M(1);
    enforce_eq(z, M(1));

    enforce_eq(x * M(3), M(3));
    enforce_eq(x * M(3) / M(3), M(1));

    auto x_copy = x;
    enforce_eq(x, x_copy);
    cout << "x_copy: " << x_copy << endl;

    x_copy += x_copy;

    cout << "x_copy*x_copy: " << x_copy*x_copy << endl;

    M xi(1);
    cout << "xi: " << xi << endl;
    cout << "xi+xi: " << xi + xi << endl;
    cout << "xi*xi: " << xi * xi << endl;
    cout << "xi/xi: " << xi / xi << endl;

    auto xi_copy = xi;
    cout << "xi_copy: " << xi_copy << endl;

    xi_copy.undefine();
    cout << "xi_copy: " << xi_copy << endl;
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_maybe<int, false>(argc, argv, envp);
    test_maybe<float, false>(argc, argv, envp);
    test_maybe<double, false>(argc, argv, envp);
    test_maybe<long double, false>(argc, argv, envp);
    test_maybe<std::complex<float>, false>(argc, argv, envp);
    test_maybe<std::complex<double>, false>(argc, argv, envp);
    return 0;
}
