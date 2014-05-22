/*!
 * \file code_vli.hpp
 * \brief Variable Length Integer Coding (VLIC).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-13 16:10
 */

#pragma once
#include "bitwise.hpp"

/*!
 * Linear Variable Length Integer Encode (VLIC) \p a to \p bs.
 */
template<class BitSet, class Int>
inline BitSet&
lvli_encode(BitSet& bs, Int a)
{
    if (a == 0) {               // handle special case
        bs << true;             // because binlog is not defined for 0
    } else {
        const size_t n = pnw::binlog(a);
        showln(a);
        for (size_t i = 0; i < n; ++i) { bs << false; } // FUNC \c dotimes()
        bs << true;
    }
    return bs;
}

/*!
 * Linear Variable Length Integer Decode (VLIC) \p a from \p bs.
 */
template<class BitSet, class Int>
inline BitSet&
lvli_decode(BitSet& bs, Int & a)
{
    a = 1;
    return bs;
}
