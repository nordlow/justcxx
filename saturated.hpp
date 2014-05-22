/*! \file saturated.hpp
 * \brief Saturated Arithmetic Wrapper Class Template.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-04-28 18:49
 *
 * \see ranged.hpp
 *
 * \see https://en.wikipedia.org/wiki/Saturation_arithmetic
 * \see http://forum.dlang.org/thread/yncxfhikgmcbgdfyixvl@forum.dlang.org
 * \see http://www.artima.com/cppsource/safebool.html
 * \todo Support mixed-type arithmetic using type propagation using std::common_type<T,U>::type or GCC's C++14 return type deduction.
 */

#pragma once
#include <algorithm>
#include "cc_features.h"
#include "saturate.hpp"
#include <boost/lexical_cast.hpp>

/*! Saturated Arithmetic Type Wrapper.
 */
template<class T>
class saturated {
public:
    saturated() : m_(0) {}

    /*! Disable Implicit Conversions from wrapped type. */
#ifdef HAVE_CXX11_EXPLICIT
    explicit saturated(T a) : m_(a) {}
    explicit operator bool() const { return m_; } ///< \see http://stackoverflow.com/questions/6242768/is-the-safe-bool-idiom-obsolete-in-c11
#endif

    /*! Arithmetic on \c saturated. */
    /// @{
    friend saturated operator + (const saturated& a, const saturated& b) { return saturated(sadd(a.m_, b.m_)); } // std::plus
    friend saturated operator - (const saturated& a, const saturated& b) { return saturated(ssub(a.m_, b.m_)); } // std::minus
    friend saturated operator * (const saturated& a, const saturated& b) { return saturated(smul(a.m_, b.m_)); } // std::times
    friend saturated operator / (const saturated& a, const saturated& b) { return saturated(a.m_ / b.m_); } // std::div
    friend saturated operator % (const saturated& a, const saturated& b) { return saturated(a.m_ % b.m_); } // std::mod
    /// @}
    /*! \@name Arithmetic with other types. TODO: Is this what we want? */
    /// @{
    template<class U> friend saturated operator + (const saturated& a, const U& b) { return saturated(sadd(a.m_, b)); }
    template<class U> friend saturated operator + (const U& a, const saturated& b) { return saturated(sadd(a, b.m_)); }
    template<class U> friend saturated operator - (const saturated& a, const U& b) { return saturated(ssub(a.m_, b)); }
    template<class U> friend saturated operator - (const U& a, const saturated& b) { return saturated(ssub(a, b.m_)); }
    template<class U> friend saturated operator * (const saturated& a, const U& b) { return saturated(smul(a.m_, b)); }
    template<class U> friend saturated operator * (const U& a, const saturated& b) { return saturated(smul(a, b.m_)); }
    template<class U> friend saturated operator / (const saturated& a, const U& b) { return saturated(a.m_ / b); }
    template<class U> friend saturated operator / (const U& a, const saturated& b) { return saturated(a / b.m_); }
    template<class U> friend saturated operator % (const saturated& a, const U& b) { return saturated(a.m_ % b); }
    template<class U> friend saturated operator % (const U& a, const saturated& b) { return saturated(a % b.m_); }
    /// @}

    //! Comparison.
    /// @{
    friend bool operator == (const saturated& a, const saturated& b) { return a.m_ == b.m_; }
    friend bool operator != (const saturated& a, const saturated& b) { return a.m_ != b.m_; }
    friend bool operator < (const saturated& a, const saturated& b) { return a.m_ < b.m_; }
    friend bool operator > (const saturated& a, const saturated& b) { return a.m_ > b.m_; }
    friend bool operator <= (const saturated& a, const saturated& b) { return a.m_ <= b.m_; }
    friend bool operator >= (const saturated& a, const saturated& b) { return a.m_ >= b.m_; }
    template<class U> friend bool operator == (const saturated& a, const U& b) { return a.m_ == b; }
    template<class U> friend bool operator != (const saturated& a, const U& b) { return a.m_ != b; }
    template<class U> friend bool operator <  (const saturated& a, const U& b) { return a.m_ < b; }
    template<class U> friend bool operator >  (const saturated& a, const U& b) { return a.m_ > b; }
    template<class U> friend bool operator <= (const saturated& a, const U& b) { return a.m_ <= b; }
    template<class U> friend bool operator >= (const saturated& a, const U& b) { return a.m_ >= b; }
    template<class U> friend bool operator == (const U& a, const saturated& b) { return a == b.m_; }
    template<class U> friend bool operator != (const U& a, const saturated& b) { return a != b.m_; }
    template<class U> friend bool operator <  (const U& a, const saturated& b) { return a < b.m_; }
    template<class U> friend bool operator >  (const U& a, const saturated& b) { return a > b.m_; }
    template<class U> friend bool operator <= (const U& a, const saturated& b) { return a <= b.m_; }
    template<class U> friend bool operator >= (const U& a, const saturated& b) { return a >= b.m_; }
    /// @}

    //! Minimum and Maximum.
    /// @{
    friend saturated min(const saturated& a, const saturated& b) { return std::min(a.m_, b.m_); }
    friend saturated max(const saturated& a, const saturated& b) { return std::max(a.m_, b.m_); }
    /*! min, max with other types. */
    template<class U> friend saturated min(const saturated& a, const U& b) { return std::min(a.m_, b); }
    template<class U> friend saturated min(const U& a, const saturated& b) { return std::min(a, b.m_); }
    template<class U> friend saturated max(const saturated& a, const U& b) { return std::max(a.m_, b); }
    template<class U> friend saturated max(const U& a, const saturated& b) { return std::max(a, b.m_); }
    /// @}

    /*! @name Explicit Arithmetic. */
    template<class U> saturated& mul(const U& a) { m_ *= a; return *this; }
    template<class U> saturated& div(const U& a) { m_ /= a; return *this; }
    friend spure saturated inc(const saturated& a) { return saturated(sinc(a.m_)); }
    friend spure saturated dec(const saturated& a) { return saturated(sdec(a.m_)); }
    saturated& inc() { *this = inc(*this); return *this; }
    saturated& ded() { *this = dec(*this); return *this; }

    saturated& operator += (const saturated& a) { *this = *this + a; return *this; }
    saturated& operator -= (const saturated& a) { *this = *this - a; return *this; }
    saturated& operator *= (const saturated& a) { *this = *this * a; return *this; }
    saturated& operator /= (const saturated& a) { *this = *this / a; return *this; }
    template<class U> saturated& operator += (const U& a) { return *this += saturated(a); }
    template<class U> saturated& operator -= (const U& a) { return *this -= saturated(a); }
    template<class U> saturated& operator *= (const U& a) { return *this *= saturated(a); }
    template<class U> saturated& operator /= (const U& a) { return *this /= saturated(a); }

    /*! Prefix Increment */
    saturated& operator ++ () { saturated(sinc(get())); return *this; }
    /*! Prefix Decrement */
    saturated& operator -- () { saturated(sdec(get())); return *this; }
    /*! Suffix Increment. */
    saturated operator ++ (int) { auto prev = *this; ++*this; return prev; } ///< \note: Reuses prefix!
    /*! Suffx Decrement. */
    saturated operator -- (int) { auto prev = *this; --*this; return prev; } ///< \note: Reuses prefix!

    /*! Set wrapped value. */
    saturated& set(T m) const { m_ = m; return *this; }
    /*! Get wrapped value. */
    T get() const { return m_; }

    static constexpr saturated zero() { return saturated(0); } ///< \todo: Disable for specific classes of \c T.
    static constexpr saturated one() { return saturated(1); } ///< \todo: Disable for specific classes of \c T.

    static constexpr saturated min() { return saturated(std::numeric_limits<T>::min()); }
    static constexpr saturated max() { return saturated(std::numeric_limits<T>::max()); }

    static constexpr saturated empty() { return saturated(std::numeric_limits<T>::min()); }
    static constexpr saturated inf()   { return saturated(std::numeric_limits<T>::max()); }

    friend std::ostream& show(std::ostream& os, const saturated& a) {
        if (a.get() == saturated::max().get()) {
            os << "high";
        } else {
            os << a.get();
        }
        os << unitof<T>();
        return os;
    }
    friend std::ostream& operator << (std::ostream& os, const saturated& a) { return show(os, a); }

    /*! Iterative Power \p x^ \p a. */
    friend spure saturated iter_pow(saturated x, const saturated& a) { return iter_pow<saturated>(x, a); }
private:
    T m_;               ///< Bit-Index.
};

typedef saturated<char> sat_size_t;

typedef saturated<char> schar;
typedef saturated<unsigned char> suchar;

typedef saturated<short> sshort;
typedef saturated<unsigned short> sushort;

typedef saturated<long> slong;
typedef saturated<unsigned long> sulong;

typedef saturated<long long> slonglong;
typedef saturated<unsigned long long> sulonglong;

typedef saturated<uint8_t> suint8_t;
typedef saturated<uint16_t> suint16_t;
typedef saturated<uint32_t> suint32_t;
typedef saturated<uint64_t> suint64_t;
