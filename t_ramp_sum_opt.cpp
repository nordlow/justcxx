// see: http://lwn.net/Articles/411776/

#include <iostream>
#include "cycle.h"

template<typename T>
inline
void ramp_sum(T a, const T & b, T & c)
{
    for (c = 0; a < b; a++)
        c += a;
}

/*! Test Compiler Optimization of \c ramp_sum.
 */
template<typename T>
int test_poly_opt(T n = 1000000000L)
{
    T c;
    ticks t0 = getticks();
    ramp_sum<T>(0, 2*n, c); // call it
    ticks t1 = getticks();
    std::cout << "Calculating " << c
              << " took " << elapsed(t1,t0) << std::endl; // print sum
    return 0;
}

int main(int argc, char *argv[])
{
    size_t n = 10000;
    test_poly_opt<uint16_t>(n);
    test_poly_opt<uint32_t>(n);
    test_poly_opt<uint64_t>(n);
    test_poly_opt<float>(n);
    test_poly_opt<double>(n);
    return 0;
}
