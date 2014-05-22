/*!
 * \file complexf.h
 * \brief Complex float.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <math.h>
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

#ifdef PNW_USE_GNUC_COMPLEX
typedef _Complex float complexf;
#else
typedef struct
{
  float re, im;
} complexf;
#endif

/* ---------------------------- Group Separator ---------------------------- */

static inline float
complexf_get0(const complexf * a)
{
#ifdef PNW_USE_GNUC_COMPLEX
  return __real__ * a;
#else
  return a->re;
#endif
}

static inline float
complexf_get1(const complexf * a)
{
#ifdef PNW_USE_GNUC_COMPLEX
  return __imag__ * a;
#else
  return a->im;
#endif
}

static inline float
complexf_at(const complexf * a, int i)
{
#ifdef PNW_USE_GNUC_COMPLEX
  return i == 0 ? __real__ * a : __imag__ * a;
#else
  return (&a->re)[i];
#endif
}

static inline complexf
complexf_init(float a, float b)
{
  complexf v;
#ifdef PNW_USE_GNUC_COMPLEX
  __real__ v = a;
  __imag__ v = b;
#else
  v.re = a;
  v.im = b;
#endif
  return v;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Complex float multiply.
 */
static inline void
complexf_mul(complexf * rop, const complexf * op1, const complexf * op2)
{
#ifdef HAVE_CPU_3DNOW
  const uint64_t uint64_neg_2nd_f32 = 0x8000000000000000ULL;
  __asm__ volatile("movq       (%1), %%mm6 \n\t" /* Load x1 */
		   "movq       (%2), %%mm7 \n\t" /* Load x2 */
		   "movq      %%mm6, %%mm0 \n\t"
		   "pfmul     %%mm7, %%mm0 \n\t"
		   "pxor       (%3), %%mm0 \n\t"
		   "pfacc     %%mm0, %%mm0 \n\t" /* Real part in mm0[0..31] */
		   "movq      %%mm7, %%mm1 \n\t"
		   "punpckhdq %%mm1, %%mm1 \n\t"
		   "punpckldq %%mm7, %%mm1 \n\t"
		   "pfmul     %%mm6, %%mm1 \n\t"
		   "pfacc     %%mm1, %%mm1 \n\t" /* Imag part in mm1[0..31] */
		   "punpckldq %%mm1, %%mm0 \n\t" /* Merge real and imag */
		   "movq      %%mm0, (%0)  \n\t" /* and save it. */
		   "femms                  \n\t"::"r" (rop), "r"(op1),
     "r"(op2), "r"(&uint64_neg_2nd_f32));
#else
#ifdef PNW_USE_GNUC_COMPLEX
  *rop = *op1 * *op2;
#else
  if (rop == op1 || rop == op2) {
    complexf tmp;
    tmp.re = op1->re * op2->re - op1->im * op2->im;
    tmp.im = op1->re * op2->im + op1->im * op2->re;
    *rop = tmp;
  } else {
    rop->re = op1->re * op2->re - op1->im * op2->im;
    rop->im = op1->re * op2->im + op1->im * op2->re;
  }
#endif
#endif
}

/*!
 * Complex norm.
 */
static inline void
complexf_norm(complexf * rop, const complexf * op)
{
#ifdef PNW_USE_GNUC_COMPLEX
#else
  float re = fabs(op->re), im = fabs(op->im), q;
  if (re >= im) {
    q = im / re;
    rop->re = re * sqrtf(1 + q * q);
  } else {
    q = re / im;
    rop->re = im * sqrtf(1 + q * q);
  }
  rop->im = 0;
#endif
}

/*!
 * Complex division.
 */
static inline void
complexf_div(complexf * rop, const complexf * op1, const complexf * op2)
{
#ifdef PNW_USE_GNUC_COMPLEX
  *rop = *op1 / *op2;
#else
  if (rop == op1 || rop == op2) {
    PERR("rop == op1 || rop == op2. no division performed\n");
  } else {
    float q, den;		/* quotient and denominator */
    if (op2->re == 0.0) {
      rop->re = op1->im / op2->im;
      rop->im = -op1->re / op2->im;
    } else if (op2->im == 0.0) {
      rop->re = op1->re / op2->re;
      rop->im = op1->im / op2->re;
    } else if (fabs(op1->re) >= fabs(op1->im)) {
      q = op2->im / op2->re;
      den = op2->re + q * op2->im;
      rop->re = (op1->re + q * op1->im) / den;
      rop->im = (op1->im - q * op1->re) / den;
    } else {
      q = op2->re / op2->im;
      den = op2->im + q * op2->re;
      rop->re = (op1->im + q * op1->re) / den;
      rop->im = (op1->im * q - op1->re) / den;
    }
  }
#endif
}

/*!
 * Print complex number op to \p stream.
 */
static inline void
complexf_out_str(FILE * stream, const complexf * op)
{
#ifdef PNW_USE_GNUC_COMPLEX
  fprintf(stream, "(re:%f,im:%f)", __real__ * op, __imag__ * op);
#else
  fprintf(stream, "(re:%f,im:%f)", op->re, op->im);
#endif
}

/*!
 * Print array of complex numbers in of length \p n to \p stream.
 */
void
complexfarray_fprint(FILE * stream, const complexf * in, int n);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
