#include <iostream>

using std::cout;
using std::cin;
using std::endl;

#define NBITS (4);

/*!
 * Contains a bitfield consisting of x, y.
 *
 * Illustrates the use of C bitfields.
 */
class Test
{
public:
    Test() : x(0), y(0) {}
public:
    int x:NBITS;
    int y:NBITS;
};

void test_bitfield()
{
    Test t;
    const uint n = 1 << NBITS;
    for (uint i = 0; i < n; i++, t.x++, t.y--)
    {
	cout << "x:" << t.x << " "
	     << "y:" << t.y << endl;
    }
}

int main(int argc, char *argv[])
{
    cout << "size: " << sizeof(Test) << endl;
    test_bitfield();
}
