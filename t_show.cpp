/*! \file t_show.cpp
 * Test show.
 */

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <bitset>
#include <string>
#include <tuple>
#include <tr2/dynamic_bitset>
#include <boost/dynamic_bitset.hpp>
#include "rand.hpp"
#include "geometry/vec.hpp"
#include "geometry/box.hpp"
#include "show_all.hpp"

using std::cout;
using std::endl;
using std::string;

template<class C>
inline void test_rand_show() {
    C a;
    // cout << rand(a) << endl;
}
template<class C>
inline void test_rand_n_show(size_t n = 10) {
    cout << rand_n<C>(n) << endl;
    C c(n);
    rand(c);
}

int main(int argc, const char * argv[], const char * envp[])
{
    std::cout << std::setprecision(2);
    test_rand_show<std::tuple<std::string,uint8_t,uint16_t> >();

    size_t n = 16;
    test_rand_n_show<std::vector<char> >(n);
    test_rand_n_show<std::vector<wchar_t> >(n);
    test_rand_n_show<std::vector<int8_t> >(n);
    test_rand_n_show<std::vector<uint8_t> >(n);
    test_rand_n_show<std::vector<int16_t> >(n);
    test_rand_n_show<std::vector<uint16_t> >(n);
    test_rand_n_show<std::vector<int32_t> >(n);
    test_rand_n_show<std::vector<uint32_t> >(n);
    test_rand_n_show<std::vector<int64_t> >(n);
    test_rand_n_show<std::vector<uint64_t> >(n);
    test_rand_n_show<std::vector<bool> >(n);
    test_rand_n_show<std::vector<std::pair<bool, bool > > >(n);
    test_rand_n_show<std::vector<std::pair<bool, int8_t> > >(n);
    test_rand_n_show<std::vector<std::pair<bool, vec2f> > >(3);
    // test_rand_n_show<std::vector<std::tuple<bool, int8_t> > >(n);
    test_rand_n_show<std::vector<std::string> >(n);

    std::bitset<64> bs; rand(bs); cout << bs << endl;
    std::tr2::dynamic_bitset<size_t> dbs(64); rand(dbs); cout << dbs << endl;
    boost::dynamic_bitset<size_t> dbs_(64); rand(dbs_); cout << dbs_ << endl;

    dshowln(n + n);

    string s("alpha");

    dshowln(12);
    dshowln('a');

    dshowln(s);
    show_n(11, 22, 33); cout << endl;

    showln(vec2f({1, 2}));
    showln(vec3f({1, 2, 3}));
    showln(vec4f({1, 2, 3, 4}));

    // showln(box2f({{1, 2}, {3, 4}}));
    return 0;
}
