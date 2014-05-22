/*!
 * \file clip.h
 * \brief Line clipping.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "vec2f.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Liang-Barsky line clip in 2D.
 *
 * if TRUE is returned (rp1, rp2) defines the line (p1, p2) clipped to the
 * window defined by its corners (min, max)
 */
int fvec2_liang_barsky_clip_2d(vec2f min, vec2f max,
			       vec2f p1, vec2f p2,
			       vec2f * rp1, vec2f * rp2);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
