#include <vector>
#include <iostream>

#include "pixels.hpp"
#include "show.hpp"

using std::cin;
using std::cout;
using std::endl;

typedef PixABGR32 TestPix;

void test_pixels()
{

    cout << "alpha: ";

    uint alpha;
    cin >> alpha;

    TestPix a(32, 0, 32, alpha);
    TestPix b(32, 32, 0, alpha);

    cout << "a: " << a << endl;
    cout << "b: " << b << endl;

    TestPix c = a + b;

    cout << "c: " << c << endl;
}

/*!
 * The following code demonstration motivates my choice of alpha
 * transparency calculations.
 */
void motivate_alpha()
{
    uchar x;
    cout << "x: ";

    uint i;
    cin >> i;
    x = i;

    uchar y1 = 256 - x;
    cout << "y1: " << static_cast<uint>(y1) << endl;

    uchar y2 = - x;
    cout << "y2: " << static_cast<uint>(y2) << endl;
}

int main(int argc, char *argv[])
{
    while (true)
    {
	motivate_alpha();
	cout << endl;
	test_pixels();
	cout << endl;
    }
    return 0;
}
