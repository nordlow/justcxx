/*! \file t_boost_geometry.cpp
 * \brief Test Compilation Performance on Boost.Geometry.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

using namespace boost::geometry;

using std::cout;
using std::endl;
using std::hex;
using std::dec;

BOOST_GEOMETRY_REGISTER_C_ARRAY_CS(cs::cartesian)

BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

template<class T>
int test_boost_geometry_basic(int argc, const char * argv[], const char * envp[])
{
    model::d2::point_xy<int> p1(1, 1), p2(2, 2);
    std::cout << "Distance p1-p2 is: " << distance(p1, p2) << std::endl;

    int a[2] = {1,1};
    int b[2] = {2,3};
    T d = distance(a, b);
    std::cout << "Distance a-b is: " << d << std::endl;

    T points[][2] = {{2.0, 1.3}, {4.1, 3.0}, {5.3, 2.6}, {2.9, 0.7}, {2.0, 1.3}};
    model::polygon<model::d2::point_xy<T> > poly;
    append(poly, points);
    boost::tuple<T, T> p = boost::make_tuple(3.7, 2.0);
    std::cout << "Point p is in polygon? " << std::boolalpha << within(p, poly) << std::endl;

    std::cout << "Area: " << area(poly) << std::endl;

    T d2 = distance(a, p);
    std::cout << "Distance a-p is: " << d2 << std::endl;

    return 0;
}

template<class T>
int test_boost_geometry_spherical_equatorial(int argc, const char * argv[], const char * envp[])
{
    typedef boost::geometry::model::point
        <
        T, 2, boost::geometry::cs::spherical_equatorial<boost::geometry::degree>
        > spherical_point;

    spherical_point amsterdam(4.90, 52.37);
    spherical_point paris(2.35, 48.86);

    T const earth_radius = 3959; // miles
    std::cout << "Distance in miles: " << distance(amsterdam, paris) * earth_radius << std::endl;

    return 0;
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_boost_geometry_basic<float>(argc, argv, envp);
    test_boost_geometry_spherical_equatorial<float>(argc, argv, envp);
    return 0;
}
