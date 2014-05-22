/*!
 * \file atan2_adjusted.h
 * \brief Arcus Tangent (Tangens) (\c atan2()) adjusted to the range [0...2*\pi].
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Get \c atan2f() adjusted to [-PI, PI] => [0, 2PI] */
static inline float atan2f_0to2PI(float y, float x)
{
  float ang = atan2f(y, x);
  if (ang < 0) { ang += (float)M_2PI; } /* adjust [-PI, PI] => [0, 2PI] */
  return ang;
}

/*! Get \c atan2() adjusted to [-PI, PI] => [0, 2PI] */
static inline double atan2_0to2PI(double y, double x)
{
  double ang = atan2(y, x);
  if (ang < 0) { ang += (double)M_2PI; } /* adjust [-PI, PI] => [0, 2PI] */
  return ang;
}

/*! Get \c atan2l() adjusted to [-PI, PI] => [0, 2PI] */
static inline long double atan2l_0to2PI(long double y, long double x)
{
  long double ang = atan2l(y, x);
  if (ang < 0) { ang += (long double)M_2PI; } /* adjust [-PI, PI] => [0, 2PI] */
  return ang;
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
