/*!
 * \file xwinlogo.h
 * \brief Generate an X Window System Logo.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "vec2f.h"
#include "line2f.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Draw the "official" X Window System Logo, designed by Danny Chong
 *
 * Written by Ollie Jones, Apollo Computer
 *
 * Does some fancy stuff to make the logo look acceptable even
 * if it is tiny.  Also makes the various linear elements of
 * the logo line up as well as possible considering rasterization.
 *
 * Munged to draw anti-aliased logo using Render extension.
 * Carl Worth, 2002-05-16
 */
void
gen_xlogo(float x, float y, float width, float height,
	  vec2f poly_left[6], vec2f poly_right[6]);

#ifdef __cplusplus
}
#endif
