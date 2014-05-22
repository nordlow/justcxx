/*! \file peaks.hpp
 * \brief Find Data Peaks and Valleys.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-12 11:03
 * \see MATLAB's \c findpeaks()
 *
 * \see http://www.boost.org/doc/libs/1_49_0/libs/iterator/doc/filter_iterator.html
 * \todo Generalize to higher order filter function
 * - ix;[-1 0 1]
 * - [](It p, It m, It n) { return (*p < *m and *m > *n); }
 */

#pragma once
#include <vector>

/*!
 * Find \em Peak Indexes of all elements in [first, last].
 */
template<class InputIterator, class Ix = typename InputIterator::size_type>
inline pure
std::vector<Ix> peaks(const InputIterator first,
                      const InputIterator last)
{
    std::vector<Ix> ix;
    auto n = first;             // current
    if (n != last) {
        auto p = n++;          // previous
        if (n != last) {
            auto m = n++;       // middle
            Ix i = 1;       // (peak) index counter
            while (n != last) { // if one more element follow
                if (*p < *m and *m > *n) {
                    ix.push_back(i);
                }
                p = m;     // previous => middle
                m = n;     // middle => next
                n++; i++;
            }
        }
    }
    return ix;
}
template<class C, class Ix = typename C::size_type>
inline pure
std::vector<Ix> peaks(C x) { return peaks<typename C::const_iterator, Ix>(begin(x), end(x)); }

/*!
 * Find \em Valley Indexes of all elements in [first, last].
 */
template<class InputIterator, class Ix = typename InputIterator::size_type>
inline pure
std::vector<Ix> valleys(const InputIterator first,
                        const InputIterator last)
{
    std::vector<Ix> ix;
    auto n = first;             // current
    if (n != last) {
        auto p = n++;          // previous
        if (n != last) {
            auto m = n++;       // middle
            Ix i = 1;       // (valley) index counter
            while (n != last) { // if one more element follow
                if (*p > *m and *m < *n) {
                    ix.push_back(i);
                }
                p = m;     // previous => middle
                m = n;     // middle => next
                n++; i++;
            }
        }
    }
    return ix;
}
template<class C, class Ix = typename C::size_type>
inline pure
std::vector<Ix> valleys(C x) { return valleys<typename C::const_iterator, Ix>(begin(x), end(x)); }
