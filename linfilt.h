/*!
 * \file linfilt.h
 * \brief Linear Filtering.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#include "is_aligned.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Affine transform (float) in 2 dimensions.
 *
 * x = A * x + b.
 *
 * \see mat2f_mvprod()
 */
static inline void
float_affine2d(float x[2],
	     const float a[4], const float b[2])
{
#ifdef HAVE_CPU_3DNOW
  __asm__ volatile
    ("movq   (%0), %%mm0 \n\t"	// x[0,1] => mm0
     "movq   (%1), %%mm1 \n\t"	// a[0,1] => mm1
     "movq  8(%1), %%mm2 \n\t"	// a[2,3] => mm2
     "movq   (%2), %%mm3 \n\t"	// b[0,1] => mm3

     "pfmul %%mm0, %%mm1 \n\t"
     "pfacc %%mm1, %%mm1 \n\t"	// 1:st dot => mm1
     "pfmul %%mm0, %%mm2 \n\t"
     "pfacc %%mm2, %%mm2 \n\t"	// 2:nd dot => mm2

     "punpckldq %%mm2, %%mm1 \n\t" // a * x => mm1

     "pfadd %%mm3, %%mm1 \n\t"	// a * x + b => mm1

     "movq  %%mm1,  (%0) \n\t"	// mm1 => x

     "femms              \n\t"	// cleaup mmx state
     :				// out args to C
     :"r" (x), "r"(a), "r"(b));	// in args from C
#else
  float x0 = x[0], x1 = x[1];
  x[0] = a[0] * x0 + a[1] * x1 + b[0];
  x[1] = a[2] * x0 + a[3] * x1 + b[1];
#endif
}

/* ------------------------------- SEPARATOR ------------------------------- */

/*!
 * Pointwise subtract and scale (float) in 2 dimensions.
 *
 * x = (x - a) .* b
 */
static inline void
fa2_ss(float x[2],
       const float a[2], const float b[2])
{
#ifdef HAVE_CPU_3DNOW
  __asm__ volatile
    ("movq   (%0), %%mm0 \n\t"	// x[0,1] => mm0
     "movq   (%1), %%mm1 \n\t"	// a[0,1] => mm1
     "movq   (%2), %%mm2 \n\t"	// b[0,1] => mm2

     "pfsub %%mm1, %%mm0 \n\t"	// x - a => mm0
     "pfmul %%mm2, %%mm0 \n\t"	// (x - a) .* b => mm0

     "movq  %%mm0,  (%0) \n\t"	// mm0 => x

     "femms              \n\t"	// cleaup mmx state
     :				// out args to C
     :"r" (x), "r"(a), "r"(b));	// in args from C
#else
  x[0] = (x[0] - a[0]) * b[0];
  x[1] = (x[1] - a[1]) * b[1];
#endif
}

/* ------------------------------- SEPARATOR ------------------------------- */

/*!
 * Pointwise subtract and scale (float to int) in 2 dimensions.
 *
 * y = (x - a) .* b
 */
static inline void
fa2_ss_i(int y[2],
	 const float x[2], const float a[2], const float b[2])
{
#ifdef HAVE_CPU_3DNOW
  __asm__ volatile
    ("movq   (%1), %%mm0 \n\t"	// x[0,1] => mm0
     "movq   (%2), %%mm1 \n\t"	// a[0,1] => mm1
     "movq   (%3), %%mm2 \n\t"	// b[0,1] => mm2

     "pfsub %%mm1, %%mm0 \n\t"	// x - a => mm0
     "pfmul %%mm2, %%mm0 \n\t"	// (x - a) .* b => mm0

     "pf2id %%mm0, %%mm3 \n\t"	// convert to int format

     "movq  %%mm3,  (%0) \n\t"	// mm3 => y

     "femms              \n\t"	// cleaup mmx state
     :				// out args to C
     : "r"(y),
       "r"(x), "r"(a), "r"(b));	// in args from C
#else
  y[0] = (short) ((x[0] - a[0]) * b[0]);
  y[1] = (short) ((x[1] - a[1]) * b[1]);
#endif
}

/* ------------------------------- SEPARATOR ------------------------------- */

/*!
 * Pointwise subtract and scale (float to short) in 2 dimensions.
 *
 * y = (x - a) .* b
 */
static inline void
fa2_ss_s(short y[2],
	 const float x[2], const float a[2], const float b[2])
{
#ifdef HAVE_CPU_3DNOW
  __asm__ volatile
    ("movq   (%1), %%mm0 \n\t"	// x[0,1] => mm0
     "movq   (%2), %%mm1 \n\t"	// a[0,1] => mm1
     "movq   (%3), %%mm2 \n\t"	// b[0,1] => mm2

     "pfsub %%mm1, %%mm0 \n\t"	// x - a => mm0
     "pfmul %%mm2, %%mm0 \n\t"	// (x - a) .* b => mm0

     "pf2id %%mm0, %%mm3 \n\t"	// convert to int format

     "pxor  %%mm4, %%mm4 \n\t"	// zero mm4
     "packssdw %%mm4, %%mm3 \n\t"// 2 int to short => mm3[0..31]

     "movd  %%mm3,  (%0) \n\t"	// mm3[0...31] => y

     "femms              \n\t"	// cleaup mmx state
     :				// out args to C
     : "r"(y),
       "r"(x), "r"(a), "r"(b));	// in args from C
#else
  y[0] = (short) ((x[0] - a[0]) * b[0]);
  y[1] = (short) ((x[1] - a[1]) * b[1]);
#endif
}

/* ------------------------------- SEPARATOR ------------------------------- */

/*!
 * Pointwise subtract, scale and truncate-adjust (in float), convert to short.
 *
 * y = (short)((x - a) .* b + t)
 */
static inline void
fa2_sst(short y[2],
	const float x[2],
	const float a[2], const float b[2], const float t[2])
{
#ifdef HAVE_CPU_3DNOW
  __asm__ volatile
    ("movq   (%1), %%mm0 \n\t"	// x[0,1] => mm0
     "movq   (%2), %%mm1 \n\t"	// a[0,1] => mm1
     "movq   (%3), %%mm2 \n\t"	// b[0,1] => mm2
     "movq   (%4), %%mm6 \n\t"	// t[0,1] => mm3

     "pfsub %%mm1, %%mm0 \n\t"	// x - a => mm0
     "pfmul %%mm2, %%mm0 \n\t"	// (x - a) .* b => mm0
     "pfadd %%mm6, %%mm0 \n\t"	// (x - a) .* b + t => mm0

     "pf2id %%mm0, %%mm4 \n\t"	// convert to short format

     "pxor  %%mm5, %%mm5 \n\t"	// zero mm5
     "packssdw %%mm5, %%mm4 \n\t"// 2 int to short => mm3[0..31]

     "movd  %%mm4,  (%0) \n\t"	// mm4[0...31] => y

     "femms              \n\t"	// cleaup mmx state
     :				// out args to C
     : "r"(y),
       "r"(x), "r"(a), "r"(b), "r"(t));	// in args from C
#else
  y[0] = (short) ((x[0] - a[0]) * b[0] + t[0]);
  y[1] = (short) ((x[1] - a[1]) * b[1] + t[1]);
#endif
}

/* ------------------------------- SEPARATOR ------------------------------- */

/*!
 * Pointwise subtract, scale and truncate-adjust (in float), convert to short
 * and then shift in 2 dimensions.
 *
 * y = (short)((x - a) .* b + t) + s
 */
static inline void
fa2_sst_sc(short y[2],
	   const float x[2],
	   const float a[2], const float b[2], const float t[2],
	   const short s[2])
{
#ifdef HAVE_CPU_3DNOW
  __asm__ volatile
    ("movq   (%1), %%mm0 \n\t"	// x[0,1] => mm0
     "movq   (%2), %%mm1 \n\t"	// a[0,1] => mm1
     "movq   (%3), %%mm2 \n\t"	// b[0,1] => mm2
     "movq   (%4), %%mm6 \n\t"	// t[0,1] => mm6
     "movd   (%5), %%mm3 \n\t"	// s[0,1] => mm3

     "pfsub %%mm1, %%mm0 \n\t"	// x - a => mm0
     "pfmul %%mm2, %%mm0 \n\t"	// (x - a) .* b => mm0
     "pfadd %%mm6, %%mm0 \n\t"	// (x - a) .* b + t => mm0

     "pf2id %%mm0, %%mm4 \n\t"	// convert to short format

     "pxor  %%mm5, %%mm5 \n\t"	// zero mm5
     "packssdw %%mm5, %%mm4 \n\t"// 2 int to short => mm3[0..31]
     "paddw %%mm3, %%mm4 \n\t"	// add the shift

     "movd  %%mm4,  (%0) \n\t"	// mm4[0...31] => y

     "femms              \n\t"	// cleaup mmx state
     :				// out args to C
     : "r"(y),
       "r"(x), "r"(a), "r"(b), "r"(t), "r"(s));	// in args from C
#else
  y[0] = (short) ((x[0] - a[0]) * b[0] + t[0]) + s[0];
  y[1] = (short) ((x[1] - a[1]) * b[1] + t[1]) + s[1];
#endif
}

/* ------------------------------- SEPARATOR ------------------------------- */

/*!
 * Pointwise subtract and scale (in float), convert to short and
 * shift in 2 dimensions.
 *
 * y = (short)((x - a) .* b) + s
 */
static inline void
fa2_ss_sc(short y[2],
	  const float x[2], const float a[2], const float b[2],
	  const short s[2])
{
#ifdef HAVE_CPU_3DNOW
  __asm__ volatile
    ("movq   (%1), %%mm0 \n\t"	// x[0,1] => mm0
     "movq   (%2), %%mm1 \n\t"	// a[0,1] => mm1
     "movq   (%3), %%mm2 \n\t"	// b[0,1] => mm2
     "movd   (%4), %%mm3 \n\t"	// s[0,1] => mm3

     "pfsub %%mm1, %%mm0 \n\t"	// x - a => mm0
     "pfmul %%mm2, %%mm0 \n\t"	// (x - a) .* b => mm0

     "pf2id %%mm0, %%mm4 \n\t"	// convert to short format

     "pxor  %%mm5, %%mm5 \n\t"	// zero mm5
     "packssdw %%mm5, %%mm4 \n\t"// 2 int to short => mm3[0..31]
     "paddw %%mm3, %%mm4 \n\t"	// add the shift

     "movd  %%mm4,  (%0) \n\t"	// mm4[0...31] => y

     "femms              \n\t"	// cleaup mmx state
     :				// out args to C
     : "r"(y),
       "r"(x), "r"(a), "r"(b), "r"(s));	// in args from C
#else
  y[0] = (short) ((x[0] - a[0]) * b[0]) + s[0];
  y[1] = (short) ((x[1] - a[1]) * b[1]) + s[1];
#endif
}

/* ------------------------------- SEPARATOR ------------------------------- */

/*!
 * Pointwise subtract and scale in 2 dimensions (2-way parallel):
 *
 * y0 = (x0 - a0) .* b0
 * y1 = (x1 - a1) .* b1
 */
static inline void
sh_fa2_ss_two(int *y0, const float *x0, const float *a0, const float *b0,
	      int *y1, const float *x1, const float *a1, const float *b1)
{
  y0[0] = (short) ((x0[0] - a0[0]) * b0[0]);
  y0[1] = (short) ((x0[1] - a0[1]) * b0[1]);

  y1[0] = (short) ((x1[0] - a1[0]) * b1[0]);
  y1[1] = (short) ((x1[1] - a1[1]) * b1[1]);
}

/* ------------------------------- SEPARATOR ------------------------------- */

/*!
 * Pointwise subtract and scale in 2 dimensions (3-way parallel):
 *
 * y0 = (x0 - a0) .* b0
 * y1 = (x1 - a1) .* b1
 * y2 = (x2 - a2) .* b2
 */
static inline void
sh_fa2_ss_three(int *y0, const float *x0, const float *a0,
		const float *b0, int *y1, const float *x1,
		const float *a1, const float *b1, int *y2,
		const float *x2, const float *a2, const float *b2)
{
  y0[0] = (short) ((x0[0] - a0[0]) * b0[0]);
  y0[1] = (short) ((x0[1] - a0[1]) * b0[1]);

  y1[0] = (short) ((x1[0] - a1[0]) * b1[0]);
  y1[1] = (short) ((x1[1] - a1[1]) * b1[1]);

  y2[0] = (short) ((x2[0] - a2[0]) * b2[0]);
  y2[1] = (short) ((x2[1] - a2[1]) * b2[1]);
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
