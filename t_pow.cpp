#include "algorithm_x.hpp"
#include "show.hpp"
#include "pow.hpp"

#include <iostream>

using std::cout;
using std::endl;

void test_pow()
{
    int y, x = 2, p = 8;
    iter_pow(y, x, p);
    cout << y << endl;
}

int main(int argc, char *argv[])
{
    test_pow();
    return 0;
}
