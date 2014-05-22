/*! \file f.cpp
 * \brief Test Static Analysis in Compilers.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

int main(int argc, const char * argv[], const char * envp[])
{
    char x[2];
    x[2] = 1;
    const size_t i = 2;
    x[i] = 2;
    x[i - 1] = 2;
    x[i + 1] = 2;

    // nullptr dereference
    const char* p = nullptr;
    auto p_ = *p;

    return 0;
}
