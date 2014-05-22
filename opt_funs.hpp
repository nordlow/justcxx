#pragma once
#include <algorithm>
#include <ostream>
#include "nelder_mead.hpp"
#include "math_x.hpp"
#include "cc_features.h"

inline void timestamp() {}

using std::sqr;
using std::quart;

/*! Rosenbrock Parabolic Value.
 * Global Minimum at (1,1).
 * \see https://en.wikipedia.org/wiki/Rosenbrock_function
 */
template<class T> inline spure T rosenbrock(T x0, T x1) {
    const T fx1 = x1 - sqr(x0);
    const T fx2 = 1.0 - x0;
    return 100.0 * sqr(fx1) + sqr(fx2);
}
template<class T> inline pure T rosenbrock(const T x[2]) { return rosenbrock(x[0], x[1]); }

/*! Powell Quartic.
 * Global minimum at (0,0,0,0). */
template<class T> inline spure T powell(T x0, T x1, T x2, T x3) {
    const T fx1 = x0 + 10.0 * x1;
    const T fx2 = x2 - x3;
    const T fx3 = x1 - 2.0 * x2;
    const T fx4 = x0 - x3;
    return (sqr(fx1) +
            5.0 * sqr(fx2) +
            quart(fx3) +
            10.0 * quart(fx4));
}
template<class T> inline T powell(const T x[4]) { return powell(x[0], x[1], x[2], x[3]); }

/*! Fletcher-Powell Helical Valley. */
template<class T> inline T helical(T x0, T x1, T x2) {
    T theta;
    if        (0.0 < x0)  { theta = atan2(x1, x0) / 2.0 / M_PI;
    } else if (x0 < 0.0)  { theta = 0.5 + atan2(x1, x0) / 2.0 / M_PI;
    } else if (x0 == 0.0) { theta = 0.25; }
    const T fx1 = x2 - 10.0 * theta;
    const T fx2 = sqrt(sqr(x0) + sqr(x1));
    const T fx3 = x2;
    return (100.0 *
            sqr(fx1) +
            sqr(fx2) +
            sqr(fx3));
}
template<class T> inline T helical(const T x[3]) { return helical(x[0], x[1], x[2]); }

/*! Sum of \p p:th powers. */
template<class T> inline T powsum(const T a[10], int p) {
    return std::accumulate(a, a+10, 0,
                           [p](T x, T y) {
                               return x + ((p == 4) ? quart(y) : std::pow(y, p)) ;
                           });
}
template<class T> inline T quartic(const T x[10]) { return powsum(x, 4); }
template<class T> inline T pentic(const T x[10]) { return powsum(x, 5); }
