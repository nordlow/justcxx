/*! \file smul.h
 * \brief Saturated Multiplication.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-11-28 11:03
 * \see http://groups.google.se/group/gnu.gcc.help/browse_thread/thread/12b32ed63949adde
 * \see http://www.fefe.de/intof.html
 */

#pragma once
#include "pnw_types.h"
#include "clamp.h"
#include "sadd.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \name Saturated Unsigned Multiply: \p a * \p b */
/* @{ */

static inline spure int8_t int8_smul(int8_t a, int8_t b) {
  const int32_t c = (int32_t)a * b;
  return CLAMP(INT8_MIN, c, INT8_MAX);
}
static inline spure uint8_t uint8_smul(uint8_t a, uint8_t b) {
  const uint32_t c = (uint32_t)a * b;
  return HIGH_CLAMP(c, UINT8_MAX);
}

static inline spure int16_t int16_smul(int16_t a, int16_t b){
  const int32_t c = (int32_t)a * b;
  return CLAMP(INT16_MIN, c, INT16_MAX);
}
static inline spure uint16_t uint16_smul(uint16_t a, uint16_t b) {
  const uint32_t c = (uint32_t)a * b;
  return HIGH_CLAMP(c, UINT16_MAX);
}

static inline spure int32_t int32_smul(int32_t a, int32_t b)
{
#if defined (__GNUC__) && defined (__i386__)
  int32_t c;
  __asm__ ("imull %2, %1\n\t"
           "cmovc %3, %0\n\t"
           : /* %0 */ "=r" (c)
           : /* %1 */ "0" (a),
             /* %2 */ "r" (b),
             /* %3 */ "r" ((a ^ b) & 0x80000000 ? INT32_MIN : INT32_MAX)
           );
#if 0
  __asm__ volatile ("movl %1, %%eax \n\t" /* \c eax = \p a */
                    "movl $0, %%edx \n\t"  /* \c edx = 0 */
                    "imull %2 \n\t"        /* \c edx: \c eax = \p a * \p b */
                    "movl $0xffffffff, %%ecx \n\t"
                    "cmovc %%ecx, %%eax \n\t" /* if carry set \c eax to INT32_MAX */
                    "movl %%eax, %0 \n\t" /* write \c eax to return value */
                    : "=r" (c)            /* output */
                    : "r" (a), "r" (b)    /* input */
                    : "%eax", "%ecx", "%edx" /* clobbered */
                    );
#endif
  return c;
#else
  const int64_t c = (int64_t)a * b;
  return CLAMP(INT32_MIN, c, INT32_MAX);
#endif
}
static inline spure uint32_t uint32_smul(uint32_t a, uint32_t b)
{
#if defined (__GNUC__) && defined (__i386__)
  uint32_t c;
  __asm__ volatile ("movl %1, %%eax \n\t" /* \c eax = \p a */
                    "movl $0, %%edx \n\t"  /* \c edx = 0 */
                    "mull %2 \n\t"        /* \c edx: \c eax = \p a * \p b */
                    "movl $0xffffffff, %%ecx \n\t"
                    "cmovc %%ecx, %%eax \n\t" /* if carry set \c eax to UINT32_MAX */
                    "movl %%eax, %0 \n\t" /* write \c eax to return value */
                    : "=r" (c)
                    : "r" (a), "r" (b)
                    : "%eax", "%ecx", "%edx"
                    );
  return c;
#else
  const uint64_t c = (uint64_t)a * b;
  return HIGH_CLAMP(c, UINT32_MAX);
#endif
}

static inline spure int64_t int64_smul(int64_t a, int64_t b)
{
  /* Note: For explanation see "Issue 3" at http://www.fefe.de/intof.html */

  const int32_t aH = ((int32_t*)&a)[1];
  const uint32_t aL = ((uint32_t*)&a)[0];
  const int32_t bH = ((int32_t*)&b)[1];
  const uint32_t bL = ((uint32_t*)&b)[0];

  // The first term is shifted by 64 bits, so if both aH and bH are
  // nonzero, we have an overflow right there and can abort.
  if (aH && bH) { return INT64_MAX; } /* overflow */

  // At least one the terms in the following addition must be zero
  // (from previous calculation), so we can't have an overflow in the
  // addition. Then, if a > \c UINT32_MAX, we have an overflow in the
  // multiplication and can abort.
  const int64_t x = (int64_t)aH * bL + (int64_t)aL * bH;
  if (x > INT32_MAX) { return INT64_MAX; } /* overflow */

  // finally we can do
  return int64_sadd((x << 32), (int64_t)aL * bL);
}

static inline spure uint64_t uint64_smul(uint64_t a, uint64_t b)
{
  /* Note: For explanation see "Issue 3" at http://www.fefe.de/intof.html */

  const uint32_t aH = ((uint32_t*)&a)[1];
  const uint32_t aL = ((uint32_t*)&a)[0];
  const uint32_t bH = ((uint32_t*)&b)[1];
  const uint32_t bL = ((uint32_t*)&b)[0];

  // The first term is shifted by 64 bits, so if both aH and bH are
  // nonzero, we have an overflow right there and can abort.
  if (aH && bH) { return UINT64_MAX; } /* overflow */

  // At least one the terms in the following addition must be zero
  // (from previous calculation), so we can't have an overflow in the
  // addition. Then, if a > \c UINT32_MAX, we have an overflow in the
  // multiplication and can abort.
  const uint64_t x = (uint64_t)aH * bL + (uint64_t)aL * bH;
  if (x > UINT32_MAX) { return UINT64_MAX; } /* overflow */

  // finally we can do
  return uint64_sadd((x << 32), (uint64_t)aL * bL);
}

/* @} */

#ifdef __cplusplus
}
#endif
