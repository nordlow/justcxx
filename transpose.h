#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

void init_transpose(void);

/*!
 * Performs a Very Optimized \em Transpose of a 8-by-8 Matrix of bytes
 * at \p a into \p b using \c MMX.
 *
 * \param[in] in is the in matrix.
 * \param[out] out is the out matrix that must be disjunct from \p in.
 */
#ifdef HAVE_CPU_MMX
static inline void
chararray_transpose8x8(char *out, char *in)
{
#ifndef NDEBUG
  if (out == in) { PERR("out == in!\n"); }
#endif
  __asm__ volatile (
		     /* Row 0-3: Read stage begins. */
		     /* ########################## */
		     "movq  0(%1), %%mm0 \n\t"
		     "movq  8(%1), %%mm1 \n\t"
		     "movq 16(%1), %%mm2 \n\t"
		     "movq 24(%1), %%mm3 \n\t"
		     "movq 32(%1), %%mm4 \n\t"
		     "movq 40(%1), %%mm5 \n\t"
		     "movq 48(%1), %%mm6 \n\t"
		     "movq 56(%1), %%mm7 \n\t"
		     /* Row 0-3: First stage begins. */
		     "punpcklbw %%mm1, %%mm0 \n\t"
		     "punpcklbw %%mm3, %%mm2 \n\t"
		     "punpcklbw %%mm5, %%mm4 \n\t"
		     "punpcklbw %%mm7, %%mm6 \n\t"
		     /* Row 0-3: Second stage begins. */
		     "movq      %%mm0, %%mm1 \n\t"
		     "punpcklwd %%mm2, %%mm0 \n\t"
		     "punpckhwd %%mm2, %%mm1 \n\t"
		     "movq      %%mm4, %%mm5 \n\t"
		     "punpcklwd %%mm6, %%mm4 \n\t"
		     "punpckhwd %%mm6, %%mm5 \n\t"
		     /* Row 0-3: Third stage begins. */
		     "movq      %%mm0, %%mm2 \n\t"
		     "punpckldq %%mm4, %%mm0 \n\t"
		     "punpckhdq %%mm4, %%mm2 \n\t"
		     "movq      %%mm1, %%mm3 \n\t"
		     "punpckldq %%mm5, %%mm1 \n\t"
		     "punpckhdq %%mm5, %%mm3 \n\t"
		     /* Row 0-3: Write stage begins. */
		     "movq %%mm0,  0(%0) \n\t"
		     "movq %%mm2,  8(%0) \n\t"
		     "movq %%mm1, 16(%0) \n\t"
		     "movq %%mm3, 24(%0) \n\t"
		     /* Row 4-7: Read stage begins. */
		     /* ########################## */
		     "movq  0(%1), %%mm0 \n\t"
		     "movq  8(%1), %%mm1 \n\t"
		     "movq 16(%1), %%mm2 \n\t"
		     "movq 24(%1), %%mm3 \n\t"
		     "movq 32(%1), %%mm4 \n\t"
		     "movq 40(%1), %%mm5 \n\t"
		     "movq 48(%1), %%mm6 \n\t"
		     "movq 56(%1), %%mm7 \n\t"
		     /* Row 4-7: First stage begins. */
		     "punpckhbw %%mm1, %%mm0 \n\t"
		     "punpckhbw %%mm3, %%mm2 \n\t"
		     "punpckhbw %%mm5, %%mm4 \n\t"
		     "punpckhbw %%mm7, %%mm6 \n\t"
		     /* Row 4-7: Second stage begins. */
		     "movq      %%mm0, %%mm1 \n\t"
		     "punpcklwd %%mm2, %%mm0 \n\t"
		     "punpckhwd %%mm2, %%mm1 \n\t"
		     "movq      %%mm4, %%mm5 \n\t"
		     "punpcklwd %%mm6, %%mm4 \n\t"
		     "punpckhwd %%mm6, %%mm5 \n\t"
		     /* Row 4-7: Third stage begins. */
		     "movq      %%mm0, %%mm2 \n\t"
		     "punpckldq %%mm4, %%mm0 \n\t"
		     "punpckhdq %%mm4, %%mm2 \n\t"
		     "movq      %%mm1, %%mm3 \n\t"
		     "punpckldq %%mm5, %%mm1 \n\t"
		     "punpckhdq %%mm5, %%mm3 \n\t"
		     /* Row 4-7: Write stage begins. */
		     "movq %%mm0, 32(%0) \n\t"
		     "movq %%mm2, 40(%0) \n\t"
		     "movq %%mm1, 48(%0) \n\t"
		     "movq %%mm3, 56(%0) \n\t"::"r" (out), "r"(in));
  MMX_EMMS();
}
#endif

#ifdef HAVE_CPU_MMX
static inline void
shortarray_transpose4x4(short *out, short *in)
{
  __asm__ volatile (""::"r" (out), "r"(in));
  MMX_EMMS();
}
#endif

#ifdef HAVE_CPU_MMX
static inline void
longarray_transpose2x2(long *out, long *in)
{
  __asm__ volatile (""::"r" (out), "r"(in));
  MMX_EMMS();
}
#endif

void llongmatrix_sqr_transpose(long long **out, int side);

void llongmatrix_transpose(long long **out, const long long **in, int w_out,
		      int h_out);

void chara8_bitTranspose8x8(char *out, const char *in);

#if 0
void b1m_transpose_slow(b1 ** out, const b1 ** in, int n, int m);
void b1m_transpose(b1 ** out, const b1 ** in, int w_out, int h_out);
#endif

void dmatrix_transpose(double **a, double **b, int bm, int bn);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
