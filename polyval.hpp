/*! \file polyval.hpp
 * \brief Operations on Polynomials.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-01-06 18:37
 */

#include <cmath>
#pragma once

/*! \name Evaluate polynomial of degree n with coefficients coeffs at value \p x.
 */
template<class T>
inline T T_polyval(T x, T *coeffs, int n)
{
    int j;
    T y = 0;
    for (j = 1; j < n; j++)
        y = coeffs[j] + x * y;
    return y;
}
