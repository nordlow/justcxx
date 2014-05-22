#include <iostream>

#include "algorithm_x.hpp"
#include "show.hpp"
#include "Roi.hpp"

using std::cout;
using std::endl;

typedef Roi_14_2 Region;

void print_stat(const Region & a)
{
    cout << a << endl;
    cout << "is_empty:" << a.is_empty() << " ";
    cout << "is_atomic:" << a.is_atomic() << " ";
    cout << "is_rational:" << a.is_rational() << " ";
    cout << "is_frac:" << a.is_frac() << endl << endl;
}

void test_roi()
{
    Region a((uint)0,
	     (uint)0,
	     (uint)1,
	     (uint)1);

    print_stat(a);

    Region b, c;

    a.row_split_to(b, c);
    print_stat(b);
    print_stat(c);
}

int main(int argc, char *argv[])
{
    test_roi();
    return 0;
}
