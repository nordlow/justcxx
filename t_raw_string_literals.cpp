/*! \file t_raw_string_literals.cpp
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

int main(int argc, const char * argv[], const char * envp[])
{
    std::string s = R"X*X(First line
Second line
)X*X";
    cout << s;
    return 0;
}
