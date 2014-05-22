/*! \file hamming_distance.hpp
 * \brief Hamming Distance.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <algorithm>

namespace pnw {

/*! Return the \em Hamming Distance between the two sequences arguments \p a and \p b.
 *
 * The hamming distance is defined as the number of bits that differ between
 * two binary codes (of equal length).
 *
 * If \p a and \p b are of different length the minimum (call it m) is chosen. Then
 * the m first number of bits are compared.
 */
template<template<class> class T>
inline pure size_t hamming_distance(const T<bool> & a,
                                    const T<bool> & b)
{
    const size_t n = std::min(a.size(), b.size());
    size_t c = std::abs(a.size() - b.size());
    for (size_t i = 0; i < n; i++) // \todo Reuse reduction algorithm.
    {
        if (a[i] != b[i]) {
            c++;
        }
    }
    return c;
}

}
