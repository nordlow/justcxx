/*! \file t_angle.cpp
 * \brief Test angles.
 */

#include <ostream>
#include <string>
#include "angles.hpp"
#include "enforce.hpp"

using std::cout;
using std::endl;
using std::hex;
using std::dec;

int main(int argc, const char* argv[], const char* envp[])
{
    using namespace angles;
    testAll<double>();
    return 0;
}
