/*! \file t_uninitialized.cpp
 * \brief
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include "algorithm_x.hpp"

using std::cout;
using std::endl;
using std::hex;
using std::dec;

int main(int argc, const char * argv[], const char * envp[])
{
    using namespace pnw;
    showln(uninitialized<int8_t>());
    cout << uninitialized<int16_t>() << endl;
    cout << uninitialized<int32_t>() << endl;
    cout << uninitialized<float>() << endl;
    cout << uninitialized<double>() << endl;
    cout << uninitialized<long double>() << endl;
    return 0;
}
