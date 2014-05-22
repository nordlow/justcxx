/*!
 * \file split.hpp
 * \brief Split up Containers in Several Parts.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "clamp.hpp"

namespace pnw
{
    /*!
     * Extract the \p sn elements in \p a starting at \p si and put it in
     * \p b. The rest is put in \p c.
     */
    template<typename T>
    inline void range_split(const T & a, T & b, T & c,
                            size_t si, size_t sn)
    {
        const size_t n = a.size();

        si = pnw::upper_clamp(si, n); // for safety
        sn = pnw::upper_clamp(sn, n - si); // for safety

        const size_t n1 = si;
        const size_t n2 = sn;
        const size_t n3 = n - (n1 + n2);

        b.resize(n2);
        c.resize(n1 + n3);

        std::copy(begin(a), begin(a) + n1, begin(c));
        std::copy(begin(a) + n1, begin(a) + n1 + n2, begin(b));
        std::copy(begin(a) + n1 + n2, end(a), begin(c) + n1);
    }

    /*! Split \p a into \p b and \p c.     */
    template<typename T>
    inline void mid_split(const T & a, T & b, T & c)
    {
        const size_t n = a.size();
        const size_t i = n / 2;

        b.resize(i);
        c.resize(n - i);

        std::copy(begin(a), begin(a) + i, begin(b));
        std::copy(begin(a) + i, end(a), begin(c));
    }
}
