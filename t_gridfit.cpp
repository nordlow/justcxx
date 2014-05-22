#include "algorithm_x.hpp"
#include "show.hpp"
#include "show_vector.hpp"
#include "gridfit.hpp"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;

void test_gridfit()
{
    typedef float T;
    typedef std::vector<T> Vec;

    using std::cout;
    using std::endl;

    Vec d = { 12.8,
              12.9,
              13.0,
              11.1,
              12.2,
              13.3,
              13.4,
              13.5 };
    cout << d << endl;
    auto x = gridfit(d, static_cast<T>(1.0));
    cout << x << endl;
}

int main(int argc, char *argv[])
{
    test_gridfit();
    return 0;
}
