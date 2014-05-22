/*! \file norm.hpp
 * \brief ln-norm calculations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Use C++11 Variadic Templates.
 * \todo Rename to norm_1, norm_2, norm_3, norm_p.
 */

#pragma once

#include <cmath>
#include <algorithm>
#include "algorithm_x.hpp"
#include <type_traits> // <boost/type_traits/common_type.hpp>

namespace pnw
{

/*! \em Approximate Euclidean Distance, within +- 8 percent. */
template<class T>
inline pure T eud3d_approx8(T a, T b, T c)
{
    using std::abs;
    a = abs(a);
    b = abs(b);
    c = abs(c);
    sort3(a, b, c);
    return a/4 + 11*b/32 + c;
}

/*! \em Approximate Euclidean Distance, within +- 9 percent. */
template<class T>
inline pure T eud3d_approx9(T a, T b, T c)
{
    using std::abs;
    a = abs(a);
    b = abs(b);
    c = abs(c);
    sort3(a, b, c);
    return a/4 +  5*b/16 + c;
}

/*! \em Approximate Euclidean Distance, within +- 13 percent. */
template<class T>
inline pure T eud3d_approx13(T a, T b, T c)
{
    using std::abs;
    a = abs(a);
    b = abs(b);
    c = abs(c);
    sort3(a, b, c);
    return a/4 + b/4 + c;
}

#if BOOST_NO_VARIADIC_TEMPLATES
/*! Return Euclidean Distance of vector [ \p a \p b ]. */
template<class T>
inline pure T eud2d(const T & a, const T & b) { return std::sqrt(a*a + b*b); }
/*! Return Euclidean Distance of vector [ \p a \p b \p c ]. */
template<class T>
inline pure T eud3d(const T & a, const T & b, const T & c) { return std::sqrt(a*a + b*b + c*c); }
#else
/*! L1-Norm of \p b. */
template<class T>
inline pure T norm1(const T & a) { return std::abs(a); }
/*! L1-Norm of \p a and \p b. */
template<class T, typename ... R, class C = typename std::common_type<T, R...>::type>
inline pure C norm1(const T & a, const R & ... b) { return std::abs(a) + norm1(b...); }
/*! Square Sum of \p a. */
template<class T>
inline pure T square_sum(const T & a) { return a*a; }
/*! Square Sum of \p a and \p b. */
template<class T, typename ... R, class C = typename std::common_type<T, R...>::type>
inline pure C square_sum(const T & a, const R & ... b) { return a*a + square_sum(b...); }
/*! L2-Norm of \p b. */
template<class ... R, class C = typename std::common_type<R...>::type>
inline pure C norm2(const R & ... b) { return std::sqrt(square_sum(b...)); }
#endif

template<class T>
inline void lnnorm2d(T & a, const T & b, const T & c, uint n)
{
    T nn = static_cast<T>(n);
    a = std::pow(std::pow(b, nn) +
                 std::pow(c, nn),
                 1 / nn);
}
template<class T> inline pure T lnnorm2d(const T & b, const T & c, uint n) { T a; lnnorm2d(a, b, c, n); return a; }

template<class T>
inline void lnnorm3d(T & a,
                     const T & b,
                     const T & c,
                     const T & d,
                     uint n)
{
    T nn = static_cast<T>(n);
    a = std::pow(std::pow(b, nn) +
                 std::pow(c, nn) +
                 std::pow(d, nn),
                 1 / nn);
}
template<class T> inline pure T lnnorm3d(const T & b, const T & c, const T & d, uint n) { T a; lnnorm3d(a, b, c, d, n); return a;}

}
