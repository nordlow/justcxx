/*! \file ssub.h
 * \brief Saturated Subtraction.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-11-28 11:03
 * \see http://groups.google.se/group/gnu.gcc.help/browse_thread/thread/12b32ed63949adde
 */

#pragma once
#include "pnw_types.h"
#include "clamp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \name Saturated Signed Subtraction: \p a - \p b */
/* @{ */
static inline spure int8_t int8_ssub(int8_t a, int8_t b)
{
  const int32_t c = (int32_t)a - b;
  return CLAMP(INT8_MIN, c, INT8_MAX);
}
static inline spure int16_t int16_ssub(int16_t a, int16_t b)
{
  const int32_t c = (int32_t)a - b;
  return CLAMP(INT16_MIN, c, INT16_MAX);
}
static inline spure int32_t int32_ssub(int32_t a, int32_t b)
{
#if defined (__GNUC__) && defined (__i386__)
  int32_t c;
  __asm__ volatile ("sub %2, %0 \n\t" /* \c eax = \p a - \p b */
                    "cmovc %3, %0 \n\t" /* saturate if carry flag */
                    : "=r" (c)  /* %0 */
                    : "0" (a),  /* %1 */
                      "rm" (b), /* %2 */
                      "rm" (INT32_MIN) /* %3 */
                    );
  /* Alternative Version: */
  /* __asm__ volatile ("sub %2, %0 \n\t" /\* \c eax = \p a + \p b *\/ */
  /*                   "mov $0, %%edx \n\t" */
  /*                   "cmovc %%edx, %0 \n\t" /\* if carry(borrow) set \c eax to 0 *\/ */
  /*                   : "=r" (c)                /\* output *\/ */
  /*                   : "0" (a), "rm" (b)        /\* input *\/ */
  /*                   ); */
  return c;
#else
  const int64_t c = (int64_t)a - b;
  return CLAMP(INT32_MIN, c, INT32_MAX);
#endif
}
static inline spure int64_t int64_ssub(int64_t a, int64_t b)
{
  if (a >= 0) {
    if (b >= 0) {               /* a (P) - b (P) */
      return a - b;             /* no overflow possible! */
    } else {                    /* a (P) - b (N) */
      return ((INT64_MAX - a < b) ? INT64_MAX : a - b); /* \c int64_sadd(a, -b) also works! */
    }
  } else {
    if (b >= 0) {               /* a (N) - b (P) */
      return ((a + INT64_MIN < b) ? INT64_MIN : a - b);
    } else {                    /* a (N) - b (N) */
      return (a == 0 && b == INT64_MIN) ? INT64_MAX : a - b; /* Note: overflow when \p a is 0 and \p b is \c INT64_MIN! */
    }
  }
}
/* @} */

/* ========================================================================= */

/*! \name Saturated Unsigned Subtraction: \p a - \p b */
/* @{ */
static inline spure uint8_t uint8_ssub(uint8_t a, uint8_t b)
{
  if (a > b) { return a-b; } else { return 0; }
}
static inline spure uint16_t uint16_ssub(uint16_t a, uint16_t b)
{
  if (a > b) { return a-b; } else { return 0; }
}
static inline spure uint32_t uint32_ssub(uint32_t a, uint32_t b)
{
#if defined (__GNUC__) && defined (__i386__)
  uint32_t c;
  __asm__ volatile ("sub %2, %0 \n\t" /* \c eax = \p a - \p b */
                    "cmovc %3, %0 \n\t" /* saturate if carry flag */
                    : "=r" (c)  /* %0 */
                    : "0" (a),  /* %1 */
                      "rm" (b), /* %2 */
                      "rm" (UINT32_MIN) /* %3 */
                    );
  /* Alternative Version: */
  /* __asm__ volatile ("sub %2, %0 \n\t" /\* \c eax = \p a + \p b *\/ */
  /*                   "mov $0, %%edx \n\t" */
  /*                   "cmovc %%edx, %0 \n\t" /\* if carry(borrow) set \c eax to 0 *\/ */
  /*                   : "=r" (c)                /\* output *\/ */
  /*                   : "0" (a), "rm" (b)        /\* input *\/ */
  /*                   ); */
  return c;
#else
  if (a > b) { return a-b; } else { return 0; }
#endif
}
static inline spure uint64_t uint64_ssub(uint64_t a, uint64_t b)
{
  if (a > b) { return a-b; } else { return 0; }
}
/* @} */

#ifdef __cplusplus
}
#endif
