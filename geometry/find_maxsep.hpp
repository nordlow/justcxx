/*! \file find_maxsep.hpp
 * \brief Find Maximum Separations of Geometric Primitves.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "../pdim.h"
#include "box.hpp"

/*! \name Determine separations. */
/// \{
/*! Determine the largest separation between the vectors \p a and \p b.
 * \return separating dimension if one was found, or \p N otherwise.
 */
template<class T, std::size_t N>
inline pdim256_t maxsep_vv(const vec2<T>& a,
                           const vec2<T>& b,
                           T& pos, bool& swap)
{
    pdim256_t best_i = N; // indicate nothing found yet
    T best_sep = 0;		// intialize to worst case
    for (pdim256_t i = 0; i < N; i++)
    {
        if (a(i) < b(i)) // separation (\p a and \p b) found
        {
            const T sep = b(i) - a(i);
            if (best_i == N or best_sep < sep) // first or better
            {
                best_i = i;
                best_sep = sep;
                pos = pnw::mean(a(i), b(i));
                swap = false;
            }
        }
        else if (b(i) < a(i)) // separation (b and a) found
        {
            const T sep = a(i) - b(i);
            if (best_i == N or best_sep < sep) // first or better
            {
                best_i = i;
                best_sep = sep;
                pos = pnw::mean(b(i), a(i));
                swap = true;
            }
        }
    }
    return best_i;
}

/*! Determine Largest Separation between the vector \p a and the box \p b.
 * \return separating dimension if one was found, or \p N otherwise.
 */
template<class T, std::size_t N>
inline pdim256_t maxsep_vb(const vec2<T>& a,
                           const box2<T>& b,
                           T& pos, bool& swap)
{
    pdim256_t best_i = N; // indicate nothing found yet
    T best_sep = 0;		// intialize to worst case

    for (pdim256_t i = 0; i < N; i++)
    {
        if (a(i) < b(0)(i)) // separation (\p a and \p b.lower) found
        {
            const T sep = b(0)(i) - a(i);
            if (best_i == N or best_sep < sep) // first or better
            {
                best_i = i;
                best_sep = sep;
                pos = pnw::mean(a(i), b(0)(i));
                swap = false;
            }
        }
        else if (b(1)(i) < a(i)) // separation (b.upper and a) found
        {
            const T sep = a(i) - b(1)(i);
            if (best_i == N or best_sep < sep) // first or better
            {
                best_i = i;
                best_sep = sep;
                pos = pnw::mean(b(1)(i), a(i));
                swap = true;
            }
        }
    }

    return best_i;
}

/*! Determine Largest Separation Between \p a and \p b.
 *
 * Try to find the best way of partitioning \p a and \p b from each other.
 *
 * \param[out] pos is assigned (if return value >= 0) the position along dimension
 * pdim where partitioning should be performed.
 *
 * \param[out] swap is true if the order of \p a and \p b should be swapped in order
 * for them to be sorted along pdim.
 *
 * \return partitioning dimension (0 <= pdim < \p N)
 * along which \p a and \p b are separated, or \p N if \p a and \p b overlap.
 */
template<class T, std::size_t N>
inline pdim256_t maxsep_bb(const box2<T>& a, const box2<T>& b,
                           T& pos, bool& swap)
{
    pdim256_t best_i = N; // indicate nothing found yet
    T best_sep = 0;		// intialize to worst case
    for (pdim256_t i = 0; i < N; i++)
    {
        if (a(1)(i) < b(0)(i)) // separation (\p a and \p b.lower) found
        {
            const T sep = b(0)(i) - a(1)(i);
            if (best_i == N or best_sep < sep) // first or better
            {
                best_i = i;
                best_sep = sep;
                pos = pnw::mean(a(1)(i), b(0)(i));
                swap = false;
            }
        }
        else if (b(1)(i) < a(0)(i)) // separation (b.upper and a) found
        {
            const T sep = a(0)(i) - b(1)(i);
            if (best_i == N or best_sep < sep) // first or better
            {
                best_i = i;
                best_sep = sep;
                pos = pnw::mean(b(1)(i), a(0)(i));
                swap = true;
            }
        }
    }
    return best_i;
}

/*! Determine Largest Separation between the \em Lower and \em Upper Corner of \p c.
 * \return separating dimension if one was found, or \p N otherwise.
 */
template<class T, std::size_t N>
pdim256_t maxsep_c(const box2<T>& c, T& pos)
{
    const vec2<T>& a = c(0);
    const vec2<T>& b = c(1);
    pdim256_t best_i = N; // indicate nothing found yet
    T best_sep = 0;		// intialize to worst case
    for (pdim256_t i = 0; i < N; i++) {
        const T sep = b(i) - a(i);
        if (best_i == N or best_sep < sep) // first or better
        {
            best_i = i;
            best_sep = sep;
            pos = pnw::mean(a(i), b(i));
        }
    }
    return best_i;
}

/*! Determine the best way of separating \p a, \p b and \p c.
 * \return separating dimension if one was found, or \p N otherwise.
 */
template<class T, std::size_t N>
inline pure int maxsep_bbb(const box2<T>& a,
                           const box2<T>& b,
                           const box2<T>& c)
{
    return 0;
}

/// \}
