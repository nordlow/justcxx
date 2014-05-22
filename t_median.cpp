/*!
 * \file t_median.cpp
 * \brief Test Median.
 */

#include <iostream>
#include <string>
#include <vector>
#include "algorithm_x.hpp"
#include "show.hpp"
#include "show_vector.hpp"
#include "enforce.hpp"

template<RAC T>
typename T::value_type test_median_element(T & x)
{
    std::cout << "x: " << x << std::endl;
    std::cout << "m: " << pnw::median_element(begin(x), end(x)) << std::endl;
    auto m = pnw::median_element(x);
    std::cout << "m: " << m << std::endl;
    std::cout << "x: " << x << std::endl;
    return m;
}

int main(int argc, const char * argv[], const char * envp[])
{
    typedef float T;
    {
        std::vector<T> x = {11};
        enforce_eq(test_median_element(x), 11);
    }
    typedef float T;
    {
        std::vector<T> x = {1,3,2};
        enforce_eq(test_median_element(x), 2);
    }
    {
        std::vector<T> x = {1,4,3,2,5,6};
        enforce_eq(test_median_element(x), 3.5);
    }
    return 0;
}
