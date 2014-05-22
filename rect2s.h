/*!
 * \file rect2s.h
 * \brief Two-dimensional Rectangle of shorts.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "vec2s.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Two-dimensional rectangle of shorts and unsigned shorts.
 */
typedef struct
{
  short x, y;			/**< Lower Corner. */
  ushort w, h;			/**< Dimensions in width and height. */
} rect2s;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Set \p a to the box [x, y, x+w, y+h]
 */
static inline void
rect2s_set(rect2s * a, short x, short y, ushort w, ushort h)
{
  a->x = x;
  a->y = y;
  a->w = w;
  a->h = h;
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
