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
    const uint64_t x = 1;
    const auto n = 64;

    assert(rotl64(x, 64) == 1);
    for (size_t i = 0; i != n; ++i)
    {
        assert(rotr64(rotl64(x, i), i) == 1);
        assert(rotl64(rotr64(x, i), i) == 1);
    }

    return 0;
}
