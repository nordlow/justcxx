#include "algorithm_x.hpp"
#include "show.hpp"
#include "resample.hpp"

#include <iostream>

using std::cout;
using std::endl;

void test_sample()
{
    uint n = 11;
    double a = -9.9, min = 0.0, max = 10.0, step = (max - min) / (n-1);
    uint i = uniform_resample_idx(a, min, max, n);
    double b = min + i * step;

    cout << "a:    " << a << endl
         << "min:  " << min << endl
         << "max:  " << max << endl
         << "step: " << step << endl
         << "n:    " << n << endl
         << "i:    " << i << endl
         << "b:    " << b << endl;
}

int main(int argc, char *argv[])
{
    test_sample();
    return 0;
}
