/*!
 * \file t_arrays.cpp
 * \brief
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

void f(int x[3])
{
    cout << sizeof(x) << endl;
}

int main(int argc, const char * argv[], const char * envp[])
{
    int x[4];
    x[4] = 0;
    f(x);
    return 0;
}
