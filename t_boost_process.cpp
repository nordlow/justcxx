/*! \file t_boost_process.cpp
 * \brief Test Boost.Process 0.5
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>
#include <boost/process.hpp>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

using namespace boost::process;
using namespace boost::process::initializers;

int main(int argc, const char * argv[], const char * envp[])
{
    auto c = execute(run_exe("/usr/bin/evince"), inherit_env());
    auto exit_code = wait_for_exit(c);
    // terminate(c)
    return 0;
}
