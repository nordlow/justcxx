#include "algorithm_x.hpp"
#include "show.hpp"
#include "norm.hpp"

#include <iostream>

template<typename T>
void test_lnnorm(uint n = 2)
{
    T b = 3, c = 4, d = 5, e = 6, f = 7;
    using std::cout;
    using std::endl;
    cout << lnnorm2d(b, c, n) << endl
         << lnnorm3d(b, c, d, n) << endl
        ;
}

int main(int argc, char *argv[])
{
    test_lnnorm<float>();
    test_lnnorm<double>();
    return 0;
}
