/*! \file t_namespace.cpp
 * \brief
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

namespace a {
void sqr(int x) { cout << "Got int " << x << endl; }
}

namespace b {
void sqr(float x) { cout << "Got float " << x << endl; }
}

int main(int argc, const char * argv[], const char * envp[])
{
    using namespace a;
    using namespace b;
    sqr(11);
    sqr(11.0f);
    return 0;
}
