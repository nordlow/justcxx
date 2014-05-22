/*! \file t_outer_sphere.cpp
 * \brief Test Outer Sphere Algorithm.
 */

#include "geometry/outer_sphere.hpp"
#include "math_x.hpp"
#include "rand.hpp"
#include "show.hpp"
#include <iostream>

using std::cout;
using std::endl;

template<class T>
inline int test_outer_sphere(int num = 1000000)
{
    std::vector<vec3<T> > points(num);

    rand(points);
    auto sphere = outer_sphere<T>(points.data(), num);
    dshowln(sphere);
    // using std::sqr;
    // T max_dist = 0;
    // for (int i = 0; i < num; i++) {
    //     T dist = sqrt(sqr(points[i].x - sphere.c().x) +
    //                   sqr(points[i].y - sphere.c().y) + sqr(points[i].z - sphere.c().z));
    //     max_dist = std::max(max_dist, dist);
    // }
    // lprintf("%f\n", max_dist);
    return 0;
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_outer_sphere<float>();
    return 0;
}
