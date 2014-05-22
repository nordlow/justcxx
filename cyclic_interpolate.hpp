/*! \file cyclic_interpolate.hpp
 * \brief Cyclic Interpolate.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/
 */

#pragma once
#include <vector>
#include "interpolate.hpp"

/*! Edge Interpolation Type. */
typedef enum {
    INTERPOLATE_EDGE_CONSTANT,
    INTERPOLATE_EDGE_FADING,
    INTERPOLATE_EDGE_CYCLIC,
} INTERPOLATE_EDGE_t;

/*! Cyclic Linear 1-Dimensional Interpolated Sampling of data x at* position i.
 *
 * We can visualize the algorithm using a bar histogram where the data elements
 * in x are represented by bars centered at position 0.5, 1.5, 2.5.
 *
 * The return value at position \p i is described as the linear interpolation of
 * the two samples in x that lies closest to i.
 */
template<class Ta, class Tb>
inline pure
Ta cyclic_linear_interpolate_1d_sample(const std::vector<Ta>& x, Tb i,
                                       INTERPOLATE_EDGE_t eit)
{
    const uint m = x.size();
    i = std::fmod(i, m);        // cyclic indexing
    Tb fi = i - std::floor(i);	// fractional part of i
    uint a, b;                  // sampling indexes
    Tb wa, wb;                  // and their weigths
    if (fi < 0.5)
    {
        b = static_cast<uint>(std::floor(i));
        wa = + 0.5 - fi;
        wb = + 0.5 + fi;
    }
    else			// fi >= 0.5
    {
        b = static_cast<uint>(std::ceil(i));
        wa = + 1.5 - fi;
        wb = - 0.5 + fi;
    }
    if (b > 0) { a = b - 1; } else { a = m - 1; }
    // std::cout << "i:" << i << " "
    //           << "fi:" << fi << " "
    //           << "a:" << a << " "
    //           << "b:" << b << " "
    //           << "wa:" << wa << " "
    //           << "wb:" << wb << std::endl;
    return linear_interpolate(x[a], x[b], wb, 1.0);
}

#ifdef ARMA_INCLUDES
/*!
 * Cyclic linear 1-dimensional interpolated sampling of data x at
 * position i, j.
 */
template<class Ta, class Tb>
inline pure
Ta cyclic_linear_interpolate_1d_sample(const arma::Mat<Ta>& x,
                                       uint i, Tb j,
                                       INTERPOLATE_EDGE_t eit)
{
    const uint n = x.n_cols();
    j = std::fmod(j, n);	// cyclic indexing
    Tb fj = j - std::floor(j);	// fractional part of j
    uint a, b;			// sampling indexes
    Tb wa, wb;			// and their weigths
    if (fj < 0.5) {
        b = static_cast<uint>(std::floor(j));
        wa = + 0.5 - fj;
        wb = + 0.5 + fj;
    } else {                    // fj >= 0.5
        b = static_cast<uint>(std::ceil(j));
        wa = + 1.5 - fj;
        wb = - 0.5 + fj;
    }
    if (b > 0) { a = b - 1; } else { a = n - 1; }
    // interpolate along (second) column dimension
    return linear_interpolate(x(i, a), x(i, b), wb, 1.0);
}

/*!
 * Cyclic linear Two-dimensional interpolated sampling of the matrix x at
 * [row \p i, column \p j].
 *
 * Used in raytracing when performing spherical texture mapping. In that case
 * [\p j, \p i] become the texture mapping coordinates [u, v].
 */
template<class Ta, class Tb>
inline pure
Ta cyclic_linear_interpolate_2d_sample(const arma::Mat<Ta>& x,
                                       Tb i, Tb j,
                                       INTERPOLATE_EDGE_t eit)
{
    const uint m = x.n_rows();
    i = std::fmod(i, m);        // cyclic indexing
    Tb fi = i - std::floor(i);	// fractional part of i
    uint c, d;                  // sampling indexes
    Tb wc, wd;                  // and their weigths
    if (fi < 0.5) {
        d = static_cast<uint>(std::floor(i));
        wc = + 0.5 - fi;
        wd = + 0.5 + fi;
    } else {                    // fi >= 0.5
        d = static_cast<uint>(std::ceil(i));
        wc = + 1.5 - fi;
        wd = - 0.5 + fi;
    }
    if (d > 0) { c = d - 1; } else { c = m - 1; }
    // interpolate along (first) row dimension
    return linear_interpolate_1d(cyclic_linear_interpolate_1d_sample(x, c, j, eit),
                                 cyclic_linear_interpolate_1d_sample(x, d, j, eit),
                                 wd, 1.0);
}
#endif
