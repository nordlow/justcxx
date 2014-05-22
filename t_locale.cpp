/*!
 * \file t_locale.cpp
 * \brief Three Ways of doing case-conversions.
 * \see http://stackoverflow.com/questions/313970/stl-string-to-lower-case
 * \todo Use Boost.Locale present from Boost 1.48
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include <locale>
#include <iomanip>
#include "enforce.hpp"
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

int main(int argc, const char * argv[], const char * envp[])
{
    using std::cout;
    using std::endl;
    using std::string;

    string a = "ÅÄÖ";               // sample string
    std::locale loc("sv_SE.UTF-8"); // default locale

    auto do_enforce = true;

    cout << "original: " << a << endl;

    // Boost Simple
    {
        auto b = boost::to_lower_copy(a); // locale safe
        cout << "tolower: " << b << endl;
        if (do_enforce) enforce_neq(a, b);
        auto c = boost::to_upper_copy(b); // locale safe
        cout << "back: " << c << endl;
        if (do_enforce) enforce_eq(a, c);
    }

    // Boost Verbose
    {
        auto b = a;
        std::transform(begin(a), end(a), begin(b),
                       boost::bind(std::tolower<char>, _1, loc));
        cout << "tolower: " << b << endl;
        if (do_enforce) enforce_neq(a, b);
        string c(b);
        std::transform(begin(b), end(b), begin(c),
                       boost::bind(std::toupper<char>, _1, loc));
        cout << "back: " << c << endl;
        if (do_enforce) enforce_eq(a, c);
    }

    // C++
    {
        auto b = a;
        std::transform(begin(a), end(a), begin(b),
                       std::bind2nd(std::ptr_fun(&std::tolower<char>), loc));
        cout << "tolower: " << b << endl;
        if (do_enforce) enforce_neq(a, b);
        string c(b);
        std::transform(begin(b), end(b), begin(c),
                       std::bind2nd(std::ptr_fun(&std::toupper<char>), loc));
        cout << "back: " << c << endl;
        if (do_enforce) enforce_eq(a, c);
    }

    return 0;
}
