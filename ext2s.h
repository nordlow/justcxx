/*!
 * \file ext2s.h
 * \brief Two-dimensional Extents of ushorts.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Two-dimensional Vector of shorts.
 */
typedef struct
{
  ushort w;		/**< w Coordinate. */
  ushort h;		/**< h Coordinate. */
} ext2s;

/*!
 * Set \p a to the vector [w, h].
 */
static inline void
ext2s_set(ext2s * a, ushort w, ushort h)
{
  a->w = w;
  a->h = h;
}

/*!
 * Indexing of vector \p a at \p i.
 *
 * \return element at index \p i.
 */
static inline ushort
ext2s_at(const ext2s * a, int i)
{
  return (&a->w)[i];
}

static inline void
ext2s_out_str(FILE * stream, const ext2s * a)
{
  fprintf(stream, "[%5d,%5d]", a->w, a->h);
}

static inline void
ext2sarray_fprint(FILE * stream, const ext2s * in, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ext2s_out_str(stream, &in[i]);
    if (i < n - 1) {
      putc(' ', stream);
    } else {
      putc('\n', stream);
    }
  }
}

#ifdef __cplusplus
}
#endif
