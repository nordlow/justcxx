/*!
 * \file mid.hpp
 * \brief Calculations of Means (Averages) using Mid-Point to
 *        prevent overflow in integer template instances.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

namespace pnw {

/*!
 * Get Midway Value between \p a and \p b, where \p a < \p b.
 * Prevents overflows when working on integers.
 */
template<typename T>
inline pure T mid2(const T & a, const T & b)
{
    if (a < b) {
        return a + (b - a) / 2;
    } else {
        return b + (a - b) / 2;
    }
}

}

#endif
