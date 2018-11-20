/*! \file foo.cpp
 * \brief
 */

#include <iostream>
#include <string>
#include <variant>
#include <array>
#include <vector>
#include <unordered_set>

using namespace std;

using V = vector<size_t>;
using A = array<size_t, 2>;
using S = unordered_set<size_t>;
using X = variant<V, A, S>;

int main(int argc, const char * argv[], const char * envp[])
{
    cout << sizeof(V) << endl;
    cout << sizeof(A) << endl;
    cout << sizeof(S) << endl;
    cout << sizeof(X) << endl;
    return 0;
}
