/*! \file cmp.h
 * \brief Comparisons.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-05-02 13:03
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "pnw_types.h"
#include "cc_features.h"
#include "inttypes.h"

/*!
 * Compare \p a with \p b.
 * \return the sign of (\p a - \p b).
 */
static inline spure char char_cmp(char a, char b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure short short_cmp(short a, short b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure int int_cmp(int a, int b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure uint uint_cmp(uint a, uint b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure uint16_t uint16_cmp(uint16_t a, uint16_t b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure uint64_t uint64_cmp(uint64_t a, uint64_t b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure int float_cmp(float a, float b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure int double_cmp(double a, double b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure int ldouble_cmp(long double a, long double b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure int size_cmp(size_t a, size_t b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}
static inline spure int ptr_cmp(const void * const a, const void * const b)
{
  if (a < b) { return -1; } else if (a == b) { return 0; } else { return 1; }
}

static inline pure char char_ptr_lt(const char * a, const char * b) { return (*a < *b); }
static inline pure short short_ptr_lt(const short * a, const short * b) { return (*a < *b); }
static inline pure int int_ptr_lt(const int * a, const int * b) { return (*a < *b); }
static inline pure uint uint_ptr_lt(const uint * a, const uint * b) { return (*a < *b); }
static inline pure int float_ptr_lt(const float * a, const float * b) { return (*a < *b); }
static inline pure int double_ptr_lt(const double * a, const double * b) { return (*a < *b); }
static inline pure int ldouble_ptr_lt(const long double * a, const long double * b) { return (*a < *b); }
static inline pure int size_ptr_lt(const size_t * a, const size_t * b) { return (*a < *b); }

#ifdef __cplusplus
}
#endif
