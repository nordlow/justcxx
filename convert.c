#include "convert.h"
#include <string.h>

void
uint8array_convert_darray(uchar *a, const double *b, size_t l)
{
  size_t j;
  for (j = 0; j < l; j++)
    a[j] = b[j];
}

void
uint16array_convert_darray(uint16_t * a, const double *b, size_t l)
{
  size_t j;
  for (j = 0; j < l; j++)
    a[j] = b[j];
}

void
uint32array_convert_darray(uint32_t * a, const double *b, size_t l)
{
  size_t j;
  for (j = 0; j < l; j++)
    a[j] = b[j];
}

void
uint64array_convert_darray(uint64_t * a, const double *b, size_t l)
{
  size_t j;
  for (j = 0; j < l; j++)
    a[j] = b[j];
}

void
sint8array_convert_darray(int8_t * a, const double *b, size_t l)
{
  size_t j;
  for (j = 0; j < l; j++)
    a[j] = b[j];
}

void
sint16array_convert_darray(int16_t * a, const double *b, size_t l)
{
  size_t j;
  for (j = 0; j < l; j++)
    a[j] = b[j];
}

void
sint32array_convert_darray(int32_t * a, const double *b, size_t l)
{
  size_t j;
  for (j = 0; j < l; j++)
    a[j] = b[j];
}

void
sint64array_convert_darray(int64_t * a, const double *b, size_t l)
{
  size_t j;
  for (j = 0; j < l; j++)
    a[j] = b[j];
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name ASCII - EBCDIC conversion tables
 */

/* @{ */

uint8_t uint8array256_ascii_convert_ebcdic[256] = {
  0, 1, 2, 3, 156, 9, 134, 127, 151, 141, 142, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 157, 133, 8, 135, 24, 25, 146, 143, 28, 29, 30, 31,
  128, 129, 130, 131, 132, 10, 23, 27, 136, 137, 138, 139, 140, 5, 6, 7,
  144, 145, 22, 147, 148, 149, 150, 4, 152, 153, 154, 155, 20, 21, 158, 26,
  32, 160, 161, 162, 163, 164, 165, 166, 167, 168, 91, 46, 60, 40, 43, 33,
  38, 169, 170, 171, 172, 173, 174, 175, 176, 177, 93, 36, 42, 41, 59, 94,
  45, 47, 178, 179, 180, 181, 182, 183, 184, 185, 124, 44, 37, 95, 62, 63,
  186, 187, 188, 189, 190, 191, 192, 193, 194, 96, 58, 35, 64, 39, 61, 34,
  195, 97, 98, 99, 100, 101, 102, 103, 104, 105, 196, 197, 198, 199, 200, 201,
  202, 106, 107, 108, 109, 110, 111, 112, 113, 114, 203, 204, 205, 206, 207,
    208,
  209, 126, 115, 116, 117, 118, 119, 120, 121, 122, 210, 211, 212, 213, 214,
    215,
  216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
    231,
  123, 65, 66, 67, 68, 69, 70, 71, 72, 73, 232, 233, 234, 235, 236, 237,
  125, 74, 75, 76, 77, 78, 79, 80, 81, 82, 238, 239, 240, 241, 242, 243,
  92, 159, 83, 84, 85, 86, 87, 88, 89, 90, 244, 245, 246, 247, 248, 249,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 250, 251, 252, 253, 254, 255
};

uint8_t uint8array256_ebcdic_convert_ascii[256] = {
  0, 1, 2, 3, 55, 45, 46, 47, 22, 5, 37, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31,
  64, 79, 127, 123, 91, 108, 80, 125, 77, 93, 92, 78, 107, 96, 75, 97,
  240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 122, 94, 76, 126, 110,
    111,
  124, 193, 194, 195, 196, 197, 198, 199, 200, 201, 209, 210, 211, 212, 213,
    214,
  215, 216, 217, 226, 227, 228, 229, 230, 231, 232, 233, 74, 224, 90, 95, 109,
  121, 129, 130, 131, 132, 133, 134, 135, 136, 137, 145, 146, 147, 148, 149,
    150,
  151, 152, 153, 162, 163, 164, 165, 166, 167, 168, 169, 192, 106, 208, 161,
    7,
  32, 33, 34, 35, 36, 21, 6, 23, 40, 41, 42, 43, 44, 9, 10, 27,
  48, 49, 26, 51, 52, 53, 54, 8, 56, 57, 58, 59, 4, 20, 62, 225,
  65, 66, 67, 68, 69, 70, 71, 72, 73, 81, 82, 83, 84, 85, 86, 87,
  88, 89, 98, 99, 100, 101, 102, 103, 104, 105, 112, 113, 114, 115, 116, 117,
  118, 119, 120, 128, 138, 139, 140, 141, 142, 143, 144, 154, 155, 156, 157,
    158,
  159, 160, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182,
    183,
  184, 185, 186, 187, 188, 189, 190, 191, 202, 203, 204, 205, 206, 207, 218,
    219,
  220, 221, 222, 223, 234, 235, 236, 237, 238, 239, 250, 251, 252, 253, 254,
    255
};

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Array - array conversions.
 */

void intarray_convert_int(int *a, int b, int l)
{
  for (int j = 0; j < l; j++)
    a[j] = b;
}

void
u16v_convert_u8(uint16_t * out, const uint8_t * in, int n)
{
  int i;
  int d, q, r, lastx;

#ifdef HAVE_CPU_MMX
  d = 8;			/* divisor (element blocklength) */
#else
  d = 2;			/* divisor (element blocklength) */
#endif

  if (n < d) {
    for (i = 0; i < n; i++) {out[i] = in[i]; }
  }

  q = n / d;
  r = n % d;
  lastx = (q - 1) * d + r;

#ifdef HAVE_CPU_MMX
  for (i = 0; i < q * d; i += d)
    __asm__ volatile
      ("movq (%1),  %%mm0\n\t"
       "movq %%mm0, %%mm1\n\t"
       "pxor %%mm2, %%mm2\n\t"
       "punpcklbw %%mm2, %%mm0\n\t"
       "punpckhbw %%mm2, %%mm1\n\t"
       "movq %%mm0,  (%0)\n\t"
       "movq %%mm1, 8(%0)\n\t"
       :
       : "r" (out + i), "r"(in + i));
  MMX_EMMS();

  if (r > 0)
    __asm__ volatile
      ("movq (%1),  %%mm0\n\t"
       "movq %%mm0, %%mm1\n\t"
       "pxor %%mm2, %%mm2\n\t"
       "punpcklbw %%mm2, %%mm0\n\t"
       "punpckhbw %%mm2, %%mm1\n\t"
       "movq %%mm0,  (%0)\n\t"
       "movq %%mm1, 8(%0)\n\t"
       :
       : "r" (out + lastx), "r"(in + lastx));

  MMX_EMMS();
#else
  for (i = 0; i < q; i++)
    (((uint32_t *) out)[i] =
     (((uint32_t) in[i + 0]) << 0) + (((uint32_t) in[i + 1]) << 16));
  if (r > 0)
    out[lastx] = in[lastx];
#endif
}

/* ---------------------------- Group Separator ---------------------------- */

void
u16v_rgb555_convert_u8v_gray(uint16_t * out, const uint8_t * in, int n)
{
  int i;
  uint16_t uint16_tmp;
#ifdef HAVE_CPU_MMX
  int q, r, d;
  d = 4;			/* divisor (element blocklength) */
  q = n / d;
  r = n % d;
  for (i = 0; i < q * d; i += d)
    __asm__ volatile
      ("movd      (%1),  %%mm6\n\t"	/* Load 4 GRAY's in mm6 */
       "pxor      %%mm2, %%mm2\n\t"	/* sum=0 */
       "pxor      %%mm1, %%mm1\n\t"	/* Need a zero */
       "punpcklbw %%mm6, %%mm1\n\t"	/* We unpack 4 LSB */
       "psrlw       $11, %%mm1\n\t"	/* Moveto Blue's pos */
       "paddw     %%mm1, %%mm2\n\t"	/* and add her */
       "psllw        $5, %%mm1\n\t"	/* Moveto Green's pos */
       "paddw     %%mm1, %%mm2\n\t"	/* and add her */
       "psllw        $5, %%mm1\n\t"	/* Move to Red's pos */
       "paddw     %%mm1, %%mm2\n\t"	/* and add her */
       "movq      %%mm2,  (%0)\n\t"	/* Save 4 RGB565's. */
       :
       : "r" (out + i), "r"(in + i));
  MMX_EMMS();
  for (; i < q * d + r; i++) {
    uint16_tmp = in[i] >> 3;	/* 5 bits for R, G and B. */
    out[i] = uint16_tmp | (uint16_tmp << 5) | (uint16_tmp << 10);
  }
#else
  for (i = 0; i < n; i++) {
    uint16_tmp = in[i] >> 3;	/* 5 bits for R, G and B. */
    out[i] = uint16_tmp | (uint16_tmp << 5) | (uint16_tmp << 10);
  }
#endif
}

void
u16v_rgb565_convert_u8v_gray(uint16_t * out, const uint8_t * in, int n)
{
  int i;
  uint16_t uint16_tmp, uint16_ttmp;
#ifdef HAVE_CPU_MMX
  int q, r, d;
  d = 4;			/* divisor (element blocklength) */
  q = n / d;
  r = n % d;
  for (i = 0; i < q * d; i += d)
    __asm__ volatile
      ("movd      (%1),  %%mm6\n\t"	/* Load 4 GRAY's in mm7 */
       "pxor      %%mm2, %%mm2\n\t"	/* sum=0 */
       "pxor      %%mm1, %%mm1\n\t"	/* Need a zero */
       "punpcklbw %%mm6, %%mm1\n\t"	/* We unpack 4 LSB */
       "psrlw       $11, %%mm1\n\t"	/* Move to Blue's pos */
       "paddw     %%mm1, %%mm2\n\t"	/* and add her */
       "psllw        $6, %%mm1\n\t"	/* Move to Green's pos */
       "paddw     %%mm1, %%mm2\n\t"	/* and add her */
       "psllw        $5, %%mm1\n\t"	/* Move to Red's pos */
       "paddw     %%mm1, %%mm2\n\t"	/* and add her */
       "movq      %%mm2,  (%0)\n\t"	/* Save 4 RGB565's. */
       ::"r" (out + i), "r"(in + i));
  MMX_EMMS();
  for (; i < q * d + r; i++) {
    uint16_ttmp = in[i] >> 2;	/* 6 bits for Green */
    uint16_tmp = uint16_ttmp >> 1;	/* and 5 bits for Red and Blue. */
    out[i] = uint16_tmp | (uint16_ttmp << 5) | (uint16_tmp << 11);
  }
#else
  for (i = 0; i < n; i++) {
    uint16_ttmp = in[i] >> 2;	/* 6 bits for Green */
    uint16_tmp = uint16_ttmp >> 1;	/* and 5 bits for Red & Blue. */
    out[i] = uint16_tmp | (uint16_ttmp << 5) | (uint16_tmp << 11);
  }
#endif
}

void
u32v_bgr32_convert_u8v_gray(uint32_t * out, const uint8_t * in, int n)
{
  int i;
  uint32_t uint32_tmp;
#ifdef HAVE_CPU_MMX
  int q, r, d;
  d = 8;			/* divisor (element blocklength) */
  q = n / d;
  r = n % d;
  for (i = 0; i < q * d; i += d)
    __asm__ volatile
      ("movq (%1), %%mm7\n\t"	/* Load 8 GRAY's */
       "movq %%mm7, %%mm0\n\t"
       "movq %%mm7, %%mm1\n\t"
       "movq %%mm7, %%mm2\n\t"
       "movq %%mm7, %%mm3\n\t"
       "punpcklbw %%mm0, %%mm0\n\t"
       "punpcklbw %%mm1, %%mm1\n\t"
       "punpckhbw %%mm2, %%mm2\n\t"
       "punpckhbw %%mm3, %%mm3\n\t"
       "punpcklwd %%mm0, %%mm0\n\t"
       "punpckhwd %%mm1, %%mm1\n\t"
       "punpcklwd %%mm2, %%mm2\n\t"
       "punpckhwd %%mm3, %%mm3\n\t"
       /* R byte is set to GRAY value so skip these:
	  "psrld $8, %%mm0\n\t"
	  "psrld $8, %%mm1\n\t"
	  "psrld $8, %%mm2\n\t"
	  "psrld $8, %%mm3\n\t" */
       "movq %%mm0,  (%0)\n\t"	/* Save two */
       "movq %%mm1, 8(%0)\n\t"	/* Save two */
       "movq %%mm2, 16(%0)\n\t"	/* Save two */
       "movq %%mm3, 24(%0)\n\t"	/* Save two */
       :
       : "r" (out + i), "r"(in + i));
  MMX_EMMS();
  for (; i < q * d + r; i++) {
    uint32_tmp = in[i];
    out[i] = (uint32_tmp << 0) | (uint32_tmp << 8) | (uint32_tmp << 16) |
      (uint32_tmp << 24);
  }
#else
  for (i = 0; i < n; i++) {
    uint32_tmp = in[i];
    out[i] = (uint32_tmp << 0) | (uint32_tmp << 8) | (uint32_tmp << 16) |
      (uint32_tmp << 24);
  }
#endif
}

void
u32v_rgb32_convert_u8v_gray(uint32_t * out, const uint8_t * in, int n)
{
  u32v_bgr32_convert_u8v_gray(out, in, n);
}

void
u8v_bgr24_convert_u8v_gray(uchar *out, const uchar *in, int n)
{
}

void
u8v_rgb24_convert_u8v_gray(uchar *out, const uchar *in, int n)
{
}

void
u8v_gray_convert_u32v_rgba(uchar *out, const uint32_t *in, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = (in[i] >> 0) & 0xff;
    g = (in[i] >> 8) & 0xff;
    b = (in[i] >> 16) & 0xff;

    out[i] = (r + g + b) / 3;
  }
}

void
u16v_rgb555_convert_u16v_rgb565(ushort *out, const ushort *in,
				int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = in[i] & (0x1f << 0) >> 0;
    g = in[i] & (0x1f << 6) >> 1;
    b = in[i] & (0x1f << 11) >> 1;

    out[i] = r | g | b;
  }
}

void
u16v_rgb565_convert_u16v_rgb555(ushort *out, const ushort *in,
				int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = in[i] & (0x1f << 0) << 0;
    g = in[i] & (0x1f << 5) << 1;
    b = in[i] & (0x1f << 10) << 1;

    out[i] = r | g | b;
  }
}

void
u16v_rgb565_convert_u8v_bgr24(ushort *out, const uchar *in,
			      int n)
{
  int i;
  for (i = 0; i < n; i++) {

    ulong r, g, b;

    r = in[3 * i + 0] & ~0x7 >> (3);
    g = in[3 * i + 1] & ~0x3 << (11 - 8);
    b = in[3 * i + 2] & ~0x7 << (16 - 8);

    out[i] = r | g | b;
  }
}

void
u8v_rgb24_convert_u8v_gray8(uchar *out, const uchar *in,
			    int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = g = b = in[i];

    out[3 * i + 0] = r;
    out[3 * i + 1] = g;
    out[3 * i + 2] = b;
  }
}

void
u8v_rgb24p_convert_u8v_gray8(uchar *out, const uchar *in,
			     int n)
{
  memcpy(out + 0 * n, in, n);
  memcpy(out + 1 * n, in, n);
  memcpy(out + 2 * n, in, n);
}

void
u8v_gray8_convert_u8v_rgb24p(uchar *out, const uchar *in,
			     int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = in[0 * n + i];
    g = in[1 * n + i];
    b = in[2 * n + i];

    out[i] = (r + g + b) / 3;
  }
}

void
u8v_bgr24_convert_u16v_rgb565(uchar *out, const ushort *in,
			      int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = in[i] & 0xff << 3;
    g = (in[i] >> 3) & ~((1 << 2) - 1);
    b = in[i + 1] & ~((1 << 3) - 1);

    out[3 * i + 0] = r;
    out[3 * i + 1] = g;
    out[3 * i + 2] = b;
  }
}

void
u8v_rgb24_convert_u32v_rgb32(uchar *out, const ulong *in,
			     int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = in[i] >> 0;
    g = in[i] >> 8;
    b = in[i] >> 16;

    out[3 * i + 0] = r;
    out[3 * i + 1] = g;
    out[3 * i + 2] = b;
  }
}

void
rgb32_convert_rgb24(ulong *out, const uchar *in, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = in[3 * i + 0];
    g = in[3 * i + 1];
    b = in[3 * i + 2];

    out[i] = (r << 0) | (g << 8) | (b << 16);
  }
}

void
bgr32_convert_rgb24(ulong *out, const uchar *in, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = in[3 * i + 0];
    g = in[3 * i + 1];
    b = in[3 * i + 2];

    out[i] = (b << 0) | (g << 8) | (r << 16);
  }
}

void
rgb32_convert_rgb32(ulong *out, const uchar *in, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    ulong r, g, b;

    r = in[3 * i + 0];
    g = in[3 * i + 1];
    b = in[3 * i + 2];

    out[i] = (b << 0) | (g << 8) | (r << 16);
  }
}
