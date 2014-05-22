/*!
 * \file t_peaks.cpp
 * \brief Test Peaks and Valleys.
 */

#include <iostream>
#include <string>
#include "algorithm_x.hpp"
#include "enforce.hpp"
#include "peaks.hpp"
#include "show_vector.hpp"
#include "show_list.hpp"

using std::cout;
using std::endl;

template<class T>
int test_peaks(int argc, const char * argv[], const char * envp[])
{
    {
        std::vector<T> xl = { 0,1,0,1,0 };
        cout << "x: " << xl << " peaks(xv): " << peaks(xl) << endl;
        std::list<T> xv = { 0,1,0,1,0 };
        cout << "x: " << xv << " peaks(xv): " << peaks(xv) << endl;
        enforce_eq(xl, xv);
    }
    return 0;
}

template<class T>
int test_valleys(int argc, const char * argv[], const char * envp[])
{
    {
        std::vector<T> xl = { 0,1,0,1,0 };
        cout << "x: " << xl << " valleys(xv): " << valleys(xl) << endl;
        std::list<T> xv = { 0,1,0,1,0 };
        cout << "x: " << xv << " valleys(xv): " << valleys(xv) << endl;
        enforce_eq(xl, xv);
    }
    return 0;
}

int main(int argc, const char * argv[], const char * envp[])
{
    typedef int T;
    return (test_peaks<T>(argc, argv, envp) +
            test_valleys<T>(argc, argv, envp));
}
