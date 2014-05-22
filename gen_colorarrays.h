/*!
 * \file gen_colorarrays.h
 * \brief Generate Arrays of Colors.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#include "color.hpp"
#include "vec2f.h"
#include "color_constants.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Flag for color interpolation */
extern bool g_color_linterpol_flag;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Generate OpenGL Color Splat Gradient from Inner Color \p colorI to Outer
 * Color \p pcolorO.
 */
void
cGLarray_genSplat(uint32_t * cGL,
                  color_t col,
                  size_t glcolors_stride, size_t glcolorsN);

/*!
 * Generate OpenGL Color Splat Gradient from Inner Color \p colorI to Outer
 * Color \p colorO.
 */
void
cGLarray_genSplatIO(uint32_t * cGL,
                    color_t colorI, color_t colorO,
                    size_t glcolorsN);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Generate Linear-Interpolated (linterpol) Circular OpenGL Color Gradient at
 * \p cGL.
 */
void
cGLarray_genCirGrad(const color_t * colorsG, size_t gradN,
                    uint32_t * cGL, size_t glcolors_stride,
                    size_t glcolorsN);

/*!
 * Generate Linear-Interpolated (linterpol) Circular Inner-Outer
 * Interleaved OpenGL Color Gradient at \p cGL.
 */
void
cGLarray_genCirGradIO(color_t colorI, color_t colorO,
                      const color_t * colorsGI, size_t numGI,
                      const color_t * colorsGO, size_t numGO,
                      uint32_t * cGL, size_t glcolors_stride,
                      size_t glcolorsN);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Convert a Color Array at \p colors of length \p glcolorsN to OpenGL Colors at \p cGL.
 */
void
colorarrayGL_convert_colorarray(uint32_t * cGL,
                                const color_t * colors, size_t glcolorsN);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Generate a Vertical Color Gradient at \p colorsG of
 * length \p vtxN from \p colorS to \p colorD using the Y-coordinates
 * of \p vtx of length \p vtxN.
 */
static inline
void colorarray_gen_GRADV_from_vec2farray(color_t * colorsG,
                                          color_t colorS, color_t colorD,
                                          const vec2f * vtx, size_t vtxN)
{
  /* \TODO: COSTLY FUNCTION!!! */
  float ymin, ymax, yspan; vec2farray_rdSpanY(vtx, vtxN, &ymin, &ymax, &yspan);
  for (size_t i = 0; i < vtxN; i++) {
    colorsG[i] = linear_interpolate(colorS, (vtx[i].y - ymin) / yspan, colorD);
  }
}

/*!
 * Generate a Horizontal Color Gradient at \p colorsG of
 * length \p vtxN from \p colorS to \p colorD using the X-coordinates
 * of \p vtx of length \p vtxN.
 */
static inline
void colorarray_gen_GRADH_from_vec2farray(color_t * colorsG,
                                          color_t colorS, color_t colorD,
                                          const vec2f * vtx, size_t vtxN)
{
  /* \TODO: COSTLY FUNCTION!!! */
  float xmin, xmax, xspan; vec2farray_rdSpanX(vtx, vtxN, &xmin, &xmax, &xspan);
  for (size_t i = 0; i < vtxN; i++) {
    colorsG[i] = linear_interpolate(colorS, (vtx[i].x - xmin) / xspan, colorD);
  }
}

/*!
 * Generate a Radial Color Gradient at \p colorsG of
 * length \p vtxN from \p colorS to \p colorD using the R-coordinates
 * of \p vtx of length \p vtxN.
 */
static inline
void colorarray_gen_GRADR_from_vec2farray(color_t * colorsG,
                                          color_t colorS, color_t colorD,
                                          const vec2f * cen,
                                          const vec2f * vtx, size_t vtxN)

{
  /* \TODO: COSTLY FUNCTION!!! */
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
