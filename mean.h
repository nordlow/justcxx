/*!
 * \file mean.h
 * \brief Arithmetic Mean (Normalized Addition)
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-11-28 11:03
 */

#pragma once

#include "pnw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Get Midway Value between \p a and \p b, where \p a < \p b.
 * Prevents overflows when working on integers.
 */
static inline __attribute__ ((const)) char char_mid2(char a, char b)
{
  if (a < b) {
    return a + (b - a) / 2;
  } else {
    return b + (a - b) / 2;
  }
}

/*!
 * Get Midway Value between \p a and \p b, where \p a < \p b.
 * Prevents overflows when working on integers.
 */
static inline __attribute__ ((const)) short short_mid2(short a, short b)
{
    if (a < b) {
        return a + (b - a) / 2;
    } else {
        return b + (a - b) / 2;
    }
}

/*!
 * Get Midway Value between \p a and \p b, where \p a < \p b.
 * Prevents overflows when working on integers.
 */
static inline __attribute__ ((const)) int int_mid2(int a, int b)
{
    if (a < b) {
        return a + (b - a) / 2;
    } else {
        return b + (a - b) / 2;
    }
}

/*!
 * Get Midway Value between \p a and \p b, where \p a < \p b.
 * Prevents overflows when working on integers.
 */
static inline __attribute__ ((const)) uint32_t uint32_mid2(uint32_t a, uint32_t b)
{
  if (a < b) {
    return a + (b - a) / 2;
  } else {
    return b + (a - b) / 2;
  }
}

/*!
 * Get Midway Value between \p a and \p b, where \p a < \p b.
 * Prevents overflows when working on integers.
 */
static inline __attribute__ ((const)) uint64_t uint64_mid2(uint64_t a, uint64_t b)
{
  if (a < b) {
    return a + (b - a) / 2;
  } else {
    return b + (a - b) / 2;
  }
}

/* ========================================================================= */

/*!
 * Calculate the \em Average of \p a and \p b.
 */
static inline __attribute__ ((const)) float float_mean2(float a, float b)
{
  return (a + b) / 2.0f;
}

/*!
 * Calculate the \em Average of \p a and \p b.
 */
static inline __attribute__ ((const)) double double_mean2(double a, double b)
{
  return (a + b) / 2.0;
}

/* ========================================================================= */

/*!
 * Calculate the \em Average of \p a and \p b.
 */
static inline __attribute__ ((const)) float float_mean3(float a, float b, float c)
{
  return (a + b + c) / 3.0f;
}

/*!
 * Calculate the \em Average of \p a and \p b.
 */
static inline __attribute__ ((const)) double double_mean3(double a, double b, double c)
{
  return (a + b + c) / 3.0;
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
