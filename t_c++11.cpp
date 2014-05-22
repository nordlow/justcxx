/*! \file t_c++11.cpp
 * \brief Deterministic Result but with Non-Deterministic Execution Time.
 */

#include <iostream>
#include <future>
#include <vector>
#include <string>

using namespace std;

int main(int argc, const char * argv[], const char * envp[])
{
    vector<future<string>> v;

    // async calls execute in this or other thread depending on time-complexity
    v.push_back(async([] { return string("first "); }));
    v.push_back(async([] { return string("second "); }));
    v.push_back(async([] { return string("third "); }));

    for (auto& i : v) {
        cout << i.get();
    }
    cout << endl;
    return 0;
}
