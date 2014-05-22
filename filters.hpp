/*! \file filters.hpp
 * \brief Digital Filters.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see https://en.wikipedia.org/wiki/Window_function
 * \see Tidsdiskreta Signal & System p. 145.
 */

#pragma once
#include "utils.hpp"

/*! Hamming Filter Tap at \p i / \p n.
 * Maximal sidolobsnivå: -43 dB.
 */
template<class T> inline T hamming_tap(size_t i, size_t n) {
    return (+ 0.54
            - 0.46 * std::cos(M_2PI * i / (n-1)));
}

/*! Blackman Filter Tap at \p i / \p n.
 * Maximal sidolobsnivå: -58 dB.
 */
template<class T> inline T blackman_tap(size_t i, size_t n) {
    return (+ 0.42
            - 0.5  * std::cos(M_2PI * i / (n-1))
            + 0.08 * std::cos(M_4PI * i / (n-1)));
}

/*! Hanning Filter Tap at \p i / \p n.
 * Maximal sidolobsnivå: -32 dB.
 *
 * Pro: Doesn't need normalization when used as sliding window with 50 percent
 * overlap.
 */
template<class T> inline T hanning_tap(size_t i, size_t n) {
    return (+ 0.5
            - 0.5  * std::cos(M_2PI * i / (n-1)));
}

/*! Generate Hamming Window \p a of resolution \p n.
 */
template<class T> inline void hamming(T* a, size_t n) {
    for (size_t i = 0; i < n; i++) { a[i] = hamming_tap(i, n); }
}

/*! Generate Blackman Window \p a of resolution \p n.
 */
template<class T> inline void blackman(T* a, size_t n) {
    for (size_t i = 0; i < n; i++) { a[i] = blackman_tap(i, n); }
}

/*! Generate Hanning Window \p of resolution \p n.
 */
template<class T> inline void hanning(T* a, size_t n) {
    for (size_t i = 0; i < n; i++) { a[i] = hanning_tap(i, n); }
}
