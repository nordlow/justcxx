/*! \file t_typename.cpp
 * \brief
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <string>
#include "typenameof.hpp"
#include "show.hpp"

using std::cout;
using std::endl;
using std::hex;
using std::dec;

class SomeClass {
};

int main(int argc, const char * argv[], const char * envp[])
{
    int x;
    dshowln(typenameof<int>());
    dshowln(typeid(SomeClass).name());
    return 0;
}
