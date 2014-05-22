/*!
 * \file packed_ops_SIMD.h
 * \brief Wrappers for SIMD-optimized operations - Uses SSE, SSE2,
 *       SSE3, SSE4, etc..
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-08-21 12:36
 *
 * Uses non-aligned version of the SSE-instruction movXX.

 * If you specify align to 16 bytes immediately before the relevant data
 * declaration, however, then the assembler will make sure the data is
 * on a 16-byte boundary and the faster MOVAPS/MOVAPD (move four aligned
 * packed single-precision) can be used instead of MOVSS/MOVSD.
 */

#pragma once

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "is_aligned.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Parallel Add \p a and \p b to \p c in float (using SSE). */
static inline
void float_aligned16_SSE_add(farray_aligned16 a,
			     farray_aligned16 b,
			     farray_aligned16 c)
{
#ifdef HAVE_CPU_SSE
  __asm__ volatile ("movaps (%0), %%xmm0;"
                    "movaps (%1), %%xmm1;"
                    "addps %%xmm0, %%xmm1;"
                    "movaps %%xmm1, (%2);"
                    :
                    : "r"(a.dat), "r"(b.dat), "r"(c.dat)
                    : "%xmm0", "%xmm1");
#endif
}

/*! Parallel Multiply \p a and \p b to \p c in float (using SSE). */
static inline
void float_aligned16_SSE_mul(farray_aligned16 a,
			     farray_aligned16 b,
			     farray_aligned16 c)
{
#ifdef HAVE_CPU_SSE
  __asm__ volatile ("movaps (%0), %%xmm0;"
                    "movaps (%1), %%xmm1;"
                    "mulps %%xmm0, %%xmm1;"
                    "movaps %%xmm1, (%2);"
                    :
                    : "r"(a.dat), "r"(b.dat), "r"(c.dat)
                    : "%xmm0", "%xmm1");
#endif
}

/*! Parallel Add \p a and \p b to \p c in double (using SSE2). */
static inline
void double_aligned16_SSE2_add(darray_aligned16 a,
			       darray_aligned16 b,
			       darray_aligned16 c)
{
#ifdef HAVE_CPU_SSE2
  __asm__ volatile ("movapd (%0), %%xmm0;"
                    "movapd (%1), %%xmm1;"
                    "addpd %%xmm0, %%xmm1;"
                    "movapd %%xmm1, (%2);"
                    :
                    : "r"(a.dat), "r"(b.dat), "r"(c.dat)
                    : "%xmm0", "%xmm1");
#endif
}

/*! Parallel Multiply \p a and \p b to \p c in double (using SSE2). */
static inline
void double_aligned16_SSE2_mul(darray_aligned16 a,
			       darray_aligned16 b,
			       darray_aligned16 c)
{
#ifdef HAVE_CPU_SSE2
  __asm__ volatile ("movapd (%0), %%xmm0;"
                    "movapd (%1), %%xmm1;"
                    "mulpd %%xmm0, %%xmm1;"
                    "movapd %%xmm1, (%2);"
                    :
                    : "r"(a.dat), "r"(b.dat), "r"(c.dat)
                    : "%xmm0", "%xmm1");
#endif
}

/* ---------------------------- Group Separator ---------------------------- */

static inline
void float_unaligned_SSE_add(float *a, float *b, float *c)
{
#ifdef HAVE_CPU_SSE
  __asm__ volatile ("movss (%0), %%xmm0;"
                    "movss (%1), %%xmm1;"
                    "addss %%xmm0, %%xmm1;"
                    "movss %%xmm1, (%2);"
                    :
                    : "r"(a), "r"(b), "r"(c)
                    : "%xmm0", "%xmm1");
#endif
}

static inline
void float_unaligned_SSE_mul(float *a, float *b, float *c)
{
#ifdef HAVE_CPU_SSE
  __asm__ volatile ("movss (%0), %%xmm0;"
                    "movss (%1), %%xmm1;"
                    "mulss %%xmm0, %%xmm1;"
                    "movss %%xmm1, (%2);"
                    :
                    : "r"(a), "r"(b), "r"(c)
                    : "%xmm0", "%xmm1");
#endif
}

static inline
void double_unaligned_SSE2_add(double *a, double *b, double *c)
{
#ifdef HAVE_CPU_SSE2
  __asm__ volatile ("movsd (%0), %%xmm0;"
                    "movsd (%1), %%xmm1;"
                    "addsd %%xmm0, %%xmm1;"
                    "movsd %%xmm1, (%2);"
                    :
                    : "r"(a), "r"(b), "r"(c)
                    : "%xmm0", "%xmm1");
#endif
}

static inline
void double_unaligned_SSE2_mul(double *a, double *b, double *c)
{
#ifdef HAVE_CPU_SSE2
  __asm__ volatile ("movsd (%0), %%xmm0;"
                    "movsd (%1), %%xmm1;"
                    "mulsd %%xmm0, %%xmm1;"
                    "movsd %%xmm1, (%2);"
                    :
                    : "r"(a), "r"(b), "r"(c)
                    : "%xmm0", "%xmm1");
#endif
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
