/*!
 * \file palette.h
 * \brief Color Palette.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "palette_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Initialize Palette \p cpal.
 */
static inline void
cpal_init(cpal_t * cpal)
{
  cpal->cA = 0;
  cpal->cN = 0;
}

/*!
 * Initialize Palette \p cpal.
 */
static inline void
cpal_initn(cpal_t * cpal, size_t elmsN)
{
  cpal->cA = (color_t*)malloc((elmsN + 1) * sizeof(color_t));
  cpal->cN = elmsN;
}

/*!
 * Allocate and Initialize Palette and return it.
 */
static inline cpal_t *
cpal_new(void)
{
  cpal_t * cpal = (cpal_t*)malloc(sizeof(cpal_t));
  cpal_init(cpal);
  return cpal;
}

/*!
 * Allocate and Initialize Palette containing \p elmsN number of
 * undefined colors and return it.
 */
static inline cpal_t *
cpal_newn(size_t elmsN)
{
  cpal_t * cpal = (cpal_t*)malloc(sizeof(cpal_t));
  cpal_initn(cpal, elmsN);
  return cpal;
}

/*!
 * Append color to palette cpal, without checking for duplicate appends.
 */
static inline void
cpal_app(cpal_t * cpal, color_t color)
{
  cpal->cA =
    (color_t*)realloc(cpal->cA,
		      (cpal->cN + 1) * sizeof(color_t));
  cpal->cA[cpal->cN] = color;
  cpal->cN += 1;
}

/*!
 * Append several objects colors of length \p colors_num to palette @p
 * cpal, without checking for duplicate appends.
 */
static inline void
cpal_appM(cpal_t * cpal, const color_t * colors, unsigned int colors_num)
{
  cpal->cA =
    (color_t*)realloc(cpal->cA,
		      (cpal->cN + colors_num) * sizeof(color_t));
  memcpy(&cpal->cA[cpal->cN], colors, colors_num * sizeof(color_t));
  cpal->cN += colors_num;
}

/*!
 * Init a Color Palette from a color array at \p colors of length @p
 * colors_num.
 */
static inline cpal_t *
cpal_new_from_colorarray(const color_t * colors, unsigned int colors_num)
{
  cpal_t * cpal = cpal_new();
  cpal_appM(cpal, colors, colors_num);
  return cpal;
}

/*!
 * Init a Color Palette from the three colors \p a,
 * \p b and \p c.
 */
static inline cpal_t *
cpal_new_from_3colors(const color_t a, color_t b, color_t c)
{
  cpal_t * cpal = cpal_newn(3);
  cpal->cA[0] = a;
  cpal->cA[1] = b;
  cpal->cA[2] = c;
  return cpal;
}

/*!
 * Clone the Palette \p cpal.
 */
static inline cpal_t *
cpal_clone(const cpal_t * cpal)
{
  return cpal_new_from_colorarray(cpal->cA, cpal->cN);
}

/*!
 * Clear Contents of the Palette \p cpal.
 */
static inline void
cpal_clear(cpal_t * cpal)
{
  if (cpal->cA) { free(cpal->cA); cpal->cA = 0; }
  cpal->cN = 0;
}

/*!
 * Delete the palette \p cpal.
 */
static inline void
cpal_delete(cpal_t * cpal)
{
  cpal_clear(cpal);
  free(cpal);
}

/*!
 * Print palette \p cpal in the format [c0, c1, ..., cN-1]
 */
static inline void
cpal_fprint(FILE * stream, const cpal_t * cpal)
{
  putc('[', stream);
  for (size_t i = 0; i < cpal->cN; i++)  {
    if (i >= 1) { putc(',', stream); }
    color_fprint(stream, cpal->cA[i]);
  }
  putc(']', stream);
}

/*!
 * Encode the Palette \p cpal into \p chv.
 */
static inline int
chv_enc_cpal(chv_t * chv, const cpal_t * cpal)
{
  return chv_enc_colorarray(chv, cpal->cA, cpal->cN);
}

/*!
 * Decode a Palette into \p cpal from \p chv.
 */
static inline int
chv_dec_cpal(chv_t * chv, cpal_t * cpal)
{
  cpal_clear(cpal);
  int cnt = 0, ret = 0;

  uint64_t elmsN_u64 = 0;
  if ((ret = chv_dec_u64be(chv, &elmsN_u64)) >= 0) cnt += ret; else return ret;
  const size_t elmsN = elmsN_u64;

  cpal_initn(cpal, elmsN);

  for (size_t i = 0; i < elmsN; i++) {
    if ((ret = chv_dec_u32be(chv, &cpal->cA[i])) >= 0) cnt += ret; else return ret;
  }
  return cnt;

}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
