#include "algorithm_x.hpp"
#include "show.hpp"

#include <iostream>

using std::cout;
using std::endl;

void test_bits()
{
    uint x = std::numeric_limits<uint>::max();
    cout << "x=" << x << endl;
    for (uint i = 0; i <= 32; i++)
    {
	cout << std::dec
	     << "i=" << i
	     << " mask_bits(x," << i << ")="
	     << pnw::mask_bits(x, i) << endl;
    }
}

int main(int argc, char *argv[])
{
    test_bits();
    return 0;
}
