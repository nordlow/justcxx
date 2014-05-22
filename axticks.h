/*!
 * \file axticks.h
 * \brief Generate Array of Axis Ticks used in Data Graphs/Plots.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <math.h>
#include "pnw_types.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \name Generate Plot Axis Ticks.
 *
 * \param[out] step returns the Resulting \em Step
 * \param[out] i0 returns the \em Lower Limit
 * \param[out] i1 returns the \em Upper Limit
 * \param[out] n returns the \em Number of Steps
 */

/* @{ */

static inline void
gen_axticksf(float *step, float *i0, float *i1,
             uint *n, float gmin, float gmax)
{
  const float gspan = gmax - gmin;
  *step = powf(10, floorf(log10f(gspan / 2)));
  *i0 = ceilf(gmin / *step);
  *i1 = floorf(gmax / *step);
  *n = (size_t) (*i1 - *i0) + 1;
}

static inline void
gen_axticksd(double *step, double *i0, double *i1,
             uint *n, double gmin, double gmax)
{
  const double gspan = gmax - gmin;
  *step = pow(10, floor(log10(gspan / 2)));
  *i0 = ceil(gmin / *step);
  *i1 = floor(gmax / *step);
  *n = (size_t) (*i1 - *i0) + 1;
}

/* @} */

#ifdef __cplusplus
}
#endif
