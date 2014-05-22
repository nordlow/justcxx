/*!
 * \file bitlogic.h
 * \brief Bitwise Logical Operations on C Arrays.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \name Bitwise Logical Operations on Byte Arrays.
 */
/* @{ */

static inline void
uint8array64_and(uint8_t * out, const uint8_t * in1, const uint8_t * in2)
{
#ifdef HAVE_CPU_MMX
  __asm__ volatile ("movq   (%1), %%mm0\n\t"
                    "movq  8(%1), %%mm1\n\t"
                    "movq 16(%1), %%mm2\n\t"
                    "movq 24(%1), %%mm3\n\t"
                    "movq 32(%1), %%mm4\n\t"
                    "movq 40(%1), %%mm5\n\t"
                    "movq 48(%1), %%mm6\n\t"
                    "movq 56(%1), %%mm7\n\t"
                    "pand   (%2), %%mm0\n\t"
                    "pand  8(%2), %%mm1\n\t"
                    "pand 16(%2), %%mm2\n\t"
                    "pand 24(%2), %%mm3\n\t"
                    "pand 32(%2), %%mm4\n\t"
                    "pand 40(%2), %%mm5\n\t"
                    "pand 48(%2), %%mm6\n\t"
                    "pand 56(%2), %%mm7\n\t"
                    "movq %%mm0,   (%0)\n\t"
                    "movq %%mm1,  8(%0)\n\t"
                    "movq %%mm2, 16(%0)\n\t"
                    "movq %%mm3, 24(%0)\n\t"
                    "movq %%mm4, 32(%0)\n\t"
                    "movq %%mm5, 40(%0)\n\t"
                    "movq %%mm6, 48(%0)\n\t"
                    "movq %%mm7, 56(%0)\n\t"
                    :
                    :"r" (out), "r"(in1), "r"(in2));
#endif
}

static inline void
uint8array64_or(uint8_t * out, const uint8_t * in1, const uint8_t * in2)
{
#ifdef HAVE_CPU_MMX
  __asm__ volatile ("movq   (%1), %%mm0\n\t"
                    "movq  8(%1), %%mm1\n\t"
                    "movq 16(%1), %%mm2\n\t"
                    "movq 24(%1), %%mm3\n\t"
                    "movq 32(%1), %%mm4\n\t"
                    "movq 40(%1), %%mm5\n\t"
                    "movq 48(%1), %%mm6\n\t"
                    "movq 56(%1), %%mm7\n\t"
                    "por   (%2), %%mm0\n\t"
                    "por  8(%2), %%mm1\n\t"
                    "por 16(%2), %%mm2\n\t"
                    "por 24(%2), %%mm3\n\t"
                    "por 32(%2), %%mm4\n\t"
                    "por 40(%2), %%mm5\n\t"
                    "por 48(%2), %%mm6\n\t"
                    "por 56(%2), %%mm7\n\t"
                    "movq %%mm0,   (%0)\n\t"
                    "movq %%mm1,  8(%0)\n\t"
                    "movq %%mm2, 16(%0)\n\t"
                    "movq %%mm3, 24(%0)\n\t"
                    "movq %%mm4, 32(%0)\n\t"
                    "movq %%mm5, 40(%0)\n\t"
                    "movq %%mm6, 48(%0)\n\t"
                    "movq %%mm7, 56(%0)\n\t"
                    :
                    :"r" (out), "r"(in1), "r"(in2));
#endif
}

static inline void
uint8array64_xor(uint8_t * out, const uint8_t * in1, const uint8_t * in2)
{
#ifdef HAVE_CPU_MMX
  __asm__ volatile ("movq   (%1), %%mm0\n\t"
                    "movq  8(%1), %%mm1\n\t"
                    "movq 16(%1), %%mm2\n\t"
                    "movq 24(%1), %%mm3\n\t"
                    "movq 32(%1), %%mm4\n\t"
                    "movq 40(%1), %%mm5\n\t"
                    "movq 48(%1), %%mm6\n\t"
                    "movq 56(%1), %%mm7\n\t"
                    "pxor   (%2), %%mm0\n\t"
                    "pxor  8(%2), %%mm1\n\t"
                    "pxor 16(%2), %%mm2\n\t"
                    "pxor 24(%2), %%mm3\n\t"
                    "pxor 32(%2), %%mm4\n\t"
                    "pxor 40(%2), %%mm5\n\t"
                    "pxor 48(%2), %%mm6\n\t"
                    "pxor 56(%2), %%mm7\n\t"
                    "movq %%mm0,   (%0)\n\t"
                    "movq %%mm1,  8(%0)\n\t"
                    "movq %%mm2, 16(%0)\n\t"
                    "movq %%mm3, 24(%0)\n\t"
                    "movq %%mm4, 32(%0)\n\t"
                    "movq %%mm5, 40(%0)\n\t"
                    "movq %%mm6, 48(%0)\n\t"
                    "movq %%mm7, 56(%0)\n\t"
                    :
                    :"r" (out), "r"(in1), "r"(in2));
#endif
}

/* @} */

#ifdef __cplusplus
}
#endif
