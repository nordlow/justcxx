/*! \file ranged.hpp
 * \brief Ranged Arithmetic Wrapper Class Template.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-04-28 18:49
 *
 * \see saturated.hpp
 * \see http://www.artima.com/cppsource/safebool.html
 * \todo Use std::overflow_error("") in arithmetic. Needs version of sadd,ssub,smul that return overflow flags aswell.
 * \todo Support mixed-type arithmetic using type propagation using std::common_type<T,U>::type or GCC's C++14 return type deduction.
 *
 * \see Add option to range check value during cast using std::numeric_limits<T> or ranged.min,max: http://www.boost.org/doc/libs/1_53_0/libs/numeric/conversion/doc/html/boost_numericconversion/improved_numeric_cast__.html
 */

#pragma once
#include <algorithm>
#include <ostream>
#include "algorithm_x.hpp"
#include "range.hpp"
#include "typenameof.hpp"
#include "clamp.hpp"
#include "saturate.hpp"

#ifndef NDEBUG
#include <string>
#endif

#if HAVE_BOOST_SERIALIZATION_ACCESS_HPP
#  include <boost/serialization/access.hpp>
#endif

#if HAVE_BOOST_LEXICAL_CAST_HPP
#  include <boost/lexical_cast.hpp>
#endif

/*! Inclusive Ranged Arithmetic Type Wrapper.
 *
 * \tparam X is true when exceptions should be thrown unless \c NDEBUG
 *         is set. If false saturation will occur.
 */
template<class T, bool X = true>
class ranged {
public:
     /*! Copy Construct needs no checking as its inherits correct relations from rhs \p a. */
    // ranged(const ranged<T>& a) { checked_set(a); }

    /*! Copy Assignment Construct needs to check. */
    ranged& operator = (const ranged& a) { checked_set(a.get()); }

    /*! \em Copy-Construct from Value \p a and Limits \p lims_. */
    ranged(const T&  a, const range<T>&  lims_) : m_lims(lims_) { checked_set(a); }
    /*! \em Move-Construct from Value \p a and Limits \p lims_. */
    ranged(const T&& a, const range<T>&  lims_) : m_lims(lims_) { checked_set(std::move(a)); }
    /*! \em Move-Construct from Value \p a and Limits \p lims_. */
    ranged(const T&  a, const range<T>&& lims_) : m_lims(std::move(lims_)) { checked_set(a); }
    /*! \em Move-Construct from Value \p a and Limits \p lims_. */
    ranged(const T&& a, const range<T>&& lims_) : m_lims(std::move(lims_)) { checked_set(std::move(a)); }

    /*! \em Copy-Construct from Value \p a and Limits (\p low, \p high). */
    ranged(const T&  a, const T&  low, const T&  high) : m_lims(low, high) { checked_set(a); }
    /*! \em Move-Construct from Value \p a and Limits (\p low, \p high). */
    ranged(const T&& a, const T&& low, const T&& high) : m_lims(std::move(low), std::move(high)) { checked_set(std::move(a)); }

    // ranged(const range<T>& lims) : m_(0), m_lims(lims) {}

    /*! Implicit Conversion to Wrapped Value (which is always a superset of the wrapper subtype). */
    operator T() const { return m_; }

#ifdef HAVE_CXX11_EXPLICIT
    explicit operator bool() const { return m_; } ///< \see http://stackoverflow.com/questions/6242768/is-the-safe-bool-idiom-obsolete-in-c11
#endif

    /*! Check if \p is include in \c this' range. */
    bool includes(const T& a) const {
#ifndef NDEBUG
        return m_lims.includes(a);
#else
        return true;
#endif
    }

    void trigger_error(const T& a) {
#ifndef NDEBUG
        // \see http://www.boost.org/doc/libs/1_53_0/doc/html/boost_lexical_cast/performance.html
        if (X) {
#if HAVE_BOOST_LEXICAL_CAST_HPP
            throw std::out_of_range("value a:" + boost::lexical_cast<std::string>(a) + " not within range:" + boost::lexical_cast<std::string>(lims()));
#elif HAVE_CXX11_TO_STRING
            throw std::out_of_range("value a:" + std::to_string(a) + " not within range:" + std::to_string(lims()));
#else
            throw std::out_of_range(std::string("value a not within range"));
#endif
        }
#endif
    }

    /*! Set to \p a if include in \c this' range otherwise throw \c std::out_of_range exception. */
    void checked_set(const T&  a) { if (includes(a)) { m_ = a;            } else { trigger_error(a); m_ = clamp(a, min(), max()); } }
    void checked_set(const T&& a) { if (includes(a)) { m_ = std::move(a); } else { trigger_error(a); m_ = clamp(a, min(), max()); } }

    /*! Get Wrapped Value. */
    const T& get() const { return m_; }
    /*! Get Wrapped Value. */
          T& get() { return m_; }

    /*! Get Inclusive Limits/Range. */
    const range<T>& lims() const { return m_lims; }
    /*! Get Inclusive Lower Limit. */
    T min() const { return m_lims.low(); }
    /*! Get Inclusive Upper Limit. */
    T max() const { return m_lims.high(); }

    friend ranged operator + (ranged a, ranged b) {
        return ranged(sadd(a.m_, b.m_),
                      a.lims().low() +
                      b.lims().low(),
                      a.lims().high() +
                      b.lims().high()); } // std::plus
    friend ranged operator - (ranged a, ranged b) {
        return ranged(ssub(a.m_, b.m_),
                      a.lims().low() -
                      b.lims().high(),
                      a.lims().high() -
                      b.lims().low()); } // std::minus
    friend ranged operator * (ranged a, ranged b) {
        const auto ll = a.lims().low() * b.lims().low();
        const auto lh = a.lims().low() * b.lims().high();
        const auto hl = a.lims().high() * b.lims().low();
        const auto hh = a.lims().high() * b.lims().high();
        return ranged(smul(a.m_, b.m_),
                      pnw::min(ll, lh, hl, hh),
                      pnw::max(ll, lh, hl, hh));
    }
    friend ranged operator / (ranged a, ranged b) {
        const auto ll = a.lims().low() / b.lims().low();
        const auto lh = a.lims().low() / b.lims().high();
        const auto hl = a.lims().high() / b.lims().low();
        const auto hh = a.lims().high() / b.lims().high();
        return ranged(sdiv(a.m_, b.m_), // \todo Implement saturated ranged division
                      pnw::min(ll, lh, hl, hh),
                      pnw::max(ll, lh, hl, hh));
    }

    /*! @name Arithmetic. */
    friend spure ranged inc(ranged a) { return ranged(a.m_ + 1); }
    friend spure ranged dec(ranged a) { return ranged(a.m_ - 1); }
    /*! Prefix Increment */
    ranged& operator ++ () { checked_set(get() + 1); return *this; }
    /*! Prefix Decrement */
    ranged& operator -- () { checked_set(get() - 1); return *this; }
    /*! Suffix Increment. */
    ranged operator ++ (int) { auto prev = *this; ++*this; return prev; } ///< Reuses prefix!
    /*! Suffx Decrement. */
    ranged operator -- (int) { auto prev = *this; --*this; return prev; } ///< Reuses prefix!

    /*! Comparison. */
    friend spure bool operator == (ranged a, ranged b) { return a.m_ == b.m_; }
    friend spure bool operator != (ranged a, ranged b) { return a.m_ != b.m_; }
    friend spure bool operator < (ranged a, ranged b) { return a.m_ < b.m_; }
    friend spure bool operator > (ranged a, ranged b) { return a.m_ > b.m_; }
    friend spure bool operator <= (ranged a, ranged b) { return a.m_ <= b.m_; }
    friend spure bool operator >= (ranged a, ranged b) { return a.m_ >= b.m_; }

    /*! Comparison with other types. */
    template<class U> friend spure bool operator == (ranged a, U b) { return a.m_ == b; }
    template<class U> friend spure bool operator != (ranged a, U b) { return a.m_ != b; }
    template<class U> friend spure bool operator <  (ranged a, U b) { return a.m_ < b; }
    template<class U> friend spure bool operator >  (ranged a, U b) { return a.m_ > b; }
    template<class U> friend spure bool operator <= (ranged a, U b) { return a.m_ <= b; }
    template<class U> friend spure bool operator >= (ranged a, U b) { return a.m_ >= b; }
    template<class U> friend spure bool operator == (U a, ranged b) { return a == b.m_; }
    template<class U> friend spure bool operator != (U a, ranged b) { return a != b.m_; }
    template<class U> friend spure bool operator <  (U a, ranged b) { return a < b.m_; }
    template<class U> friend spure bool operator >  (U a, ranged b) { return a > b.m_; }
    template<class U> friend spure bool operator <= (U a, ranged b) { return a <= b.m_; }
    template<class U> friend spure bool operator >= (U a, ranged b) { return a >= b.m_; }

    /*! https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B#Compound_assignment_operators */
    template<class U> ranged& operator += (ranged<U> a) { *this = *this + a; return *this; }
    template<class U> ranged& operator -= (ranged<U> a) { *this = *this - a; return *this; }
    template<class U> ranged& operator *= (ranged<U> a) { *this = *this * a; return *this; }
    template<class U> ranged& operator /= (ranged<U> a) { *this = *this / a; return *this; }
    template<class U> ranged& operator %= (ranged<U> a) { *this = *this % a; return *this; }
    template<class U> ranged& operator &= (ranged<U> a) { *this = *this & a; return *this; }
    template<class U> ranged& operator |= (ranged<U> a) { *this = *this | a; return *this; }
    template<class U> ranged& operator ^= (ranged<U> a) { *this = *this ^ a; return *this; }
    template<class U> ranged& operator <<= (ranged<U> a) { *this = *this << a; return *this; }
    template<class U> ranged& operator >>= (ranged<U> a) { *this = *this >> a; return *this; }

    /*! Min/Max */
    friend spure ranged min(ranged a, ranged b) { return ranged(std::min(a.m_, b.m_), intersect(a.lims(), b.lims())); }
    friend spure ranged max(ranged a, ranged b) { return ranged(std::max(a.m_, b.m_), intersect(a.lims(), b.lims())); }

    friend std::ostream& show(std::ostream& os, const ranged& a,
                              bool include_type = false) {
        os << a.m_ << " in " << a.lims();
        if (include_type) { os << typenameof(a); }
        return os;
    }
    friend std::ostream& operator << (std::ostream& os, ranged a) { return show(os, a); }

    /*! Iterative Power \p x^ \p a. */
    // friend spure ranged iter_pow(ranged x, ranged a) { return iter_pow<ranged>(x, a); }

#if HAVE_BOOST_SERIALIZATION_ACCESS_HPP
    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) {
        // TODO: We need to encode variant header using version.
#ifndef NDEBUG
        ar & m_ & m_lims;
#else
        ar & m_;
#endif
    }
#endif

private:
    T m_;                       ///< Value.
#ifndef NDEBUG
    range<T> m_lims;            ///< Value Limits.
#endif
};

template<class T, class S = std::string> inline spure const S typenameof(const ranged<T>& a) {
    return S("ranged<") + typenameof<T>() + ">";
    // return S("ranged<") + typenameof<T>() + "> [" + a.min() + "," + a.max() + "]";
}

// #include <limits>
namespace std {
template<class T> class numeric_limits;
template<class T> class numeric_limits<ranged<T> > {
public:
    static constexpr ranged<T> min() { return ranged<T>(std::numeric_limits<T>::min(),
                                                        std::numeric_limits<T>::min(),
                                                        std::numeric_limits<T>::max()); }
    static constexpr ranged<T> max() { return ranged<T>(std::numeric_limits<T>::max(),
                                                        std::numeric_limits<T>::min(),
                                                        std::numeric_limits<T>::max()); }
};
}
