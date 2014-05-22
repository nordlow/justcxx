/*! \file t_boost_lexical_cast.cpp
 * \brief Test boost lexical cast
 */

#include <iostream>
#include <string>
#include "cc_features.h"
#include <boost/lexical_cast.hpp>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

template<class T> std::string to_str(const T& x) { return boost::lexical_cast<std::string>(x); }
template<class T> int to_int(const T& x) { return boost::lexical_cast<int>(x); }

int main(int argc, const char * argv[], const char * envp[])
{
    auto x = to_str(12);
    return 0;
}
