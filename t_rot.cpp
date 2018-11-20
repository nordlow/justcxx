/*! \file t_rot.cpp
 * \brief
 */

#include <iostream>
#include <string>
#include <cassert>
#include "rotate.h"

using namespace std;

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char * argv[],
         __attribute__((unused)) const char * envp[])
{
    uint64_t x = 1;
    const auto n = 64;
    assert(rotl64(x, 64) == 1);
    for (size_t i = 0; i != n; ++i)
    {
        cout << rotl64(x, i) << endl;
    }
    return 0;
}
