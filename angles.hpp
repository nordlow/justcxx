/*! \file angles.hpp
 * \brief Angles Template.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-06-23 01:03
 */

#pragma once
#include <ostream>
#include <cmath>
#include <utility>
#include "cc_features.h"
#include "enforce.hpp"

namespace angles {

static constexpr long double PI = 3.1415926535897932; ///< Pi
static constexpr long double RAD2DEG = 180.0f / PI; ///< Radians to Degrees Conversion Factor
static constexpr long double DEG2RAD = PI / 180.0f; ///< Degrees to Radians Conversion Factor

template<class T>
class degrees;

/*! Radian Angle.
  \see https://en.wikipedia.org/wiki/Radian
 */
template<class T>
class radians {
public:
    explicit constexpr radians(T radians_) :m_val(radians_) {}
    radians(degrees<T> degrees_); // defined below
    radians(const radians& radians_) = default;
    radians(radians&& radians_) = default;

    constexpr T get() const { return m_val; }

    radians& operator=(const radians& rhs) & = default;
    radians& operator=(radians&& rhs) & noexcept = default;

    constexpr radians operator -() const            { return radians(-m_val); }
    constexpr radians operator +(radians rhs) const { return radians(m_val + rhs.m_val); }
    constexpr radians operator -(radians rhs) const { return radians(m_val - rhs.m_val); }
    constexpr radians operator *(T rhs) const   { return radians(m_val * rhs); }
    constexpr radians operator /(T rhs) const   { return radians(m_val / rhs); }
    constexpr T   operator /(radians rhs) const { return (m_val / rhs.m_val); }
    constexpr friend radians operator*(T f, radians d) { return radians(d.m_val * f); }

    constexpr bool operator ==(radians rhs) const { return m_val == rhs.m_val; }
    constexpr bool operator !=(radians rhs) const { return m_val != rhs.m_val; }
    constexpr bool operator > (radians rhs) const { return m_val >  rhs.m_val; }
    constexpr bool operator < (radians rhs) const { return m_val <  rhs.m_val; }
    constexpr bool operator >=(radians rhs) const { return m_val >= rhs.m_val; }
    constexpr bool operator <=(radians rhs) const { return m_val <= rhs.m_val; }

    radians& operator +=(radians rhs) { m_val += rhs.m_val; return *this; }
    radians& operator -=(radians rhs) { m_val -= rhs.m_val; return *this; }

    /*! Scaling with others units is allowed. */
    radians& operator *=(T rhs)   { m_val *= rhs; return *this; }
    radians& operator /=(T rhs)   { m_val /= rhs; return *this; }

    friend std::ostream& show(std::ostream& os, const radians& a) { os << a.get() << "rad"; return os; }
    friend std::ostream& operator << (std::ostream& os, radians a) { return show(os, a); }
private:
    T m_val;
};

/*! Degree Angle.
  \see https://en.wikipedia.org/wiki/Degree_(angle)
*/
template<class T>
class degrees {
public:
    explicit constexpr degrees(T degrees_) :m_val(degrees_) {}
    degrees(radians<T> radians_) :m_val(radians_.get() * RAD2DEG) {}
    degrees(const degrees& degrees_) = default;
    degrees(degrees&& degrees_) = default;

    constexpr T get() const { return m_val; }

    degrees& operator=(const degrees& rhs) & = default;
    degrees& operator=(degrees&& rhs) & noexcept = default;

    constexpr degrees operator -() const            { return degrees(-m_val); }
    constexpr degrees operator +(degrees rhs) const { return degrees(m_val + rhs.m_val); }
    constexpr degrees operator -(degrees rhs) const { return degrees(m_val - rhs.m_val); }
    constexpr degrees operator *(T rhs) const   { return degrees(m_val * rhs); }
    constexpr degrees operator /(T rhs) const   { return degrees(m_val / rhs); }
    constexpr T   operator /(degrees rhs) const { return (m_val / rhs.m_val); }
    constexpr friend degrees operator*(T f, degrees d) { return degrees(d.m_val * f); }

    constexpr bool operator ==(degrees rhs) const { return m_val == rhs.m_val; }
    constexpr bool operator !=(degrees rhs) const { return m_val != rhs.m_val; }
    constexpr bool operator > (degrees rhs) const { return m_val >  rhs.m_val; }
    constexpr bool operator < (degrees rhs) const { return m_val <  rhs.m_val; }
    constexpr bool operator >=(degrees rhs) const { return m_val >= rhs.m_val; }
    constexpr bool operator <=(degrees rhs) const { return m_val <= rhs.m_val; }

    degrees& operator +=(degrees rhs) { m_val += rhs.m_val; return *this; }
    degrees& operator -=(degrees rhs) { m_val -= rhs.m_val; return *this; }

    /*! Scaling with others units is allowed. */
    degrees& operator *=(T rhs)   { m_val *= rhs; return *this; }
    degrees& operator /=(T rhs)   { m_val /= rhs; return *this; }

    friend std::ostream& show(std::ostream& os, const degrees& a) { os << a.get() << "deg"; return os; }
    friend std::ostream& operator << (std::ostream& os, degrees a) { return show(os, a); }
private:
    T m_val;
};

/*! @name Trigonometry. */
/// @{
template<class T> spure inline T sin(radians<T> x) { return std::sin(x.get()); }
template<class T> spure inline T cos(radians<T> x) { return std::cos(x.get()); }
template<class T> spure inline T tan(radians<T> x) { return std::tan(x.get()); }
template<class T> spure inline std::pair<T, T> sincos(radians<T> x) {
#ifdef _GNU_SOURCE
    T sin_, cos_;
    ::sincos(x.get(), &sin_, &cos_);
    return std::make_pair(sin_, cos_);
#else
    return std::make_pair(::sin(x.get()),
                          ::cos(x.get()));
#endif
}
template<class T> spure inline T sin(degrees<T> x) { return sin(radians<T>(x)); }
template<class T> spure inline T cos(degrees<T> x) { return cos(radians<T>(x)); }
template<class T> spure inline T tan(degrees<T> x) { return tan(radians<T>(x)); }
template<class T> spure inline std::pair<T, T> sincos(degrees<T> x) { return sincos(radians<T>(x)); }
/// @}

/*! @name Inverse Trigonometry. */
/// @{
template<class T> spure inline radians<T> asin(T x) { return radians<T>(asin(x)); }
template<class T> spure inline radians<T> acos(T x) { return radians<T>(acos(x)); }
template<class T> spure inline radians<T> atan(T x) { return radians<T>(atan(x)); }
template<class T> spure inline radians<T> atan2(T x, T y) { return radians<T>(atan2(x, y)); }
#ifndef _COMPLEX_H
template<class T> spure inline radians<T> carg(T x) { return radians<T>(carg(x)); }
#endif
/// @}

template<class T> inline radians<T>::radians(degrees<T> degrees_) : m_val(degrees_.get() * DEG2RAD) {}

#ifdef HAVE_CXX11_USER_DEFINED_LITERALS
inline radians<long double> operator"" _rad(long double r)        { return radians<decltype(r)> (r); }
inline radians<unsigned long long> operator"" _rad(unsigned long long r) { return radians<decltype(r)>(r); }
inline degrees<long double> operator"" _deg(long double d)        { return degrees<decltype(d)>(d); }
inline degrees<unsigned long long> operator"" _deg(unsigned long long d) { return degrees<decltype(d)>(d); }
#endif

/*! Test Arithmetic
 */
template<class T>
inline void testAll()
{
    const auto x = 180.0;

    degrees<double> d(x);       // store as degrees
    enforce_eq(d.get(), x);

#ifdef HAVE_CXX11_USER_DEFINED_LITERALS
    const auto dl = 180.0_deg;
    enforce_eq(d.get(), dl.get());
#endif

    radians<double> r(d);       // convert it to radians represention

    enforce_eq((d + r).get(), 360);
    enforce_eq((d - r).get(), 0);
    enforce_eq((2 * d).get(), 360);

    enforce_lt((cos(d) - -1), 1.0e-16);
    enforce_lt((sin(d) - 0), 1.5e-16);

    const auto pd = sincos(d);
    const auto pp = std::make_pair(sin(d), cos(d));

    enforce_eq(pd.first,
               pp.first);
    enforce_eq(pd.second,
               pp.second);

    enforce_lt((tan(degrees<T>(45.0)) - 1), 1.0e-16);
}

}
