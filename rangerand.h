/*! \file rangerand.h
 * \brief Generate Random Numbers within a range.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-06-05 10:29
 */

#pragma once

#include "rand.h"
#include "cc_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Produce Random inside the inclusive range [\p min, \p max] . */
static inline wuur char char_rangerand(char min, char max)
{
  return (float)min + (((float)max - min) * rand() / (RAND_MAX + 1.0));
}
/*! Produce Random inside the inclusive range [\p min, \p max] . */
static inline wuur short short_rangerand(short min, short max)
{
  return (float)min + (((float)max - min) * rand() / (RAND_MAX + 1.0));
}
/*! Produce Random inside the inclusive range [\p min, \p max] . */
static inline wuur float float_rangerand(float min, float max)
{
  return min + ((max - min) * rand() / (RAND_MAX + 1.0));
}
static inline wuur double double_rangerand(double min, double max)
{
  return min + ((max - min) * rand() / (RAND_MAX + 1.0));
}
static inline wuur size_t size_rangerand(size_t min, size_t max)
{
  return (double)min + (((double)max - min) * rand() / (RAND_MAX + 1.0));
}

/*! Random Vector */
static inline void uint8array_rangerand(uint8_t * a, size_t n, uint8_t min, uint8_t max)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = min + ((double)(max - min) * rand() / (RAND_MAX + 1.0));
  }
}
static inline void
uint16array_rangerand(ushort *a, size_t n, ushort min, ushort max)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = min + ((double)(max - min) * rand() / (RAND_MAX + 1.0));
  }
}
static inline void
intarray_rangerand(int *a, size_t n, int min, int max)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = min + ((double)(max - min) * rand() / (RAND_MAX + 1.0));
  }
}
static inline void
farray_rangerand(float *a, size_t n, float min, float max)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = min + ((max - min) * rand() / (RAND_MAX + 1.0));
  }
}
static inline void
darray_rangerand(double *a, size_t n, double min, double max)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = min + ((max - min) * rand() / (RAND_MAX + 1.0));
  }
}
static inline void
ldarray_rangerand(ldouble *a, size_t n, ldouble min, ldouble max)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = min + ((max - min) * rand() / (RAND_MAX + 1.0));
  }
}

/*! Random Matrix */
static inline void
fmatrix_rangerand(float **a, size_t m, size_t n, float min, float max)
{
  for (size_t i = 0; i < m; i++) {
    farray_rangerand(a[i], n, min, max);
  }
}
static inline void
dmatrix_rangerand(double **a, size_t m, size_t n, double min, double max)
{
  for (size_t i = 0; i < m; i++) {
    darray_rangerand(a[i], n, min, max);
  }
}

#ifdef __cplusplus
}
#endif
