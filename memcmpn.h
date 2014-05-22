/*!
 * \file memcmpn.h
 * \brief Specialized Versions of \c memcmp() to be optimized through
 *       (GCC autovectorization.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-01-10 10:53
 *
 * Currently declared \em inline because only then can the code
 * generation be sensitive to the \em alignment of the arguments in
 * calls to these functions.
 */

/* ========================================================================= */

#pragma once

#include "pnw_types.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Compare the Memory Area \p a with \p b both of length 20 byte by
 * byte.
 * \return -1 if \p a is less than \p b, +1 if \p a is greater than
 *         \p b, 0 otherwise.
 */
static inline int
memcmp20(const void * a, const void * b)
{
  const char *ac = (char*)a, *bc = (char*)b;
  for (uint i = 0; i < 20; i++) {
    if      (ac[i] < bc[i]) { return -1; }
    else if (ac[i] > bc[i]) { return +1; }
  }
  return 0;
}

/*!
 * Compare the Memory Area \p a with \p b both of length 64 byte by
 * byte.
 * \return -1 if \p a is less than \p b, +1 if \p a is greater than
 *         \p b, 0 otherwise.
 */
static inline int
memcmp64(const void * a, const void * b)
{
  const char *ac = (char*)a, *bc = (char*)b;
  for (uint i = 0; i < 64; i++) {
    if      (ac[i] < bc[i]) { return -1; }
    else if (ac[i] > bc[i]) { return +1; }
  }
  return 0;
}

/*!
 * Compare the Memory Area \p a with \p b both of length 160 byte by
 * byte.
 * \return -1 if \p a is less than \p b, +1 if \p a is greater than
 *         \p b, 0 otherwise.
 */
static inline int
memcmp160(const void * a, const void * b)
{
  const char *ac = (char*)a, *bc = (char*)b;
  for (uint i = 0; i < 160; i++) {
    if      (ac[i] < bc[i]) { return -1; }
    else if (ac[i] > bc[i]) { return +1; }
  }
  return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Compare the Memory Area \p a with \p b both of length \p n byte by
 * byte.
 * \return -1 if \p a is less than \p b, +1 if \p a is greater than
 *         \p b, 0 otherwise.
 */
static inline int
memcmpn(const void * a, const void * b, size_t size)
{
  int ret = 0;
  switch (size) {
  case 20: ret = memcmp20(a, b); break;
  case 64: ret = memcmp64(a, b); break;
  case 160: ret = memcmp160(a, b); break;
  default: ret = memcmp(a, b, size); break;
  }
  return ret;
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
