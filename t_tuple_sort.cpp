/*! \file t_tuple_sort.cpp
 * \brief Test tuple_sort.hpp
 * \see http://en.cppreference.com/w/cpp/utility/tuple/tie
 */

#include <iostream>
#include <string>
#include "tuple_sort.hpp"

int main(int argc, const char * argv[], const char * envp[]) {
    using std::cout;
    using std::endl;
    int a, b, c, d, e;
    std::tie(a, b, c, d, e) = order(4, 3, 2, 1, 8);
    cout << "a = " << a << endl
         << "b = " << b << endl
         << "c = " << c << endl
         << "d = " << d << endl
         << "e = " << e << endl;
}
