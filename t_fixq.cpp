#include "algorithm_x.hpp"
#include "show.hpp"
#include "FixQ.hpp"

#include <iostream>

using std::cout;
using std::cin;
using std::endl;

void test_fixq(uint az, uint ap,
	       uint bz, uint bp)
{
    typedef FixQ<ushort, 4> T;

    T a(az, ap);
    T b(bz, bp);

    cout << "a: " << a << endl;
    cout << "b: " << b << endl;

    T c(a + b);
    cout << "c: " << c << endl;
}

int main(int argc, char *argv[])
{
    while (true)
    {
	uint az, ap, bz, bp;

	cout << "az: "; cin >> az;
	cout << "ap: "; cin >> ap;

	cout << "bz: "; cin >> bz;
	cout << "bp: "; cin >> bp;

	test_fixq(az, ap,
		  bz, bp);
    }
    return 0;
}
