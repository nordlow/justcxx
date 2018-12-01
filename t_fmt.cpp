/*! \file t_fmt.cpp
 * \brief Test fmtlib.net.
 */

#include <string>
#include <vector>
#include <experimental/vector>

// http://fmtlib.net
#include "fmt/format.h"
#pragma comment(lib, "fmt") // https://stackoverflow.com/questions/12821391/c-visual-studio-linking-using-pragma-comment

using namespace std;
using namespace std::experimental;

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char * argv[],
         __attribute__((unused)) const char * envp[])
{
    // fmt::format("The answer is {}.", 42);
    // fmt::print(stderr, "System error code = {}\n", errno);

    vector<int> v = {11, 12, 13};

    for (const auto &e : v)
    {
        fmt::print(stdout, "{},", e);
    }
    fmt::print("\n");

    erase_if(v, [](int x){return x == 12;});

    for (const auto &e : v)
    {
        fmt::print(stdout, "{},", e);
    }
    fmt::print("\n");

    return 0;
}
