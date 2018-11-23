/*! \file f.cpp
 * \brief
 */

#include <iostream>
#include <memory>
#include <unordered_map>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

using SharedInt = std::shared_ptr<const int>;

inline bool operator == (const SharedInt &a,
                         const int *b)
{
    return (a.get() == b);
}

inline bool operator == (const int *a,
                         const SharedInt &b)
{
    return (a == b.get());
}

int main(int argc, const char *argv[], const char *envp[])
{
    std::unordered_map<SharedInt, double> up;

    SharedInt si;
    auto it1 = up.find(si);

    const int *ip;
    auto it2 = up.find(ip);

    return 0;
}
