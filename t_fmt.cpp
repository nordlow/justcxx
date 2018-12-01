/*! \file t_fmt.cpp
 * \brief
 */

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char * argv[],
         __attribute__((unused)) const char * envp[])
{
    vector<int> v = {11, 12, 13};
    for (const auto &e : v)
    {
        cout << e << endl;
    }
    return 0;
}
