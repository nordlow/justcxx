/*! \file foo.cpp
 * \brief
 */

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

#include <boost/date_time/posix_time/posix_time.hpp>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

int main(int argc, const char * argv[], const char * envp[])
{
    const auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << std::put_time(std::localtime(&now_c), "%F %T.%S") << '\n';

    boost::posix_time::ptime t = boost::posix_time::microsec_clock::local_time();
    cout << t << endl;
    return 0;
}
