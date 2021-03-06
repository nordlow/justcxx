/*! \file ctx.hpp
 * \brief Pattern Repetition.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "spatt.hpp"
#include <iosfwd>
#include <algorithm>
#include <limits>

namespace semnet {
namespace patterns {

/*! Pattern \em Repetition. */
class Rep : public Wrap {
public:
    typedef size_t T;           // Repetition Count Type.
    typedef std::pair<T,T> RRan; // Repetition Range Type.

    Rep(T rfixed,
        Base * sub = nullptr,
        bool greedy = false)
        : Wrap(sub), m_rrange(rfixed, rfixed), m_greedy(greedy) {}

    Rep(RRan rrange = RRan(1, std::numeric_limits<size_t>::max()),
        Base * sub = nullptr,
        bool greedy = false)
        : Wrap(sub), m_rrange(rrange), m_greedy(greedy) {}

    virtual void update_sample_range() const;
    virtual size_t complexity(bir mults = bir::full()) const;
    virtual csc rand(bir ssr = bir::full()) const;

    virtual Base::Skips8& intersect_skips(Skips8& skips) const;

    virtual std::ostream& show(std::ostream& os) const;

    Rep * set_greedy(bool greedyF) { m_greedy = greedyF; return this; }

    RRan range() const { return m_rrange; }

    virtual Base* mutating_canonicalize();

    bool fixed() const { return (m_rrange.first ==
                                 m_rrange.second); }
    T req_count() const { return (m_rrange.first); }
    T opt_count() const { return (m_rrange.second - m_rrange.first); }

    // friend Rep* operator * (Base* a, RRan b) { return (new Rep(b, a))->mutating_canonicalize(); }
    // friend Rep* operator * (RRan a, Base* b) { return (new Rep(a, b))->mutating_canonicalize(); }

protected:
    virtual pHit match_in_local(const char * buf, size_t len,
                                bir roi = bir::full()) const;

private:
    RRan m_rrange; ///< Repetition Range.

    /*! Greedy Repetition, match characters that are otherwise matched by
     * following \c Seq - \c REL_SIBLINGS .
     */
    bool m_greedy:1;
};

namespace gen {
inline Rep * two(Base * sub) { return new Rep(2, sub); }
inline Rep * three(Base * sub) { return new Rep(3, sub); }
inline Rep * four(Base * sub) { return new Rep(4, sub); }
inline Rep * five(Base * sub) { return new Rep(5, sub); }
inline Rep * six(Base * sub) { return new Rep(6, sub); }
inline Rep * seven(Base * sub) { return new Rep(7, sub); }
inline Rep * eight(Base * sub) { return new Rep(8, sub); }
inline Rep * nine(Base * sub) { return new Rep(9, sub); }
inline Rep * ten(Base * sub) { return new Rep(10, sub); }
inline Rep * eleven(Base * sub) { return new Rep(11, sub); }
inline Rep * twelve(Base * sub) { return new Rep(12, sub); }

inline Rep * rep0o1(Base * sub) { return new Rep(Rep::RRan(0, 1), sub); }
inline Rep * rep0o2(Base * sub) { return new Rep(Rep::RRan(0, 2), sub); }
inline Rep * rep0o3(Base * sub) { return new Rep(Rep::RRan(0, 3), sub); }
inline Rep * rep0oM(Base * sub) { return new Rep(Rep::RRan(0, UINT_MAX), sub); }

inline Rep * rep1o2(Base * sub) { return new Rep(Rep::RRan(1, 2), sub); }
inline Rep * rep1o3(Base * sub) { return new Rep(Rep::RRan(1, 3), sub); }
inline Rep * rep1o4(Base * sub) { return new Rep(Rep::RRan(1, 4), sub); }
inline Rep * rep1oM(Base * sub) { return new Rep(Rep::RRan(1, UINT_MAX), sub); }

inline Rep * rep3o4(Base * sub) { return new Rep(Rep::RRan(3, 4), sub); }
inline Rep * rep5o6(Base * sub) { return new Rep(Rep::RRan(5, 6), sub); }
inline Rep * rep7o8(Base * sub) { return new Rep(Rep::RRan(7, 8), sub); }

extern Rep * repREchar(Base * sub, const char re_char);

template<class... T> inline Rep* rep(T&&... a) { return new Rep(a...); }
}

}
}
