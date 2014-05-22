/*! \file t_trim.cpp
 * \brief Test Trimming.
 */

#include <iostream>
#include <string>
#include "trim.hpp"
#include "enforce.hpp"

using std::cout;
using std::endl;

int main(int argc, const char * argv[], const char * envp[])
{
    using std::string;
    // test in-place
    { string s(" a "); in_place_ltrim(s); enforce_eq(s, "a "); in_place_rtrim(s); enforce_eq(s, "a"); }
    { string s(" a "); in_place_rtrim(s); enforce_eq(s, " a"); }
    { string s(" a ");  in_place_trim(s); enforce_eq(s, "a"); }
    // test functional
    enforce_eq(ltrim(string("   a   ")), "a   ");
    enforce_eq(rtrim(string("   a   ")), "   a");
    enforce_eq( trim(string("   a   ")), "a");
    return 0;
}
