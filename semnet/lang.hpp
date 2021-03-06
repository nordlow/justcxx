/*! \file lang.hpp
 * \brief Pattern Language.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <algorithm>
#include <iosfwd>
#include "spatt.hpp"
//template<class T, class H, class P, class A> class unordered_set;

namespace semnet { namespace patterns {

/*! Language Top Pattern. */
class Lang : public Wrap {
public:
    Lang(const csc name, Base* sub);
    Lang(const char* name, Base* sub) : Lang(csc(name), sub) {}
    virtual ~Lang() {}
    virtual std::ostream& show(std::ostream& os) const;
protected:
//    boost::unordered_set<csc> m_wdict; ///< Language Words Dictionary.
};

namespace gen {
template<class... T> inline Lang* lang(T&&... a) { return new Lang(a...); }
}

}}
