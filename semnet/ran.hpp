/*! \file ran.hpp
 * \brief Pattern Alternative Range.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-07-04 15:19
 */

#pragma once
#include "alt.hpp"

namespace semnet { namespace patterns {

class Ran : public Alt {
public:
    Ran() : Alt() {}
    Ran(Base* a, Base* b, bool compl_flag = false) : Alt(a, b, compl_flag) {}
    Ran(uint8_t a, uint8_t b, bool compl_flag = false) : Alt(a, b, compl_flag) {}

    virtual void auto_set_detected_pmatch();

    virtual Base::Skips8& intersect_skips(Skips8& skips) const;

#if 0
    virtual size_t complexity(bir mults = bir::full()) const;
    virtual csc rand(bir ssr = bir::full()) const;
#endif
protected:
    virtual Base* mutating_canonicalize();
};

namespace gen {
/*! Construct using patterns \p pA. */
template<class... T> inline Ran * ran(T&&... a) { return new Ran(a...); }
}}

}
