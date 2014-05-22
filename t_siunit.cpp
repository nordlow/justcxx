/*! \file test_siunit.cpp
 * \brief Test SI Units.
 */

#include <iostream>
#include <string>
#include "units.hpp"
#include "show.hpp"

void test_siunit()
{
    pnw::siunit_show(1.1e-18); cout << endl;
    pnw::siunit_show(1.1e-15); cout << endl;
    pnw::siunit_show(1.1e-12); cout << endl;
    pnw::siunit_show(1.1e-9);  cout << endl;
    pnw::siunit_show(1.1e-6);  cout << endl;
    pnw::siunit_show(1.1e-3);  cout << endl;
    pnw::siunit_show(1.1e+0);  cout << endl;
    pnw::siunit_show(1.1e+3);  cout << endl;
    pnw::siunit_show(1.1e+6);  cout << endl;
    pnw::siunit_show(1.1e+9);  cout << endl;
    pnw::siunit_show(1.1e+12); cout << endl;
    pnw::siunit_show(1.1e+15); cout << endl;
    pnw::siunit_show(1.1e+18); cout << endl;
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_siunit();
    return 0;
}
