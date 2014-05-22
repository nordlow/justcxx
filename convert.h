#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

void uint8array_convert_darray(uchar *a, const double *b, size_t l);
void uint16array_convert_darray(uint16_t * a, const double *b, size_t l);
void uint32array_convert_darray(uint32_t * a, const double *b, size_t l);
void uint64array_convert_darray(uint64_t * a, const double *b, size_t l);
void sint8array_convert_darray(int8_t * a, const double *b, size_t l);
void sint16array_convert_darray(int16_t * a, const double *b, size_t l);
void sint32array_convert_darray(int32_t * a, const double *b, size_t l);
void sint64array_convert_darray(int64_t * a, const double *b, size_t l);

/*!
 * \name ASCII - EBCDIC conversions
 */
extern uint8_t uint8array256_ascii_convert_ebcdic[256];
extern uint8_t uint8array256_ebcdic_convert_ascii[256];
pure uint8_t uint8_ascii_convert_ebcdic(uint8_t c);
pure uint8_t uint8_ebcdic_convert_ascii(uint8_t c);

/*!
 * \name Array - scalar conversions.
 */
void intarray_convert_int(int *out, int in, int n);
void u8v_convert_u8(uint8_t * out, uint8_t in, int n);

/* Array - Array */
void f32v_convert_u8a(f32_t * out, const uint8_t * in, int n);
void u8v_convert_f32a(uint8_t * out, const f32_t * in, int n);
void u16v_convert_u8(uint16_t * out, const uint8_t * in, int n);

/*! \name Color space conversions.
 * Search Google for "color space conversions":
 * \see http://wwwzenger.informatik.tu-muenchen.de/lehre/vorlesungen/graphik/info/csc/COL_.htm
 */
/* @{ */

/*!
 * Color Space.
 */
typedef enum
{
  /* 8-bit */
  GRAY8, RGB332,
  /* 16-bit */
  RGB555, RGB565,
  /* 24-bit */
  BGR24, RGB24,
  BGR24P, RGB24P,
  /* 32-bit */
  BGR32, RGB32,
  BGRA32, RGBA32,
  BGRA32P, RGBA32P,
  YVU410, YVU420, YUYV, UYVY,
  YVU422, YVU411, YUV410, YUV420, YYUV, HI240, WNVA
} COLORSPACE_t;

/*! Convert 8-bit Gray array to 16-bit RGB555 array.
 * 3DNow! version tested on AMD K6-2.
 */
void u16v_rgb555_convert_u8v_gray(uint16_t * out, const uint8_t * in, int n);

/*! Convert 8-bit Gray array to 16-bit RGB565 array.
 * 3DNow! version tested on AMD K6-2.
 */
void u16v_rgb565_convert_u8v_gray(uint16_t * out, const uint8_t * in, int n);

/*! Convert 8-bit Gray array to 24-bit BGR24 array. */
void u8v_bgr24_convert_u8v_gray(uint8_t * out, const uint8_t * in, int n_in);

/*! Convert 8-bit Gray array to 24-bit RGB24 array. */
void u8v_rgb24_convert_u8v_gray(uint8_t * out, const uint8_t * in, int n_in);

/*! Convert 8-bit Gray array to 32-bit BGR32 array.
 * 3DNow! version tested on AMD K6-2.
 */
void u32v_bgr32_convert_u8v_gray(uint32_t * out, const uint8_t * in, int n);

/*! Convert 8-bit Gray array to 32-bit BGR32 array. */
void u32v_rgb32_convert_u8v_gray(uint32_t * out, const uint8_t * in, int n);

/*! Convert 8-bit Gray array to 32-bit RGBA32 array. */
void u8v_gray_convert_u32v_rgba(uint8_t * out, const uint32_t * in, int n);

/*! Convert 24-bit RGB array to 32-bit RGB32 array. */
void rgb32_convert_rgb24(ulong *out, const uchar *in, int n);

/*! Convert 24-bit RGB array to 32-bit BGR32 array. */
void bgr32_convert_rgb24(ulong *out, const uchar *in, int n);

/* @} */

#ifdef __cplusplus
}
#endif
