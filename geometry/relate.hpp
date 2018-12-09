/*!
 * \file relate.hpp
 * \brief Calculate Spatial Relations such ordering and separation.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <limits>
#include "bitwise.hpp"
#include "box.hpp"

/*!
 * Determine the Spatial Relation between the two boxes \p a and \p b.
 *
 * \return relation as an integer that is > 0 if \p a and \p b are spatially
 * separated. Individually
 * - bit 0 is set if \p a lies before \p b along dimension 0 (\p a left \p b)
 * - bit 1 is set if \p b lies before \p a along dimension 0 (\p a right \p b)
 * - bit 2 is set if \p a lies before \p b along dimension 1 (\p a above \p b)
 * - bit 3 is set if \p a lies before \p b along dimension 1 (\p a below \p b)
 */
template<class T>
inline pure
uint relate(const box2<T> & a,
            const box2<T> & b)
{
    return (((a.u(0) < b.l(0)) << 0) bitor // left of
            ((a.l(0) > b.u(0)) << 1) bitor // right of
            ((a.u(1) < b.l(1)) << 2) bitor // above
            ((a.l(1) > b.u(1)) << 3));     // below
}

/*!
 * Determine the Spatial Relation between the three arguments \p a, \p b,
 * and \p c.
 *
 * \return an integer that is < \p N if describing the minimum separation found
 * like follows:
 * - 0: if union(\p a, \p b) lies beside \p a
 * - 1: if union(\p b, \p c) lies beside \p b
 * - 2: if union(\p c, \p a) lies beside \p c
 * - 3: no separation found
 */
template<class T, std::size_t N>
inline pure
int relate_unions(const box2<T> & a,
                  const box2<T> & b,
                  const box2<T> & c)
{
    // the bounding boxes involved
    const box2<T> bbs[3] = { a, b, c };

    // combined in three ways
    const box2<T> combs[3] = { ::unite(bbs[0], bbs[1]),
                               ::unite(bbs[1], bbs[2]),
                               ::unite(bbs[2], bbs[0]) };

    // and their relations
    const uint rels[3] = { relate(combs[0], bbs[2]),
                           relate(combs[1], bbs[0]),
                           relate(combs[2], bbs[1]) };

    // min separations
    T mseps[3] = { pnw::max_of<T>(),
                   pnw::max_of<T>(),
                   pnw::max_of<T>() };

    for (uint i = 0; i < N; i++) {
        for (uint j = 0; j < 3; j++) {
            // separation between combs[j] and bbs[(j+2) % 3]
            if (rels[j]) {
                if (pnw::get_bit(rels[j], 2 * i + 0)) {
                    mseps[j] = std::min(mseps[j],
                                        bbs[(j + 2) % 3].l(i) -
                                        combs[j].u(i));
                }
                if (pnw::get_bit(rels[j], 2 * i + 1)) {
                    mseps[j] = std::min(mseps[j],
                                        combs[j].l(i) -
                                        bbs[(j + 2) % 3].u(i));
                }
            }
        }
    }

    // error check
    if (mseps[0] < 0) { PERR("mseps[0] < 0\n"); }
    if (mseps[1] < 0) { PERR("mseps[1] < 0\n"); }
    if (mseps[2] < 0) { PERR("mseps[2] < 0\n"); }

    // choice index and separation
    uint min_i = 3;	// nothing found yet
    T min_sep = pnw::max_of<T>();

    // determine best way of separating x, y and z
    for (uint i = 0; i < 3; i++) {
        if (rels[i] > 0 and	// if separated
            (min_i >= 3 or // and if first separation found
             min_sep > mseps[i])) // or best separation yet
        {
            // then update
            min_i = i;
            min_sep = mseps[i];
        }
    }

    return min_i;
}
