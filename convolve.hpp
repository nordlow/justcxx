/*!
 * \file convolve.hpp
 * \brief Convolution or Polynomial Multiplication.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <algorithm>

/*! Convolution or polynomial multiplication.
 * Length of out must be at least n1 + n2 - 1.
 */
template<typename T>
void convolve(T * out, const T * in1, const T * in2,
              int n1, int n2)
{
    /* Make in1 the larger vector with length n1
       and  in2 the shorter vector with length n2. */
    if (n1 < n2)
    {
        std::swap(in1, in2);
        std::swap(n1, n2);
    }

    int o = 0;                  // reset out index

    for (int m = 1; m < n2; m++, o++)
    {
        out[o] = 0;
        for (int j = 0; j < m; j++) // if n2 > 1
        {
            out[o] += in1[j] * in2[m - 1 - j];
        }
    }

    for (int m = 0; m <= n1 - n2 ; m++ , o++)
    {
        out [o] = 0;
        // \todo FLIp in2 and call normal dot which can be reused.
        for (int j = 0; j < n2; j++) // Always
        {
            out [o] += in1 [m + j] * in2 [n2 - 1 - j];
        }
    }

    for (int m = n2 - 1; m >= 1; m--, o++)
    {
        out[o] = 0;
        for (int j = 0; j < m; j++) // if n2 > 1
        {
            out[o] += in1[n1 - m + j] * in2[n2 - 1 - j];
        }
    }
}

/*! Convolution or Polynomial Multiplication.
 * \todo We could reuse convolve...
 */
template<typename T>
void convolve(T **out, const T **in1, const T **in2,
              int w1, int h1, int w2, int h2)
{
    /* Make in1 the larger matrix with lengths w1 and h1
       and  in2 the shorter matrix with lengths w2 and h2. */
    if (w1 * h1 < w2 * h2)
    {
        std::swap(in1, in2);
        std::swap(w1, w2);
        std::swap(h1, h2);
    }
    // \todo ...
}
