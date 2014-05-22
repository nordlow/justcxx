/*! \file t_geometry.cpp
 * \brief Test Geometry Algorithms.
 */

#include <iostream>
#include <string>
#include <vector>

#include <armadillo>
#include <boost/units/pow.hpp>
#include <boost/units/systems/si/energy.hpp>
#include <boost/units/systems/si/force.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/electric_potential.hpp>
#include <boost/units/systems/si/current.hpp>
#include <boost/units/systems/si/resistance.hpp>
#include <boost/units/systems/si/io.hpp>

#include "geometry/all.hpp"
#include "interpolate.hpp"
#include "cyclic_interpolate.hpp"
#include "rand.hpp"
#include "show.hpp"
#include "show_array.hpp"
#include "show_vector.hpp"
#include "algorithm_x.hpp"
#include "math_x.hpp"
#include "enforce.hpp"
#include "numeric_x.hpp"
#include "cycle.h"

using std::cout;
using std::endl;

using namespace boost::units;
using namespace boost::units::si;

template<class T, std::size_t N>
void test_boxes()
{
    box<T,N> b1 = { { 11, 13},
                    { 17, 9 } };
    box<T,N> b2 = { { 13, 11},
                    { 9, 17 } };
    auto b12 = unite(b1,b2);
    auto b12_ = box<T,N>(min(b1.l(), b2.l()),
                         max(b1.u(), b2.u()));
    enforce_eq(b12, b12_);
    // auto bI = intersect(b1,b2);
    // dshowln(bI);
}

template<class T, std::size_t N>
void test_unit_boxes()
{
    const auto m = meter;       // constant shorthand
    typedef box<quantity<length,T>, N> length_box; // type shorthand
    const length_box b1 = { { 11*m, 13*m},
                            { 17*m, 9*m } };
    const length_box b2 = { { 13*m, 11*m},
                            { 9*m, 17*m } };
    auto b12 = unite(b1,b2);
    dshowln(b12);
    // const auto fac = quantity<si::dimensionless>(1.0);
    // dshowln(b1*fac);
    auto b12_ = length_box(min(b1.l(), b2.l()),
                           max(b1.u(), b2.u()));

    enforce_eq(b12, b12_);
}

template<class T, std::size_t N = 2>
void test_layout(size_t n = 5, T p = 0.01)
{
    box<T,N> b;                 // parent box
    auto uhg_b = uhg_layout(b, n, p); dshowln(uhg_b);
    auto uvg_b = uvg_layout(b, n, p); dshowln(uvg_b);
    auto urg_b = urg_layout(b, n, n/2, 2, p); dshowln(urg_b);
}

template<class T>
void test_shapes()
{
    using namespace std;
    dshowln(tri2<T>());
    dshowln(tri3<T>());
    dshowln(tri4<T>());
    dshowln(quad2<T>());
    dshowln(quad3<T>());
    dshowln(quad4<T>());
    dshowln(pent2<T>());
    dshowln(pent3<T>());
    dshowln(pent4<T>());

    vec2<T> origo2 = {0,0};
    vec3<T> origo3 = {0,0,0};
    vec4<T> origo4 = {0,0,0,0};

    mat2<T> dm2;                // default matrix
    dshowln(dm2);
    mat2<T> zm2(0);             // default matrix
    dshowln(zm2);

    mat3<T> dm3;                // default matrix
    dshowln(dm3);
    mat3<T> zm3(0);             // default matrix
    dshowln(zm3);

    mat4<T> dm4;                // default matrix
    dshowln(dm4);
    mat4<T> zm4(0);             // default matrix
    dshowln(zm4);

    dshowln(dm2*origo2);
    dshowln(dm3*origo3);
    dshowln(dm4*origo4);

    vec2<T> vv[3] = { {0,1}, {2,1}, {2,2},};
    auto vvn = find_nearest(vv,3, origo2);
    enforce_eq(vvn-vv, 0);      // first element is nearest
    auto vvm = find_farthest(vv,3, origo2);
    enforce_eq(vvm-vv, 2);      // third element is farthest

    vec2<T> vh(11.0, 12.0f);    // C++11 Heterogeneous Variadic Template Constructor
    dshowln(vh);

    vec2<T> v1 = { 11, 13}, v2 = { 17, 9 };
    dshowln(v1);
    dshowln(v2);
    dshowln(min(v1,v2));
    dshowln(max(v1,v2));
    const auto vmin = min(v1,v2);
    const auto vmax = max(v1,v2);
    enforce_eq(vmin, vec2<T>({ 11, 9 }));
    enforce_eq(vmax, vec2<T>({ 17, 13 }));

    // test rotation laws
    vec2<T> w = { 11, 12 };     // C++11 Initializer Lists
    enforce_eq(rot90(rotm90(w)), w); // duality
    enforce_eq(rotm90(rot90(w)), w); // duality
    enforce_eq(rot90(rot90(w)), -w);
    enforce_eq(rotm90(rotm90(w)), -w);
    enforce_eq(rot90(rot90(rot90(w))) , rotm90(w));
    enforce_eq(rotm90(rotm90(rotm90(w))) , rot90(w));

    vec3<T> v3;
    // rand(v3);
    sort(v3);

    dshowln(v3);
    dshowln(vec2<T>());
    dshowln(vec3<T>());
    dshowln(vec4<T>());

    dshowln(box2<T>(static_cast<T>(1.1)));
    dshowln(box2<T>());
    dshowln(box3<T>());

    dshowln(rbox2<T>());

    dshowln(line2<T>());

    dshowln(circle<T>());
    dshowln(sphere3<T>());

    dshowln(cring<T>());
    dshowln(sphere3_shell<T>());

    dshowln(hplane2<T>());
    dshowln(hplane3<T>());

    poly2<T> poly_ = { 1,2,3 };
    dshowln(poly_);

    std::vector<vec2<T> > v2s = { {0,0}, {1,2},
                                  {2,1}, {4,9} };
    auto v2s_ob = outer_box(v2s);
    dshowln(corners(v2s_ob));
    dshowln(corners(v2s_ob));
    dshowln(v2s);
    dshowln(v2s_ob);
    enforce_eq(v2s_ob, box2<T>({{0,0}, {4,9}}));

    enforce(separate(circle<T>(0,1),
                     box2<T>({{1,1},{2,2}})));

    box3<T> b3({{3,3,3}, {7,7,7}});
    dshowln(b3);
    dshowln(corners(b3));
}

template<class T>
void bench_shapes()
{
    vec2<T> w = { 11, 12 };     // C++11 Initializer Lists
    dshowln(w);

    const T xS = 13, yS = 17;         // scalars

    size_t nC = 10000;

    T xy;
    vec2<T> x(xS), y(yS);
    dshowln(x);
    dshowln(y);
    dshowln(-x);
    dshowln(normalize(x));
    dshowln(normalize(y));
    dshowln(l1norm(x));
    dshowln(sqrnorm(x));
    enforce(x == x);            // test opeator ==
    enforce(y == y);            // test operator ==
    enforce(x != y);            // test operator !=
    dshowln(sqrt(x));
    dshowln(abs(x));

    // test interpolation
    auto xyLI = linear_interpolate(x, y, static_cast<T>(0.5));
    dshowln(xyLI);
    enforce_eq(xyLI, pnw::mean(x, y));

    auto z = x; z += z;
    enforce_eq(x-x, 0);
    enforce_eq(x+x, x*2);
    enforce_eq(x/x, 1);
    enforce_eq(z, x*2);
    enforce_eq(std::norm_dot(x,y), 1);
    enforce_eq(std::angle(x,y), 0);

    dshowln(pow(x,2));

    // TODO: Template functionize to bench_variant in timed.hpp
    {
        std::vector<double> ticks_(nC);
        for (auto& e : ticks_) {
            auto tic = getticks();
            xy = norm_dot(x, y);
            e = elapsed(getticks(), tic);
        }
        cout << "vec-norm_dot min/mean/max:"
             << *std::min_element(ticks_) << "/"
             << pnw::mean_element(ticks_) << "/"
             << *std::max_element(ticks_) << endl;
    }
#if 0
    // Armadillo Version
    // TODO: This generates GCC Segfault!
    {
        typename arma::Col<T>::template fixed<2> xA(), yA();
        std::vector<double> ticks_(nC);
        for (auto& e : ticks_) {
            auto tic = getticks();
            //xy = norm_dot(xA, yA);
            e = elapsed(getticks(), tic);
        }
        cout << "arma-norm_dot min/mean/max:"
             << *std::min_element(ticks_) << "/"
             << pnw::mean_element(ticks_) << "/"
             << *std::max_element(ticks_) << endl;
    }
    //arma::frowvec::fixed<2> xA, yA;
    // cout << "vec2/vec ratio:" << elapsed(toc, tic) / elapsed(toc_, tic_) << endl;
#endif

    dshowln(xy);
}

template<class T>
void test_geometry_armadillo_integration()
{
    enforce_eq((vec<T,2>(arma::Col<T>({ 1.1, 2.2 }))),
               (vec<T,2>({ 1.1, 2.2 })));
    enforce_eq((vec<T,2>(arma::Row<T>({ 1.1, 2.2 }))),
               (vec<T,2>({ 1.1, 2.2 })));
}

int main() {
    test_boxes<float,2>();
    test_unit_boxes<double,2>();
    test_shapes<float>();
    test_layout<float>();
    bench_shapes<float>();
    test_geometry_armadillo_integration<float>();
    return 0;
}
