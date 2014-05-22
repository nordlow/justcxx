/*! \file t_constexpr_prime.cpp
 * \brief
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>
#include <cstdlib>

constexpr bool is_prime_recursive(size_t number, size_t c)
{
    return (c*c > number) ? true :
        (number % c == 0) ? false :
        is_prime_recursive(number, c+1);
}

constexpr bool is_prime(size_t number)
{
    return (number <= 1) ? false : is_prime_recursive(number, 2);
}


using std::cout;
using std::endl;
using std::hex;
using std::dec;

int main(int argc, const char * argv[], const char * envp[])
{
    bool x = is_prime(86028121);
    cout << x << endl;
    return 0;
}
