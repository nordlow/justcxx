/*! \file axticks.hpp
 * \brief Generate Array of Axis Ticks used in Data Graphs/Plots.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <cmath>

/*! Generate Plot Axis Ticks.
 * \param[in] min Axis Minimum
 * \param[in] max Axis Maximum
 * \param[out] step Resulting Step
 * \param[out] i0 Lower Limit
 * \param[out] i1 Upper Limit
 * \param[out] n Number of Steps
 */
template<typename T>
inline void gen_axticks(T & step, T & i0, T & i1,
                        size_t & n, const T & min, const T & max)
{
    const T span = max - min;
    step = std::pow(10, std::floor(std::log10(span / 2)));
    i0 = std::ceil(min / step);
    i1 = std::floor(max / step);
    n = static_cast<size_t>(i1 - i0) + 1;
}
