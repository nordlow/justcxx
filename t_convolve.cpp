#include <iostream>

#include "algorithm_x.hpp"
#include "show.hpp"
#include "convolve.hpp"

using std::cout;
using std::endl;

template<typename T>
void test_convolve(int bn = 8, int cn = 4)
{
    std::vector<T> a(bn+cn-1);
    std::vector<T> b(bn, 1);
    std::vector<T> c(cn, 1);
    convolve(&a[0], &b[0], &c[0], bn, cn);
    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
}

int main(int argc, char *argv[])
{
    test_convolve<int>();
    return 0;
}
