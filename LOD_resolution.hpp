/*! \file LOD_resolution.hpp
 * \brief Level-Of-Detail (LOD) Resolution Calculations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date
 */

#pragma once
#include "tesstab.hpp"
#include "geometry/vec.hpp"
#include "binlog.h"

/*! Circle Quadrant LOD-resolution in bits.
 * \param[in] curve gives the circle curvature radius (along both x and y).
 */
pure uint calcCirLODres(const vec2f& mapfW2S,
                                         const float curve,
                                         bool LOD_flag);

/*! Ellipse Quadrant LOD-resolution in bits.
 * \param[in] curve gives the ellipse curvature radius (along x and y).
 */
pure uint calcEllLODres(const vec2f& mapfW2S,
                                         const vec2f& curve,
                                         bool LOD_flag);

/*! Ellipse Quadrant LOD-resolution in bits.
 * \param[in] curveI Inner Ellipse Curvature radius along x and y.
 * \param[in] curveO Outer Ellipse Curvature radius along x and y.
 */
pure uint calcEllLODres2(const vec2f& mapfW2S,
                                          const vec2f& curveI,
                                          const vec2f& curveO,
                                          bool LOD_flag);

/*! Get Maximum Ellipse Quadrant LOD-resolution in bits. */
uint
vec2f_getMaxLODres(void);
