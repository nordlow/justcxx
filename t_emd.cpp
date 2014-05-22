/*!
 * \file t_emd.cpp
 * \brief
 */

#include <iostream>
#include <string>

#include "randomize.hpp"
#include "emd.hpp"
#include "fshow.hpp"
#include <vector>
#include <iomanip>

using std::cout;
using std::endl;

int main(int argc, const char * argv[], const char * envp[])
{
    n = 32;
    std::vector<float> x(n);
    pnw::rand_n(x, 0,1);
    emd e(x);
    rand(x,0,1);
    cout << x << endl;
    return 0;
}
