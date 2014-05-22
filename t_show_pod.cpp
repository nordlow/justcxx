/*! \file t_show_pod.cpp
 * \brief Test show of Plain Old Datatypes.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include "show_all.hpp"

int main(int argc, const char * argv[], const char * envp[])
{
    int x = 12;
    showln(x);
    return 0;
}
