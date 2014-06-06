/*! \file palt.hpp
 * \brief Pattern Alternative.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "spatt.hpp"
#include <iosfwd>
#include <memory>
#include <unordered_set>
#include <unordered_map>

// namespace boost { template<class K, class T> class unordered_map; } ///< Forward Declaration..

namespace semnet {
namespace patterns {

/*! Pattern \em Alternative.
 */
class Alt : public Multi {
public:
    typedef std::unordered_set<Base*> Alts;   ///< Alternatives.
    typedef std::unordered_map<csc, Base*> Hashes; ///< Alternatives.

    Alt()
        : Multi(), m_complement(false) {
    }
    Alt(const char * name)
        : Multi(name), m_complement(false) {
    }
    Alt(Base * sub, bool complement = false)
        : Multi(), m_complement(complement) {
        add(sub);
    }
    Alt(Base * subA, Base * subB, bool complement = false)
        : Multi(), m_complement(complement) {
        add(subA, subB);
    }
    Alt(Base ** subs, size_t subsN, bool complement = false)
        : Multi(), m_complement(complement) {
        add(subs, subsN);
    }
    Alt(const char* datA[], const size_t sizeA[], size_t num);
    Alt(const char* datA[], size_t num);

    Alt(uint8_t a, uint8_t b, bool complement = false);
    template<class... T> Alt(T*... args) { add(args...); }

public:
    virtual Base * add(Base * sub);
    virtual void add(Base ** subs, size_t n);

    /*! Useful in combined construct of \p sub and add of \p sub to \p this. */
    template<class T> T* add(T* sub) { add(reinterpret_cast<Base*>(sub)); return sub; }

    /*! Add Sub-Patterns \p pA. */
    template<class... T>
    void add(T*... subs) { Base* arr[sizeof...(subs)] = { subs... }; add(arr, sizeof...(subs)); } // C++11

    virtual void clear() { remove_sub_supers(); m_subs.clear(); }
    virtual size_t size() const { return m_subs.size(); }
    virtual bool empty() const { return m_subs.empty(); }

public:
    virtual size_t complexity(bir mults = bir::full()) const;

    virtual void update_sample_range() const;

    virtual csc rand(bir ssr = bir::full()) const;

    virtual Base::Skips8& intersect_skips(Skips8& skips) const;

    virtual std::ostream& show(std::ostream& os) const;

    /*! Set Complementary Status to \p complement. */
    Alt* set_complement(bool complement) { if (m_complement != complement) { unprepare(); } m_complement = complement; return this; }

    // TODO: Replace \c mutating_canonicalize with expression templates when use smart pointers.
    // friend Alt* operator | (Base* a, Base* b) { return (new Alt(a, b))->mutating_canonicalize(); }

protected:
    void remove_sub_supers() {
      std::for_each(m_subs.begin(),
                    m_subs.end(),
                    [&](Base * sub) { sub->remove_super(this); }); // uplink
        unprepare();
    }

    bool detect_range() const;

    /*! Compute Hash Table (if not already computed) for \p pA. */
    virtual void prepare() const;

    /*! Undefine Hash Table for \p pA. */
    virtual void unprepare() const;

    virtual Base* mutating_canonicalize();

public:
    /*! Perform Hash Table Lookup for \p pA. */
    Base* hash_match(const char *buf, size_t len) const;

    virtual pHit match_in_local(const char* buf, size_t len,
                                bir roi = bir::full()) const;

protected:
    Alts m_subs;                        ///< Sub Alternatives.
    mutable std::unique_ptr<Hashes> m_hashA; ///< Hash Table for Alternatives.

    /*! \em Complement Flag. Like the regexp <code>[^a]</code> matches all \em but 'a'. */
    bool m_complement:1;
};

namespace gen {
inline Alt* alt() { return new Alt(); }
inline Alt* alt(const char * name) { Alt* palt = new Alt(); palt->set_name(name); return palt; }
inline Alt* alt(Base * pA, bool complement = false) { return new Alt(pA, complement); }
inline Alt* alt(Base ** pB_A, size_t pB_N, bool complement = false) { return new Alt(pB_A, pB_N, complement); }
inline Alt* alt(Base * pA, Base * pB, bool complement = false) {
    Base *arr[] = { pA, pB };
    Alt* palt = alt(arr, ARRAY_LENGTH(arr), complement);
    return palt;
}
/*! Construct using patterns \p pA. */
template<class... T> inline Alt* alt(T&&... a) { return new Alt(a...); }

/*! \name add_pAlt_from_cstrA - string
 * \param valA - Character val A
 * \param nameA - Name of
 * \param altM -  alt M
 * \param common_name - Name of
 * \return Alt*
 * \todo Make this protected, and move all calls to this to a
 *       member function inside a class the matches the C/C++ language
 *       and make this class a friend of pReg.
 */
Alt* alt_cstrA(const char** valA, const char** nameA, size_t altM, const char* common_name);

}
}
}
