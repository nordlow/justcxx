#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file median.h
 * \brief Median Calculations
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2010-03-17 09:15
 */

static inline pure int int_median3(int a, int b, int c)
{
  return (a < b ? (b < c ? b : a < c ? c : a) : (b > c ? b : a > c ? c : a));
}

static inline pure double double_median3(double a, double b, double c)
{
  return (a < b ? (b < c ? b : a < c ? c : a) : (b > c ? b : a > c ? c : a));
}

#ifdef __cplusplus
}
#endif
