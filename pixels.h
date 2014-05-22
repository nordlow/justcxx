/*! \file pixels.hpp
 * \brief Tools for Conversions from color_t to other Color/Pixel Formats.
 * \author Copyright(C) 2006 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "geometry/color.hpp"
#include "geometry/color_constants.hpp"
#include "stdio_x.h"

/*!
 * Pixel Format.
 *
 * The naming convention describes what the blocks of bits and bytes at
 * specific positions and of specific sizes are used for in starting
 * left-to-right with the most signficant bits and bytes.
 *
 * \NOTE That this format does not specify memory byte order for different
 * architectures but only the bit position ranges of the color components in
 * the architecture for which this code is compiled.
 *
 * Example: \c PXF_RGBU32 uses
 * - bit 31 downto 24 as an 8-bit Red-value,
 * - bit 23 downto 16 as an 8-bit Green-value,
 * - bit 15 downto  8 as an 8-bit Blue-value and
 * - bit  7 downto  0 is unused.
 */
typedef enum
{
  PXF_RGB555,		     /**< XFree86: x86 15-bit depth. Tested */
  PXF_RGB565,		     /**< XFree86: x86 16-bit depth. Tested */

  PXF_RGB24,	    /**< Memory Layout: R0, G0, B0, R1, G1, B2, ... */

  PXF_RGB32,
  PXF_BGR32,

  PXF_RGBA32,
  PXF_BGRA32,

  PXF_G8,			/**< Gray 8-bit */
  PXF_UNKNOWN
} PXF_t;

/*! Pixel Conversion Structure. */
typedef struct
{
  PXF_t fmt:7;			/**< The pixel format. */

  /***
   * Flag that a swap of byte order is needed before writing to image.
   *
   * If set (1) we should swap, otherwise (0) we should not.
   */
  uint byteswap:1;
} PConv;

/* ---------------------------- Group Separator ---------------------------- */

const char *PXF_getName(PXF_t a);

/* ---------------------------- Group Separator ---------------------------- */

/*! Return the color \p a in pixel format URGB32. */
inline pure
uint32_t
color_to_URGB32(color_t a)
{
  return (((uint32_t)color_getR(a) <<  0) |
	  ((uint32_t)color_getG(a) <<  8) |
	  ((uint32_t)color_getB(a) << 16));
}

/*! Return the color \p a in pixel format URGB32. */
inline pure
uint32_t
color_to_UBGR32(color_t a)
{
  return (((uint32_t)color_getR(a) << 16) |
	  ((uint32_t)color_getG(a) <<  8) |
	  ((uint32_t)color_getB(a) <<  0));
}

/*! Return the color \p a in pixel format RGB555. */
inline pure
uint16_t
color_to_RGB555(color_t a)
{
  PTEST("\n");
  return (((uint16_t)(color_getR(a) >> 3) <<  0) |
	  ((uint16_t)(color_getG(a) >> 3) <<  5) |
	  ((uint16_t)(color_getB(a) >> 3) << 10));
}

/*! Return the color \p a in pixel format RGB565. */
inline pure
uint16_t
color_truncate_to_RGB565(color_t a)
{
  PTEST("\n");
  return (((uint16_t)(color_getR(a) >> 3) <<  0) |
	  ((uint16_t)(color_getG(a) >> 2) <<  5) |
	  ((uint16_t)(color_getB(a) >> 3) << 11));
}

/*! Set (possibly convert) \p color to the given pixel type \p pconv and return it.
 *
 * \param[in] pconv is the pixel conversion that should be performed.
 */
inline pure
uint32_t
pix_get(PConv pconv, color_t color)
{
  uint32_t p = 0;
  switch (pconv.fmt) {
  case PXF_RGB555: p = color_to_RGB555(color); break;
  case PXF_RGB565: p = color_truncate_to_RGB565(color); break;
  case PXF_RGB32:  p = color_to_URGB32(color); break;
  case PXF_BGR32:  p = color_to_UBGR32(color); break;
  default:
    PWARN("Could not handle format %s\n", PXF_getName(pconv.fmt));
    p = 0;			/* default zero (most often black) */
    break;
  }
  return p;
}

/*! Lookup the size (in whole bytes) of the pixel format \p pxf.
 * \return the looked update bytesize, or -1 on error.
 */
inline pure
int
PXF_getByteSize(PXF_t pxf)
{
  int ret = -1;
  switch (pxf) {
  case PXF_RGB555: ret = 2; break;
  case PXF_RGB565: ret = 2; break;
  case PXF_RGB24: ret = 3; break;
  case PXF_RGB32: ret = 4; break;
  case PXF_BGR32: ret = 4; break;
  case PXF_RGBA32: ret = 4; break;
  case PXF_BGRA32: ret = 4; break;
  case PXF_G8: ret = 1; break;
  default: PWARN("Could not handle format %s\n", PXF_getName(pxf)); break;
  }

  return ret;
}

void color_pixel_test(void);
