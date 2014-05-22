/*! \file t_enforce.cpp
 * \brief
 */

#include <iostream>
#include <string>
#include <vector>
#include "enforce.hpp"

int main(int argc, const char * argv[], const char * envp[])
{
    typedef std::vector<double> V;
    enforce_lt(1, 2);
    enforce_eq3(3, 3, 3);
    enforce_eq(V({1, 2, 3, 4, 5}),
               V({1, 2, 3, 4, 5}));
    enforce_eq3(V({1, 2, 3, 4, 5}),
                V({1, 2, 3, 4, 5}),
                V({1, 2, 3, 4, 5}));
    enforce_eq4(V({1, 2, 3, 4, 5}),
                V({1, 2, 3, 4, 5}),
                V({1, 2, 3, 4, 5}),
                V({1, 2, 3, 4, 5}));
    return 0;
}
