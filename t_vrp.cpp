/*! \file t_vrp.cpp
 * \brief Test Value Range Propagation.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
#endif

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

int main(int argc, const char * argv[], const char * envp[])
{
    const unsigned char c1 = 200;
    const unsigned char c2 = 200;
    const int i = c1 + c2;
    cout << "i:" << i << endl;

    const unsigned char c3 = i & 0x3f;
    cout << "c3:" << static_cast<unsigned int>(c3) << endl;

    const short s = c3 + 1;
    cout << "s:" << static_cast<unsigned int>(s) << endl;

    return 0;
}
