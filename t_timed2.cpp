/*! \file t_timed2.cpp
 * \brief
 */

#include <iostream>
#include <string>
#include <chrono>
#include "timed.hpp"

using std::cout;
using std::endl;

int main(int argc, const char * argv[], const char * envp[])
{
    using clock_type = std::chrono::high_resolution_clock;
    auto tU = pnw::timed().call<clock_type>([&] { usleep(1); });
    cout << "usleep(1) took " << std::chrono::duration_cast<std::chrono::microseconds>(tU).count() << " microseconds" << endl;
    auto tS = pnw::timed().call<clock_type>([&] { sleep(1); });
    cout << "sleep(1) took " << std::chrono::duration_cast<std::chrono::microseconds>(tS).count() << " microseconds" << endl;
    return 0;
}
