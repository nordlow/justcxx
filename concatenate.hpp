/*! \file concatenate.hpp
 * \brief Concatenate.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-12-03 13:22
 */

#pragma once

namespace pnw
{

/*! Concatenate  Two Arguments \p a and \p b. */
template<class C> inline pure C concatenate(const C & a,
                                                              const C & b)
{
    C c(a.size() + b.size());
    std::copy(begin(a), end(a), begin(c));
    std::copy(begin(b), end(b), begin(c) + a.size());
    return c;
}

}
