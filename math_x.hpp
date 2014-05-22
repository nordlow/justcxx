/*!
 * \file math_x.hpp
 * \brief Mathematics (math.h) Extensions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <cmath>
#include "math_x.h"
#include "enforce.hpp"
#include "cc_features.h"

/*! GNU Math Extensions.
 * \see /usr/include/x86_64-linux-gnu/bits/mathcalls.h.
 */
namespace std {
#ifdef _GNU_SOURCE
inline spure double exp10(double a) { return exp10(a); }
inline spure float exp10(float a) { return exp10f(a); }
inline spure long double exp10(long double a) { return exp10l(a); }
inline spure double pow10(double a) { return pow10(a); }
inline spure float pow10(float a) { return pow10f(a); }
inline spure long double pow10(long double a) { return pow10l(a); }
inline void sincos(float a, float* sin_, float* cos_) { ::sincosf(a, sin_, cos_); }
inline void sincos(double a, double* sin_, double* cos_) { ::sincos(a, sin_, cos_); }
inline void sincos(long double a, long double* sin_, long double* cos_) { ::sincosl(a, sin_, cos_); }
#else
inline void sincos(float a, float* sin_, float* cos_) { _sin = ::sinf(angle); _cos = ::cosf(angle); }
inline void sincos(double a, double* sin_, double* cos_) { _sin = ::sin(angle); _cos = ::cos(angle); }
inline void sincos(long double a, long double* sin_, long double* cos_) { _sin = ::sinl(angle); _cos = ::cosl(angle); }
#endif
/*! \name \em Sinus and Cosinus of \p angle. */
template<class T> void sincos(T angle, T& _sin, T& _cos) { sincos(angle, &_sin, &_cos); } // delegate to pointer argument version above
}

#if HAVE_CXX11_CONSTEXPR
template<class T> inline spure constexpr T pi_ratio(int x, int y) { return M_PI*x/y; }
template<class T> inline spure constexpr T  e_ratio(int x, int y) { return M_E *x/y; }
#endif

/*! \em Square of \p a. */
namespace std {
template<class T> inline spure T sqr(const T& a) { return a*a; }
template<class T> inline spure T cube(const T& a) { return sqr(a)*a; }
template<class T> inline spure T quart(const T& a) { return sqr(sqr(a)) ; }

template<class T> inline spure T pow1(const T & a) { return a; }
template<class T> inline spure T pow2(const T & a) { return a * a; }
template<class T> inline spure T pow3(const T & a) { return pow2(a) * a; }
template<class T> inline spure T pow4(const T & a) { return pow2(pow2(a)); }
template<class T> inline spure T pow5(const T & a) { return pow2(pow2(a)) * a; }
template<class T> inline spure T pow6(const T & a) { return pow2(pow3(a)); }
}

/*! Iterative Power \p x^ \p a. */
template<class T, class E>
inline spure T iter_pow(T x, E a)
{
    T y = 1;                    // NOTE: for matrix types this must mean eye()
    enforce(a >= 0);            // TODO: support negative exponents
    while (a > 0) {
        if (a % 2 == 0) { x *= x; a /= 2; }
        else            { y *= x; a -= 1; }
    }
    return y;
}

/*! \name Hypotenuse - Euclidean distance.
 * Calculate the Euclidean Distance from origin to vector which coordinates are
 * specified by a, b, ....
 */
/* @{ */
template<class T> inline spure T hypot(const T& a, const T& b, const T& c) { return std::sqrt(a*a + b*b + c*c); }
template<class T> inline spure T hypot(const T& a, const T& b, const T& c, const T& d) { return std::sqrt(a*a + b*b + c*c + d*d); }
template<class T> inline spure T hypot(const T& a, const T& b, const T& c, const T& d, const T& e) { return std::sqrt(a*a + b*b + c*c + d*d + e*e); }
/*! Approximate Euclidean Distance.
 * Approximation is accurate to within +- 8 percent
 */
template<class T> inline spure T hypot3D(T a, T b, T c) {
    a = std::abs(a);
    b = std::abs(b);
    c = std::abs(c);
    ip_sort(a, b, c);           // sort in-place
    /* Alternatives:
       - 13 percent error:  return aa / 4 + bb / 4 + cc;
       - 9 percent error:   return aa / 4 +  5 * bb / 16 + cc; */
    return a / 4 + 11 * b / 32 + c;
}
/* @} */

/*! Normalized \p a. */
template<class T> inline T spure normalize(const T& a) { return a / norm(a); }

/*! Square Distance between \p a and \p b. */
template<template<class> class T, class Tq>
inline Tq spure sqrdist(const T<Tq> & a,
                                         const T<Tq> & b)
{
    return sqrnorm(b - a);
}

/// Return the distance between \p a and \p b.
template<template<class> class T, class Tq>
inline Tq dist(const T<Tq> & a,
               const T<Tq> & b)
{
    return std::sqrt(sqrdist(a, b));
}

/// Return the cosine of the angle between \p a and \p b.
template<template<class> class T, class Tq>
inline Tq angle_cos(const T<Tq> & a,
                    const T<Tq> & b)
{
    return dot(a, b) / (norm(a) * norm(b));
}
