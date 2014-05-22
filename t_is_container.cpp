/*! \file t_is_container.cpp
 * \brief Test is_container type_trait.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include "type_traits_x.hpp"
#include "enforce.hpp"

using std::cout;
using std::endl;
using std::hex;
using std::dec;

int main(int argc, const char * argv[], const char * envp[])
{
    enforce(is_container<std::vector<int>>::value);
    enforce(is_container<std::list<int>>::value);
    enforce(is_container<std::set<int>>::value);
    enforce_zero(is_container<int>::value);
    return 0;
}
