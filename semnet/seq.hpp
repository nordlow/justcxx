/*! \file seq.hpp
 * \brief Pattern Sequence.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <iosfwd>
#include <vector>
#include <algorithm>
#include "patt.hpp"
#include "spatt.hpp"

namespace semnet { namespace patterns {

/*! Pattern \em Sequence of Adjacent Sub-Patterns. */
class Seq : public Multi
{
public:
    typedef std::vector<Base*> Seqs; ///< Combination Sub Patterns.

    Seq() : Multi() { init(); }
    Seq(const char * name) : Multi(name) { init(); }
    Seq(Base * sub) : Multi() { add(sub); init(); }
    Seq(Base ** subs, size_t subsN) : Multi() { add(subs, subsN); init(); }
    template<class... Tp>
    Seq(Tp*... args) : Multi() { add(args...); init(); }

    virtual ~Seq() { clear(); }

    /*! \em Compute Cache (if not already computed) for \p pA. */
    virtual void prepare() const;

    /*! \em Undefine Cache for \p pA.
     * \return 1 if cache was removed, 0 if cache was already gone.
     */
    virtual void unprepare() const;

    // friend Alt* operator . (Base* a, Base* b) { return (new Seq(a, b))->mutating_canonicalize(); }

public:
    virtual void update_sample_range() const;
    virtual size_t complexity(bir mults = bir::full()) const;
    virtual csc rand(bir ssr = bir::full()) const;

    virtual Base::Skips8& intersect_skips(Skips8& skips) const;

    virtual std::ostream& show(std::ostream& os) const;

public:
    virtual Base * add(Base * sub);
    /*! Useful in combined construct of \p sub and add of \p sub to \p this. */
    template<class T> T* add(T* sub) { add(reinterpret_cast<Base*>(sub)); return sub; }
    virtual void add(Base ** subs, size_t n);

    /*! Add Sub-Patterns \p pA. */
    template<class... Tp>
    void add(Tp*... subs) { Base* parr[sizeof...(subs)] = { subs... }; add(parr, sizeof...(subs)); } // C++11

    virtual Base * add_at_BOB(Base * sub, bix off = bix(0));

    virtual void clear() { remove_sub_supers(); m_subs.clear(); }
    virtual size_t size() const { return m_subs.size(); }
    virtual bool empty() const { return m_subs.empty(); }

    // TODO: Replace \c mutating_canonicalize with expression templates when use smart pointers.
    // friend Seq * operator& (Base * a, Base * b) {
    //     auto as = dynamic_cast<Seq*>(a);
    //     auto bs = dynamic_cast<Seq*>(b);
    //     if (as and bs) { return as->add(b); }
    //     else if (as)  { return as->add(b); }
    //     else if (bs)  { return bs->add(a); }
    //     else { return (new Seq(a, b))->mutating_canonicalize(); }
    // }

protected:
    void remove_sub_supers() {
      std::for_each(m_subs.begin(), m_subs.end(), // C++11
                    [=] (Base * sub) {
                      sub->remove_super(this); // uplink
                    });
        unprepare();
    }

    virtual pHit match_in_local(const char* buf, size_t len,
                                bir roi = bir::full()) const;

    virtual Base* mutating_canonicalize();

private:
    void init() {
        m_cacheL = 0;
    }

private:
    mutable Seqs m_subs;        ///< Combination \em Sub-Patterns.
    mutable std::unique_ptr<char> m_cacheA; ///< \em Cache Byte Array for combination of only \em adjacent \c pLits. */
    mutable size_t m_cacheL;    ///< \em Cache Byte Array Length for combination of only \em adjacent \c pLits. */
};

namespace gen {
inline Seq* seq() { return new Seq(); }
inline Seq* seq(Base * a) { return new Seq(a); }
/*! Construct using patterns \p a. */
template<class... T> inline Seq* seq(T... a) { return new Seq(a...); }
inline Seq* seq(Base ** a, size_t n) { return new Seq(a,n); }
}

}
}
