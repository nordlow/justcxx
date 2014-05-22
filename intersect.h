/*!
 * \file intersect.h
 * \brief Calculations of intersections between lines and between boxes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "vec2d.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Calculate the intersection indexes i between two lines.
 *
 * The 1:st and 2:nd line are defined by
 * - a + c * i(0)
 * - b + d * i(1)
 * respectively.
 *
 * Inputs:
 * - a, b: The start POSITIONS of the lines parameterizations.
 * - c, d: The DIRECTIONS of the lines parameterizations.
 *
 * Returns:
 * - The parameter values of the lines at the intersection point.
 */
static inline vec2d
vec2d_line_intersect(vec2d a, vec2d b, vec2d c, vec2d d)
{
  vec2d i;

  double e = b.x - a.x;
  double f = b.y - a.y;

  double det = c.y * d.x - c.x * d.y;

  i.x = (d.x * f - d.y * e) / det;
  i.y = (c.x * f - c.y * e) / det;

  return i;
}

/*!
 * Calculate the intersection point using the value intersection parameter i
 * returned from vec2d_line_intersect().
 *
 * The line is defined by
 * - a + c * i
 *
 * Inputs:
 * - a: The start POSITION of the line parameterization.
 * - c: The DIRECTION of the line parameterizations.
 *
 */
static inline vec2d
vec2d_line_intersection_point(vec2d a, vec2d c, double i)
{
  vec2d p;

  p.x = a.x + c.x * i;
  p.y = a.y + c.y * i;

  return p;
}

#ifdef __cplusplus
}
#endif
