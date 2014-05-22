/*! \file f.cpp
 * \brief Test GNU extension \c feenableexcept.
 */

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif
#include "fenv.h"

#include <iostream>
#include <string>
#include <cmath>

using std::cout;
using std::endl;

void trigger_fpe() {
    cout << std::pow(10, -10000) << endl;
    cout << std::pow(10, +10000) << endl;
    cout << std::sqrt(-1.0) << endl;
    cout << 1.0 / 0.0 << endl;
}

int main(int argc, const char * argv[], const char * envp[])
{
    trigger_fpe();
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW);
    trigger_fpe();
    return 0;
}
