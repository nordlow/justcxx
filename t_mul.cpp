#include "algorithm_x.hpp"
#include "show.hpp"

#include <iostream>

using std::cout;
using std::endl;

void test_mul()
{
    int y;
    int x = 2;
    int p = 8;
    multiply(y, x, p);
    std::cout << y << std::endl;
}

int main(int argc, char *argv[])
{
    test_mul();
    return 0;
}
