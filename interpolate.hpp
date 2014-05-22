/*! \file interpolate.hpp
 * \brief Interpolate.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/
 * \todo Add functions that extract polynomials from cubic interpolation.
 */

#pragma once
#include <cmath>
#include <tuple>
#include <array>
#include <algorithm>

/*! Interpolation Method.
 * \see http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/
 */
typedef enum
{
    INTERPOL_NEAREST,           ///< \em Nearest-Neighbour.
    INTERPOL_LINEAR,            ///< \em Linear. Takes 25 percent longer than \c INTERPOL_NEAREST.
    INTERPOL_CUBIC,             ///< \em Cubic Polynomial (Spline). \see http://en.wikipedia.org/wiki/Cubic_spline
    INTERPOL_CUBIC_CATMULL_ROM, ///< \em Catmull-Rom Spline.
    INTERPOL_CUBIC_HERMITE,     ///< \em Hermite Polynomial. \see http://en.wikipedia.org/wiki/Cubic_Hermite_spline
    INTERPOL_DEFAULT = INTERPOL_CUBIC_CATMULL_ROM, // Default Type. Good compromise between precision and performance.
} INTERPOL_t;

/*! @name Linear Interpolate at Normalized Offset \p mu \in [0,1] between samples \p y1 and \p y2. */
/*! \{ */
template<class T> inline spure
T linear_interpolate(T y1,
                     T y2,
                     T mu) {
    return y1*(static_cast<T>(1) - mu) + y2*mu;
}
template<class T, class U> inline pure
std::pair<T,U> linear_interpolate(const std::pair<T,U>& y1,
                                  const std::pair<T,U>& y2,
                                  T mu) {
    return std::make_pair<T,U>(linear_interpolate(std::get<0>(y1), std::get<0>(y2), mu),
                               linear_interpolate(std::get<1>(y1), std::get<1>(y2), mu));
}
template<class T, class U, class V> inline pure
std::tuple<T,U,V> linear_interpolate(const std::tuple<T,U,V>& y1,
                                     const std::tuple<T,U,V>& y2,
                                     T mu) {
    return std::make_tuple<T,U>(linear_interpolate(std::get<0>(y1), std::get<0>(y2), mu),
                                linear_interpolate(std::get<1>(y1), std::get<1>(y2), mu),
                                linear_interpolate(std::get<2>(y1), std::get<2>(y2), mu));
}
template<class T, std::size_t N> inline pure
std::array<T,N> linear_interpolate(const std::array<T,N>& y1,
                                   const std::array<T,N>& y2,
                                   T mu) {
    std::array<T,N> a;
    std::transform(begin(y1),end(y1), begin(y2), begin(a),
                   [mu](T x, T y) { return linear_interpolate(x, y, mu); });
    return a;
}

/*! \} */
/*! \em Linearly \em Interpolate \p n Number of Elements from \p src to \p dst at progress \p prg. */
template<class T> inline void linear_interpolate(const T * src, const T * prg, const T * dst, T * curr, size_t n) {
    for (size_t i = 0; i != n; i++) {
        curr[i] = linear_interpolate(src[i], prg[i], dst[i]);
    }
}

/*! Cubic Interpolate at offset \p mu between samples \p y1 and \p y2. */
template<class T> inline spure T cubic_interpolate(T y0, T y1, T y2, T y3, T mu)
{
    const T mu2 = mu*mu;
    const T a0 = y3 - y2 - y0 + y1;
    const T a1 = y0 - y1 - a0;
    const T a2 = y2 - y0;
    const T a3 = y1;
    return (a0*mu*mu2 + a1*mu2 + a2*mu + a3);
}

/*! Cubic Catmull–Rom Spline Interpolate at offset \p mu between samples \p y1 and \p y2.
 * C1 continuous.
 * \see http://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull.E2.80.93Rom_spline
 */
template<class T>
inline spure
T cubic_catmull_rom_interpolate(T y0, T y1, T y2, T y3, T mu)
{
    const T mu2 = mu*mu;
    const T a0 = -0.5*y0 + 1.5*y1 - 1.5*y2 + 0.5*y3;
    const T a1 = y0 - 2.5*y1 + 2*y2 - 0.5*y3;
    const T a2 = -0.5*y0 + 0.5*y2;
    const T a3 = y1;
    // const T a0 = -y0/2 + 3*y1/2 - 3*y2/2 + y3/2;
    // const T a1 = y0 - 5*y1/2 + 2*y2 - y3/2;
    // const T a2 = -y0/2 + y2/2;
    // const T a3 = y1;
    return (a0*mu*mu2 + a1*mu2 + a2*mu + a3);
}

/*! Cubic Hermite Polynomial Interpolate at offset \p mu between samples \p y1 and \p y2.
 * \param tension 1 is high, 0 normal, -1 is low.
 * \param bias 0 is even, positive is towards first segment, negative towards the other.
 */
template<class T>
inline spure
T cubic_hermite_interpolate(T y0, T y1, T y2, T y3, T mu,
                            T tension = 0,
                            T bias = 0)
{
    const T mu2 = mu * mu;
    const T mu3 = mu2 * mu;
    const T m0 = ((y1-y0)*(1+bias)*(1-tension)/2 +
                  (y2-y1)*(1-bias)*(1-tension)/2);
    const T m1  = ((y2-y1)*(1+bias)*(1-tension)/2 +
                   (y3-y2)*(1-bias)*(1-tension)/2);
    const T a0 =  2*mu3 - 3*mu2 + 1;
    const T a1 =    mu3 - 2*mu2 + mu;
    const T a2 =    mu3 -   mu2;
    const T a3 = -2*mu3 + 3*mu2;
    return (a0*y1 + a1*m0 + a2*m1 + a3*y2);
}

/*! Cosine-Smoothed Interpolate at offset \p mu between samples \p y1 and \p y2. */
template<class T> inline spure T cosine_interpolate(T y1, T y2, T mu)
{
    const T muS = (1 - cos(mu * M_PI)) / 2; // WARNING: Costly Dumb-Smooth transition! Only of \em academic use.
    return y1*(static_cast<T>(1) - muS) + y2*muS;
}

// ============================================================================

/*! One-Dimensional Linear Interpolation from \p a to \p b at \p i (from 0 to m). */
template<class T>
inline pure T linear_interpolate_1d_same(const T& a, const T& b,
                                                          const T& i,
                                                          const T& m)
{
    return static_cast<T>((a * (m-i) + b * i) / m);
}

/*! One-Dimensional Linear Interpolation from \p a to \p b at \p i (from 0 to m). */
template<class Ta, class Tb>
inline pure Ta linear_interpolate_1d(const Ta& a, const Ta& b,
                                                      const Tb& i,
                                                      const Tb& m)
{
    return static_cast<Ta>((a * (m-i) + b * i) / m);
}

/*! One-Dimensional Linear Interpolation from \p a to \p b at \p i (from 0 to m), where
 * m is an even power of two (2^p). Therefore the algorithm becomes optimized
 * for uinteger arithmetic.
 *
 * The argument index i is cycled to the range [0 ... m-1].
 */
template<class Ta>
inline pure Ta linear_interpolate_1d_binary_power_norm(const Ta& a, const Ta& b,
                                                                        uint i,
                                                                        uint p)
{
    const uint m = 1 << p;
    i = i bitand (m-1);
    return (a * (m-i) + b * i) >> p;
}

/*! Two-dimensional Linear Interpolation from \p a to \p b and c to d
 * at \p i (from 0 to m) and j (from 0 to n).
 */
template<class Ta>
inline pure Ta linear_interpolate2d_same(const Ta& a, const Ta& b,
                                                          const Ta& c, const Ta& d,
                                                          const Ta& i, const Ta& j,
                                                          const Ta& m, const Ta& n)
{
    Ta ab = linear_interpolate_1d_same(a, b, i, m);
    Ta cd = linear_interpolate_1d_same(c, d, i, m);
    return linear_interpolate_1d_same(ab, cd, j, n);
}
