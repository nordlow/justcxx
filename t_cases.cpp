/*! \file t_cases.cpp
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

int main(int argc, const char * argv[], const char * envp[]) {
    int x = 1;
    if (x == 1) {
        cout << "first" << endl;
    } else if (x == 1) {
        cout << "second" << endl;
    }
    return 0;
}
