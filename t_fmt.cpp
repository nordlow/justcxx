/*! \file t_fmt.cpp
 * \brief Test fmtlib.net.
 */

#include <string>

#include <vector>
#include <experimental/vector>

#include <unordered_set>
#include <experimental/unordered_set>

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

    for (const auto &e : v) { fmt::print(stdout, "{},", e); }
    fmt::print("\n");

    erase_if(v, [](int x){return x == 12;});

    for (const auto &e : v) { fmt::print(stdout, "{},", e); }
    fmt::print("\n");

    unordered_set<int> s = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};

    for (const auto &e : s) { fmt::print(stdout, "{},", e); }
    fmt::print("\n");

    erase_if(s, [](int x){return x == 12;});

    for (const auto &e : s) { fmt::print(stdout, "{},", e); }
    fmt::print("\n");

    return 0;
}
