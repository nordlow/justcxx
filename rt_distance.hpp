/*!
 * \file rt_distance.hpp
 * \brief Round Trip Distance.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <algorithm>
#include "norm.hpp"
#include "math_x.hpp"

/*! Round Trip Distance 1D */
template<class T> inline pure
T round_trip_distance_1d(const T *a, const uint *b, size_t n)
{
    size_t i;
    T cost = 0;
    for (i = 0; i < n-1; i++) {
        cost += std::abs(a[b[i+1]] - a[b[i]]);
    }
    cost += std::abs(a[b[n-1]] - a[b[0]]);
    return cost;
}

/*! Round Trip Distance 2D */
template<class T> inline pure
T round_trip_distance_2d(const T * a, const T * b, const uint *c, size_t n)
{
    size_t i;
    T cost = 0;
    for (i = 0; i < n-1; i++) {
        cost += eud2d(a[c[i+1]] - a[c[i]],
                      b[c[i+1]] - b[c[i]]);
    }
    cost += eud2d(a[c[n-1]] - a[c[0]],
                  b[c[n-1]] - b[c[0]]);
    return cost;
}

/*! Round Trip Distance 2D (l1-norm) */
template<class T> inline pure
T round_trip_distance_2dl1(const T * a, const T * b, const int *c, size_t n)
{
    size_t i;
    T adiff, bdiff;

    T costsum = 0;
    for (i = 0; i < n-1; i++) {
        adiff = std::abs(a[c[i+1]] - a[c[i]]);
        bdiff = std::abs(b[c[i+1]] - b[c[i]]);
        T cost = sqrt(std::sqr(adiff) + std::sqr(bdiff));
        if ((adiff != 1 or bdiff != 1) and (adiff * bdiff != 0))
        {
            cost *= 16;
        }
        costsum += cost;
    }

    adiff = std::abs(a[c[n-1]] - a[c[0]]);
    bdiff = std::abs(b[c[n-1]] - b[c[0]]);
    T cost = sqrt(std::sqr(adiff) + std::sqr(bdiff));
    if ((adiff != 1 or bdiff != 1) and (adiff * bdiff != 0)) {
        cost *= 16;
    }
    costsum += cost;

    return costsum;
}
