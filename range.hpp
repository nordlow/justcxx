/*! \file range.hpp
 * \brief Range and Bit Index Range.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \keywords bit,index,range
 * \todo Reuse vec2<T> vec.hpp
 * \todo Merge with http://xanduchene.wordpress.com/2013/03/17/pythonic-ranges-in-c11/
 * \todo http://stackoverflow.com/questions/13453949/ada-style-range-types-in-d
 */

#pragma once
#include <iostream>
#include <algorithm>
#include <limits>
#include <boost/serialization/access.hpp>
#include "enforce.hpp"

/*! Inclusive Range. */
template<class T = size_t>
class range : public std::pair<T,T>
{
public:
    static constexpr T lowest() { return std::numeric_limits<T>::min(); }
    static constexpr T highest() { return std::numeric_limits<T>::max(); }

    /// Return \em Full Range.
    static constexpr range none() { return range(lowest(), lowest()); }
    static constexpr range full() { return range(lowest(), highest()); }

    /// Create Default as Undefined Range.
    range() : range(highest(), lowest()) {}
    /// Create \em One-Element Range at \p b.
    // range(T b) : range(b, b) {}
    /// Create  at \p b of length \p e - \p b.
    range(const T& low_, const T& high_) : std::pair<T,T>(low_, high_) {}

    /// Copy Construct from \p p should no be given here.
    // range(std::pair<T,T> p) : std::pair<T,T>(p) {}

    /// Create from Scalar \p p.
    range(T s) : std::pair<T,T>(s, s) {}

    /// Reset to Zero (Empty).
    void zero()  { this->first = lowest(); this->second = lowest(); }
    /// Reset to Zero (Empty).
    void clear() { this->first = highest(); this->second = lowest(); }

    // Trivial STL Iterator Compliance
    // T& begin() { return this->first; }
    // T begin() const { return this->first; }
    // T cbegin() const { return this->first; }
    // T& end() { return this->second; }
    // T end() const { return this->second; }
    // T cend() const { return this->second; }

    T& low()        { return this->first; }
    T& high()       { return this->second; }
    T  low()  const { return this->first; }
    T  high() const { return this->second; }

    range& set_low(T l)  { this->first = l; return *this; }
    range& set_high(T h) { this->second = h; return *this; }

    T& push_front() { enforce_lt(this->first, this->second); return --(this->first); }
    T& pop_front()  { enforce_lt(this->first, this->second); return ++(this->first); }
    T& push_back()  { enforce_lt(this->first, this->second); return ++(this->second); }
    T& pop_back()   { enforce_lt(this->first, this->second); return --(this->second); }

    // T& operator++(T& a) { return a++; }
    // T operator*(T& a) { return a; }

    T bitsize() const { return (high() - low()); }

    bool is_zero() const { return low() == lowest() and high() == lowest(); }

    // Check if range is empty.
    bool empty() const { return (low() == high()); }
    operator bool() const { return not empty(); }
    /// Undefine Prior to sucessive calls to \c include.
    range& undefine() {
        low() = highest();
        high() = lowest();
        return *this;
    }
    /// Check if \em defined.
    bool defined() const { return (low() != highest() and
                                   high() != lowest()); }

    friend range unite(const range& a,
                       const range& b) {
        return range(a.low() + b.low(),
                     a.high() + b.high());
    }

    friend range intersect(const range& a,
                           const range& b) {
        return range(std::max(a.low(), b.low()),
                     std::min(a.high(), b.high()));
    }

    /// Include \p a.
    range& include(const range& a) { return *this = unite(*this, a); }
    /// Includes \p a.
    bool includes(const T& a) const { return (low() <= a and a <= high()); }

    /// @name Saturated Addition.
    /// \{
    /// \em Right-Shift range \p b by \p a indexes.
    friend range operator + (const T& a, const range& b) { return range(a + b.low(),
                                                                        a + b.high()); }
    /// \em Right-Shift range \p a by \p b indexes.
    friend range operator + (const range& a, const T& b) { return range(a.low() + b,
                                                                        a.high() + b); }
    /// Sum the the begin and end limits of \p a and \p b.
    friend range operator + (const range& a, const range& b) { return range(a.low() + b.low(),
                                                                            a.high() + b.high()); }
    /// \}

    /// @name Saturated Subtraction.
    /// \{
    /// \em Left-Shift range \p b by \p a indexes.
    friend range operator - (const T& a, const range& b) { return range(a + b.low(),
                                                                        a + b.high()); }
    /// \em Right-Shift range \p a by \p b indexes.
    friend range operator - (const range& a, const T& b) { return range(a.low() + b,
                                                                        a.high() + b); }
    /// Subtract the the begin and end limits of \p a and \p b.
    friend range operator - (const range& a, const range& b) { return range(a.low() + b.low(),
                                                                            a.high() + b.high()); }
    /// \}

    /// Sequence \p a and \p b.
    friend range operator& (const range& a, const range& b) { return a + b; }
    /// Alternative \p a or \p b.
    friend range operator | (const range& a, const range& b) { return unite(a, b); }

    /// Append with \p a.
    range& append(const T& a) { return *this = *this + a; }
    range& append(const range& a) { return *this = *this + a; }
    range& operator += (const T& a) { return append(a); }
    range& operator += (const range& a) { return append(a); }

    friend std::ostream& operator << (std::ostream& os, const range& a) {
        os << '[';
        if (a.defined()) {
            os << a.low() << ',' << a.high();
        } else {
            os << "undefined,undefined";
        }
        os << ']';
        return os;
    }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & std::pair<T,T>(*this); }
};
