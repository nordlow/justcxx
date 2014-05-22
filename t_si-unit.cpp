#include "si-unit.hpp"

#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    si::quantity<double, si::kilometre> s;
    si::quantity<double, si::hour> t;
    si::quantity<double, si::Speed> v;
    //v = t / s; // rejected
    v = s / t; // ok
    cout << v << endl;
    return 0;
}
