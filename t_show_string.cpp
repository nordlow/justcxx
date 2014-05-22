/*! \file t_show_string.cpp
 * \brief
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>
#include "show_string.hpp"

using std::cout;
using std::endl;
using std::hex;
using std::dec;
using std::string;

int main(int argc, const char * argv[], const char * envp[])
{
    string s("alpha");
    show(cout, s);
    cout << endl;
    return 0;
}
