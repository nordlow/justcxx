/*! \file t_strings.cpp
 * \brief Test Performance of String Variants.
 */

#include <iostream>
#include <string>
#include <ext/vstring.h>
#include <chrono>
#include "csc.hpp"
#include "enforce.hpp"
#include "type_traits_x.hpp"

typedef __gnu_cxx::__versa_string<char> vstring;

using std::cout;
using std::endl;

void test_strings()
{
    typedef std::chrono::high_resolution_clock C;
    typedef std::chrono::milliseconds P; // precision

    P x{0};
    const size_t n = 2e5;
    const char * str = "alpha";

    std::cout << is_string<decltype(str)>::value << std::endl;

    {
        std::string s(str);
        cout << s << ": sizeof(std::string): " << sizeof(s) << "; ";
        auto tot = s;
        const auto tA = C::now();
        for (size_t i = 0; i < n; i++) {
            tot += s + s + s + s;
        }
        const auto tB = C::now();
        auto dur = std::chrono::duration_cast<P>(tB - tA);
        cout << "tot.size(): " << tot.size() << " " << dur.count() << "ms" << endl;
    }

    {
        vstring s(str);
        cout << s << ": sizeof(__versa_string<char>): " << sizeof(s) << "; ";
        auto tot = s;
        const auto tA = C::now();
        for (size_t i = 0; i < n; i++) {
            tot += s + s + s + s;
        }
        const auto tB = C::now();
        auto dur = std::chrono::duration_cast<P>(tB - tA);
        cout << "tot.size(): " << tot.size() << " " << dur.count() << "ms" << endl;
    }

    {
        boost::const_string<char> s(str);
        enforce(equal(boost::const_string<char>("ab"), 'a', 'b')) ;
        cout << s << ": sizeof(const_string<char>): " << sizeof(s) << "; ";
        auto tot = s;
        const auto tA = C::now();
        for (size_t i = 0; i < n; i++) {
            tot += s + s + s + s;
        }
        const auto tB = C::now();
        auto dur = std::chrono::duration_cast<P>(tB - tA);
        cout << "tot.size(): " << tot.size() << " " << dur.count() << "ms" << endl;
    }
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_strings();
    return 0;
}
