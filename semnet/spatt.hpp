/*! \file spatt.hpp
 * \brief SemNet Super Pattern Specifier.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "patt.hpp"

namespace semnet {
namespace patterns {

/*! \em Super (Non-Terminal) Pattern Specifier/Description.
 *
 * Specifies a \em raw \em logical data pattern in a \em finite
 * \em consecutive array of bytes (uchar*).
 *
 * Used as a parser \em reduction rule of its REL_CHILDs.
 */
class Super : public Base {
public:
    Super() : Base() { init(); }
    Super(const char* name) : Base(name) { init(); }
    Super(const csc& name) : Base(name) { init(); }

    Super(pHit (*cbuf_lmatchP)(const char * pbuf, size_t len, bix off, pMode pmm)) : Base(cbuf_lmatchP) { init(); }

    virtual bir sample_range() const;

    virtual ~Super() {}

    /// Tokenize/Lexicalize Pattern, that is tag it as \em atomic (in the context of its programming language).
    Super * tokenize() { m_token = true; return this; }

protected:
    void init() {
        m_irange.undefine();
        m_multI_cache_ok_flag = false;
        m_pexpandP = nullptr;
        m_preduceP = nullptr;
    }

    virtual void prepare() const;
    virtual void unprepare() const;

protected:
    /*! * Pattern \em Expand function. */
    int (*m_pexpandP)();

    /*! Pattern \em Reduce function. */
    int (*m_preduceP)();

    mutable bir m_irange;   ///< Instance Range (Minimum and Maximum) (cache).
    mutable bix m_multI;   ///< Instance (Variation) \em Multiplicity (cached). Number of ways a pattern can be instantiated.
    mutable bool m_multI_cache_ok_flag:1; ///< Flags that \c m_multI is up-to-date.

    /* Indicates that this pattern is a language \em lexical \em token (atomic part of a
     * language). to distinguish it from higher level syntactic parts.
     */
    bool m_token:1;

private:
    /*! Caching Delegate for \c sample_range. */
    virtual void update_sample_range() const = 0;
};

/*! \em Single-Argument (Unary) Super \em Wrapper Pattern.
 */
class Wrap : public Super {
public:
    Wrap() : Super(), m_sub(nullptr) {}
    Wrap(Base * sub) : Super(), m_sub(sub) { m_sub->add_super(this); }
    Wrap(const char* name, Base * sub = nullptr) : Super(name), m_sub(sub) { if (m_sub) { m_sub->add_super(this); } }
    Wrap(const csc& name, Base * sub = nullptr) : Super(name), m_sub(sub) { if (m_sub) { m_sub->add_super(this); } }

    virtual ~Wrap() { if (m_sub) { m_sub->remove_super(this); } }

    virtual csc rand(bir ssr = bir::full()) const { return m_sub->rand(ssr); }

    virtual Base::Skips8& intersect_skips(Skips8& skips) const { return m_sub->intersect_skips(skips); }

    virtual void update_sample_range() const { m_irange = m_sub->sample_range(); }

    virtual size_t complexity(bir mults = bir::full()) const {
        return m_sub->complexity(mults);
    }

    virtual pHit match_in_local(const char * buf, size_t len,
                                bir roi = bir::full()) const {
        return m_sub->match_in_local(buf, len, roi);
    }

protected:
    virtual Base* mutating_canonicalize() { return m_sub->mutating_canonicalize(); }

protected:
    Wrap * set_sub(Base * sub) {
        if (m_sub) { m_sub->remove_super(this); } // remove old
        m_sub = sub;
        if (sub) { sub->add_super(this); } // store new
        return this;
    }

protected:
    Base * m_sub;               ///< Sub Pattern.
};

/*! \em Multi-Argument (N-Ary) Super \em Combiner Pattern.
 * Sub-Pattern Storage is defined by inheriting sub-classes.
 */
class Multi : public Super {
public:
    Multi() : Super() {}
    Multi(const char * name) : Super(name) {}
protected:
    virtual Base * add(Base * sub) = 0;
    virtual void add(Base ** subs, size_t n) = 0;

    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
};

}
}
