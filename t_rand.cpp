/*! \file t_random.cpp
 * \brief Test my C++11 rand templates.
 */

#include <iostream>
#include <string>
#include <tr2/dynamic_bitset>

#include "rand.hpp"
#include "show.hpp"
#include "clamp.hpp"
#include "geometry/vec.hpp"
#include "geometry/box.hpp"
#include "geometry/sphere.hpp"
#include "geometry/polyn.hpp"
#include "geometry/rbox.hpp"
#include "show_all.hpp"

using std::cout;
using std::endl;

template<class T>
void test_rand()
{
    T x; rand(x); dshowln(x);
}

int main(int argc, const char * argv[], const char * envp[]) {

    test_rand<vec<float,2> >();
    test_rand<vec<float,3> >();
    test_rand<box<float,3> >();
    test_rand<sphere<float,3> >();
    test_rand<polyn<float,3, 4> >();
    test_rand<rbox<float,2> >();

    test_rand<bool>();

    test_rand<int8_t>();
    test_rand<int16_t>();
    test_rand<int32_t>();
    test_rand<int64_t>();

    test_rand<uint8_t>();
    test_rand<uint16_t>();
    test_rand<uint32_t>();
    test_rand<uint64_t>();

    test_rand<float>();
    test_rand<double>();
    test_rand<long double>();

    typedef std::vector<box<float,2> > V2;
    dshowln(rand_n<V2>(3));

    typedef std::vector<box<float,3> > V3;
    dshowln(rand_n<V3>(3));

    typedef std::vector<int32_t> VI32;
    { size_t n = 1e6; VI32 v(n); rand_n(v.data(), n); }

    typedef std::vector<uint32_t> VU32;
    { size_t n = 1e6; VU32 v(n); rand_n(v.data(), n); }

    typedef std::vector<int64_t> VI64;
    { size_t n = 1e6; VI64 v(n); rand_n(v.data(), n); }

    typedef std::vector<uint64_t> VU64;
    { size_t n = 1e6; VU64 v(n); rand_n(v.data(), n); }

    return 0;
}
