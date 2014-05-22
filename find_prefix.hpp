/*!
 * \file find_prefix.hpp
 * \brief Find Common Prefix.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <algorithm>

/*! Find (Byte) Prefix common to \p num number of byte arrays in \p datA each of length \p sizeA.
 * \return length of prefix in \em bytes.
 */
template<typename T>
inline pure size_t
find_byte_prefix(const T * datA[], const size_t sizeA[], size_t num)
{
    size_t pc = 0;              // common prefix counter
    const size_t szmin = *std::min_element(sizeA, sizeA+num); // minimum size of all \c datA
    for (size_t i = 0; i < szmin; i++) { // for each potential common prefix character
        for (size_t j = 1; j < num; j++) { // for all array elements
            if (datA[j-1][i] != datA[j][i]) { break; }
        }
        pc++;
    }
    return pc;
}
