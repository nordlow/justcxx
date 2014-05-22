/*! \file maybe.hpp
 * \brief Wrapper for Values with (Un)Maybe Boolean State.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-11-03 09:04
 * \see boost::optional, std::optional, relangs-optional-type
 *
 * void set_snan( double &d )
 * {
 *   long long *bits = (long long *)&d;
 *   *bits = 0x7ff0000080000001LL;
 * }
 */

#pragma once
#include "cc_features.h"
#include <cmath>
#include <ostream>
#if HAVE_BOOST_SERIALIZATION_ACCESS_HPP
#  include <boost/serialization/access.hpp>
#endif

/*! Optional Value of Type \c T.
 * \tparam X is true when exceptions should be thrown unless \c NDEBUG
 *         is set. If false saturation will occur.
 * \alias initialized, optional
 * \see boost::optional, std::optional, relangs-optional-type
 */
template<class T, bool X=false> class maybe {
public:
    /*! Default Construct to undefined. */
#ifndef NDEBUG
    maybe() : m_defined(false) { } ///< Skip initialization of wrapped value \c m_ in non-debug (release) builds.
#else
    maybe() : m_(T()), m_defined(false) { } ///< Always initialize wrapped value \c m_ in debug builds.
#endif

    static constexpr spure maybe nothing() { return maybe(false); }

    /*! Copy-Construct defined from value \p t. */
    maybe(const T& t) : m_(t),
#ifndef NDEBUG
                        m_defined(true)
#endif
    { }

    /*! Move-Construct defined from value \p t. */
    maybe(const T&& t) : m_(std::move(t)),
#ifndef NDEBUG
                        m_defined(true)
#endif
    { }

    /*! Assignment-Copy-Construct defined from value \p t. */
    T& operator = (const T& t) { m_ = t; m_defined = true; return m_; }
    /*! Assignment-Move-Construct defined from value \p t. */
    T& operator = (T&& t) { m_ = std::move(t); m_defined = true; return m_; }

#ifdef HAVE_CXX11_EXPLICIT
    explicit operator bool() const { return get(); } ///< \see http://stackoverflow.com/questions/6242768/is-the-safe-bool-idiom-obsolete-in-c11
#endif

    spure bool defined() const {
#ifndef NDEBUG
        return m_defined;
#else
        return true;
#endif
    }

    /*! Make undefined. */
    maybe& undefine() {
#ifndef NDEBUG
        m_defined = false;
#endif
        return *this;
    }

    /*! Check if defined. */
    bool check() const {
#ifndef NDEBUG
        if (not defined()) {
            if (X) {
                throw std::exception();
            } else {
                // \todo We are more interested in printing the caller of get(). Could we somehow print that instead?
                // Make __FILE__ and __LINE__ accessibly using macros.
                std::cerr << std::endl << __FILE__ << ":" << __LINE__ << ": warning:" << "Value is undefined" << std::endl;
            }
            return false;
        }
#endif
        return true;
    }

    /*! Implicit Conversion to Wrapped Value. */
    // operator T() const { return get(); }

    const T& get() const { check(); return m_; }
    T& get() { check(); return m_; }

    /*! Arithmetic. */
    friend spure maybe operator + (maybe a, maybe b) { return a.check() && b.check() ? a.m_ + b.m_ : nothing(); }
    friend spure maybe operator - (maybe a, maybe b) { return a.check() && b.check() ? a.m_ - b.m_ : nothing(); }
    friend spure maybe operator * (maybe a, maybe b) { return a.check() && b.check() ? a.m_ * b.m_ : nothing(); }
    friend spure maybe operator / (maybe a, maybe b) { return a.check() && b.check() ? a.m_ / b.m_ : nothing(); }
    friend spure maybe operator % (maybe a, maybe b) { return a.check() && b.check() ? a.m_ % b.m_ : nothing(); }

    /*! Prefix Increment */
    maybe& operator ++ () { if (check()) { m_++; } return *this; }
    /*! Prefix Decrement */
    maybe& operator -- () { if (check()) { m_--; } return *this; }
    /*! Suffix Increment. */
    maybe operator ++ (int) { auto prev = *this; if (check()) { ++m_; } return prev; }
    /*! Suffx Decrement. */
    maybe operator -- (int) { auto prev = *this; if (check()) { --m_; } return prev; }

    /*! Comparison. */
    friend spure maybe<bool> operator == (maybe a, maybe b) { return a.check() && b.check() ? a.m_ == b.m_ : maybe<bool>(); }
    friend spure maybe<bool> operator != (maybe a, maybe b) { return a.check() && b.check() ? a.m_ != b.m_ : maybe<bool>(); }
    friend spure maybe<bool> operator < (maybe a, maybe b) { return a.check() && b.check() ? a.m_ < b.m_ : maybe<bool>(); }
    friend spure maybe<bool> operator > (maybe a, maybe b) { return a.check() && b.check() ? a.m_ > b.m_ : maybe<bool>(); }
    friend spure maybe<bool> operator <= (maybe a, maybe b) { return a.check() && b.check() ? a.m_ <= b.m_ : maybe<bool>(); }
    friend spure maybe<bool> operator >= (maybe a, maybe b) { return a.check() && b.check() ? a.m_ >= b.m_ : maybe<bool>(); }

    /*! Min/Max */
    friend spure maybe min(maybe a, maybe b) { return a.check() && b.check() ? maybe(std::min(a.m_, b.m_)) : nothing(); }
    friend spure maybe max(maybe a, maybe b) { return a.check() && b.check() ? maybe(std::max(a.m_, b.m_)) : nothing(); }

    /*! https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B#Compound_assignment_operators */
    template<class U> maybe& operator += (maybe<U> a) { *this = *this + a; return *this; }
    template<class U> maybe& operator -= (maybe<U> a) { *this = *this - a; return *this; }
    template<class U> maybe& operator *= (maybe<U> a) { *this = *this * a; return *this; }
    template<class U> maybe& operator /= (maybe<U> a) { *this = *this / a; return *this; }
    template<class U> maybe& operator %= (maybe<U> a) { *this = *this % a; return *this; }
    template<class U> maybe& operator &= (maybe<U> a) { *this = *this & a; return *this; }
    template<class U> maybe& operator |= (maybe<U> a) { *this = *this | a; return *this; }
    template<class U> maybe& operator ^= (maybe<U> a) { *this = *this ^ a; return *this; }
    template<class U> maybe& operator <<= (maybe<U> a) { *this = *this << a; return *this; }
    template<class U> maybe& operator >>= (maybe<U> a) { *this = *this >> a; return *this; }

    friend std::ostream& show(std::ostream& os, const maybe& a, bool include_type = false) {
        if (a.defined()) {
            os << a.get();
        } else {
            os << "undefined";
        }
        return os;
    }
    friend std::ostream& operator << (std::ostream& os, maybe a) { return show(os, a); }

#if HAVE_BOOST_SERIALIZATION_ACCESS_HPP
    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) {
        // TODO: We need to encode variant header using version.
#ifndef NDEBUG
        ar & m_ & m_defined;
#else
        ar & m_;
#endif
    }
#endif
private:
    T m_;                              ///< The value.
#ifndef NDEBUG
    bool m_defined = false;             ///< True if \c m_ has a defined value.
#endif
};

/*! Builders. */
template<class T, bool X=false> inline spure maybe<T,X> make_maybe(const T& a) { return maybe<T,X>(a); }
template<class T, bool X=false> inline spure maybe<T,X> make_maybe(T&& a) { return maybe<T,X>(std::move(a)); }

/*! Arithmetic. */
template<class T, class U, bool X=false> inline spure auto operator + (maybe<T,X> a, U b) -> maybe<decltype(a.get() + b),X> { return a.check() ? a.get() + b : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto operator - (maybe<T,X> a, U b) -> maybe<decltype(a.get() - b),X> { return a.check() ? a.get() - b : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto operator * (maybe<T,X> a, U b) -> maybe<decltype(a.get() * b),X> { return a.check() ? a.get() * b : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto operator / (maybe<T,X> a, U b) -> maybe<decltype(a.get() / b),X> { return a.check() ? a.get() / b : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto operator % (maybe<T,X> a, U b) -> maybe<decltype(a.get() % b),X> { return a.check() ? a.get() % b : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto operator + (U a, maybe<T,X> b) -> maybe<decltype(a + b.get()),X> { return b.check() ? a + b.get() : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto operator - (U a, maybe<T,X> b) -> maybe<decltype(a - b.get()),X> { return b.check() ? a - b.get() : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto operator * (U a, maybe<T,X> b) -> maybe<decltype(a * b.get()),X> { return b.check() ? a * b.get() : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto operator / (U a, maybe<T,X> b) -> maybe<decltype(a / b.get()),X> { return b.check() ? a / b.get() : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto operator % (U a, maybe<T,X> b) -> maybe<decltype(a % b.get()),X> { return b.check() ? a % b.get() : maybe<T,X>::nothing(); }

/*! Comparison. */
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator == (maybe<T,X> a, U b) { return a.check() ? a.get() == b : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator != (maybe<T,X> a, U b) { return a.check() ? a.get() != b : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator <  (maybe<T,X> a, U b) { return a.check() ? a.get() < b : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator >  (maybe<T,X> a, U b) { return a.check() ? a.get() > b : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator <= (maybe<T,X> a, U b) { return a.check() ? a.get() <= b : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator >= (maybe<T,X> a, U b) { return a.check() ? a.get() >= b : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator == (U a, maybe<T,X> b) { return b.check() ? a == b.get() : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator != (U a, maybe<T,X> b) { return b.check() ? a != b.get() : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator <  (U a, maybe<T,X> b) { return b.check() ? a < b.get() : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator >  (U a, maybe<T,X> b) { return b.check() ? a > b.get() : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator <= (U a, maybe<T,X> b) { return b.check() ? a <= b.get() : maybe<bool,X>::nothing(); }
template<class T, class U, bool X=false> inline spure maybe<bool,X> operator >= (U a, maybe<T,X> b) { return b.check() ? a >= b.get() : maybe<bool,X>::nothing(); }

/*! Min/Max */
template<class T, class U, bool X=false> inline spure auto min(maybe<T,X> a, U b) -> maybe<decltype(std::min(a.get(), b)),X> { return a.check() ? make_maybe(std::min(a.get(), b)) : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto max(maybe<T,X> a, U b) -> maybe<decltype(std::max(a.get(), b)),X> { return a.check() ? make_maybe(std::max(a.get(), b)) : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto min(U a, maybe<T,X> b) -> maybe<decltype(std::min(a, b.get())),X> { return b.check() ? make_maybe(std::min(a, b.get())) : maybe<T,X>::nothing(); }
template<class T, class U, bool X=false> inline spure auto max(U a, maybe<T,X> b) -> maybe<decltype(std::max(a, b.get())),X> { return b.check() ? make_maybe(std::max(a, b.get())) : maybe<T,X>::nothing(); }

template<class T, class S = std::string> inline spure const S typenameof(const maybe<T>& a) { return S("maybe<") + typenameof<T>() + ">"; }

// #include <limits>
namespace std {
template<class T> class numeric_limits;
template<class T> class numeric_limits<maybe<T> > {
public:
    static constexpr maybe<T> min() { return maybe<T>(std::numeric_limits<T>::min()); }
    static constexpr maybe<T> max() { return maybe<T>(std::numeric_limits<T>::max()); }
};
}

#if 0
/*! Specialize for floating points using NaN (Not a Number).
 * \todo Handle either std::numeric_limits::has_signaling_NaN or std::numeric_limits::has_quiet_NaN, */
#include <limits>
#include <type_traits>
template<class T>
class maybe<typename std::enable_if<std::numeric_limits::has_quiet_NaN, T>::type > {
public:
    nothing() : m_(nanf()) { }
    bool defined() const { return !std::isnan(m_); }
    operator T&() {
        if (not defined()) { throw std::exception(); }
        return m_;
    }
private:
    T m_;                              ///< The value.
};

template<bool X> class maybe<float, X> {
public:
    nothing() : m_(nanf("NAN(string)")) { }
    bool defined() const { return !std::isnan(m_); }
    float& get() {
        if (X) { if (not defined()) { throw std::exception(); } }
        return m_;
    }
private:
    float m_;                              ///< The value.
};

template<bool X> class maybe<double, X> {
public:
    nothing() : m_(nan("NAN(string)")) { }
    bool defined() const { return !std::isnan(m_); }
    double& get() {
        if (X) { if (not defined()) { throw std::exception(); } }
        return m_;
    }
private:
    double m_;                              ///< The value.
};

template<bool X> class maybe<long double, X> {
public:
    nothing() : m_(nanl("NAN(string)")) { }
    bool defined() const { return !std::isnan(m_); }
    long double& get() {
        if (X) { if (not defined()) { throw std::exception(); } }
        return m_;
    }
private:
    long double m_;                              ///< The value.
};
#endif
