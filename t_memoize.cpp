#include <iostream>
#include <string>
#include <map>
#include "memoize.hpp"
#include "enforce.hpp"

template<typename T>
inline
T sum(const T & x, const T & y)
{
    std::cout << "sum(" << x << "," << y << ")" << std::endl;
    return x+y;
}

template<typename T>
inline
T times(const T & x, const T & y)
{
    std::cout << "times(" << x << "," << y << ")" << std::endl;
    return x*y;
}

/*! Unit Test \c memoize(). */
template<typename T>
inline void test_memoize()
{
    enforce_eq(memoized_recursion(sum<T>)(11,22), 33);
    enforce_eq(concurrent_memoize(std::function<T (T, T)>(sum<T>))(3,5), 8);
}

int main(int argc, char * argv[], char * envp[])
{
    test_memoize<int>();
    test_memoize<float>();
    test_memoize<double>();
    return 0;
}
