#include "algorithm_x.hpp"
#include "show.hpp"
#include "unpack.hpp"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;

void test_unpack(uint m = 3,
                 uint n = 4)
{
//     typedef std::vector<ushort> T;
    typedef std::vector<bool> T;

    uint mn = m * n;

    T src(mn);
    for (uint i = 0; i < mn; i++)
    {
        src[i] = std::rand() bitand 1;
    }
    cout << src << endl;

    T a, b;
    unpack(src,
           a, m,
           b, m);

    cout << a << endl
         << b << endl;
}

int main(int argc, char *argv[])
{
    test_unpack();
    return 0;
}
