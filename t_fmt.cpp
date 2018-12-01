/*! \file t_fmt.cpp
 * \brief Test fmtlib.net.
 */

#include <string>

#include <vector>
#include <experimental/vector>

#include <unordered_set>
#include <experimental/unordered_set>

// http://fmtlib.net: sudo apt install libfmt-dev
#include "fmt/format.h"
#pragma comment(lib, "fmt") // https://stackoverflow.com/questions/12821391/c-visual-studio-linking-using-pragma-comment
using namespace fmt;

using namespace std;
using namespace std::experimental;

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char * argv[],
         __attribute__((unused)) const char * envp[])
{
    // format("The answer is {}.", 42);
    // print(stderr, "System error code = {}\n", errno);

    vector<int> v = {11, 12, 13};
    print("{}\n", join(v, ","));
    erase_if(v, [](int x) { return x == 12; });
    print("{}\n", join(v, ","));

    unordered_set<int> s = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
    print("{}\n", join(s, ","));
    erase_if(s, [](int x) { return x == 12; });
    print("{}\n", join(s, ","));

    return 0;
}
