#include "polysolve.hpp"
#include "utils.h"

template<class T>
void test_polysolve(void)
{
    T a1 = 1.0, b1 = -10.5, c1 = 32.0, d1 = -30.0;
    T a2 = 1.0, b2 = -4.5, c2 = 17.0, d2 = -30.0;
    T x[3];
    int nsolutions;
    polysolve3rd(a1, b1, c1, d1, &nsolutions, x); /* should get 3 solutions: 2, 6 & 2.5   */
    darray_print(x, 3);
    polysolve3rd(a2, b2, c2, d2, &nsolutions, x); /* should get 1 solution: 2.5           */
    darray_print(x, 1);
}

int
main(int argc, char *argv[])
{
    test_polysolve<double>();
    return 0;
}
