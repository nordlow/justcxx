#include "algorithm_x.hpp"
#include "show.hpp"

#include "vec.hpp"
#include "rot2tab.hpp"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;

#define NUM 360

void test_rot2tab()
{
    typedef float T;
    typedef vec2<T> Vec;

    using std::cout;
    using std::endl;

    Vec x(1.0, 0.0);

    cout << "x: " << x << endl;

    rot2tab<T, NUM> m;

    for (int i = - 2 * NUM; i < 2 * NUM; i += NUM / 4)
    {
	cout << "i: " << i
	     << " mod " << NUM
	     << " = "
	     << std::modulo(i, NUM) << endl;

	Vec y = m.circular_lookup(i) * x;
	cout << "y: " << y << endl;
    }

}

int main(int argc, char *argv[])
{
    test_rot2tab();
    return 0;
}
