#include "algorithm_x.hpp"
#include "show.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;

typedef std::vector<double> Vec;

template<class T>
std::vector<T> & operator += (std::vector<T> & a, const T & b)
{
    for (uint i = 0; i < a.size(); i++)
    {
        a[i] += b;
    }

    return a;
}

int main(int argc, char *argv[])
{
    Vec a(3), b(3);

    cout << "a: " << a << endl
         << "b: " << b << endl;

    a = b += 3.0;

    cout << "a: " << a << endl
         << "b: " << b << endl;

    return 0;
}
