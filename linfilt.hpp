/*! \file linfilt.hpp
 * \brief Linear Filtering.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/*! Affine Transform in 2 Dimensions: \p x = \p x * \p a + \p b. */
template<class T>
inline void affine2d(T x[2], const T a[2], const T b[2])
{
    T x0 = x[0], x1 = x[1];
    x[0] = a[0] * x0 + a[0] * x1 + b[0];
    x[1] = a[1] * x0 + a[1] * x1 + b[1];
}

// Template Specializations

extern "C"
{

#include "linfilt.h"
}

inline void affine2d(float x[2], const float a[4], const float b[2])
{
    float_affine2d(x, a, b);
}

inline void pwss2d(float x[2],
                   const float a[2], const float b[2])
{
    fa2_ss(x, a, b);
}

inline void pwss2d(int y[2],
                   const float x[2], const float a[2], const float b[2])
{
    fa2_ss_i(y, x, a, b);
}

inline void pwss2d(short y[2],
                   const float x[2], const float a[2], const float b[2])
{
    fa2_ss_s(y, x, a, b);
}

inline void pwss2d(short y[2],
                   const float x[2], const float a[2], const float b[2],
                   const short s[2])
{
    fa2_ss_sc(y, x, a, b, s);
}
