/*! \file gridfit.hpp
 * \brief Fit Grids to Data.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <cmath>

/*! Determine the offset \p off of the Uniform Grid starting at zero with a grid
 * step of \p inc that best fits to the \p data.
 */
template<class T>
inline pure T gridfit(const std::vector<T> & data,
                                       const T& inc) {
    T off = 0;
    for (const auto& i : data) {
    // for (auto i = begin(data); i != end(data); i++) {
        T x = i - std::floor(i / inc) * inc;
        if (x >= inc / 2) { x -= inc; }
        // \c x is now the displacement of i relative to the closest
        // \c grid point defined by the regular grid starting at 0
        // \c with distance \c inc
        off += x;
    }
    return off / data.size();
}
