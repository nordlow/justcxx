/*! \file f.cpp
 * \brief Test aritmetic behaviour on signed and unsigned integers.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
#endif

#include <type_traits>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

int main(int argc, const char * argv[], const char * envp[])
{
    const int i = -1;
    const uint ui = 2;
    const auto i_ui = i + ui;
    const auto ui_i = ui + i;
    typename std::common_type<int, uint>::type ct1;
    typename std::common_type<uint, int>::type ct2;
    return 0;
}
