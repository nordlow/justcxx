/*! \file t_implicit_cast.cpp
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
    int x = 0x1111222233334444ULL;
    int y = static_cast<unsigned long>(0x1111222ULL);
    int z = static_cast<unsigned long>(0x111122223333ULL);
    cout << x << y << z << endl;
    return 0;
}
