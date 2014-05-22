#include "algorithm_x.hpp"
#include "show.hpp"
#include "crossover.hpp"
#include "ramp.hpp"

#include <iostream>
#include <vector>

using std::cout;
using std::cin;
using std::endl;

void test_crossover(uint n = 8)
{
    typedef std::vector<int> T;

    T a(n);
    T b(n);

    ramp(a, 10, 1);
    ramp(b, 200, 1);

    cout << "a: " << a << endl;
    cout << "b: " << b << endl;

    T c, d;

    noncyclic_twoway_random_crossover(a, b, c, d);
    cout << "c: " << c << endl;
    cout << "d: " << d << endl;
}

int main(int argc, char *argv[])
{
    while (true)
    {
	test_crossover();
	cout << endl;
    }
    return 0;
}
