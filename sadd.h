/*! \file sadd.h
 * \brief Saturated Addition.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://groups.google.se/group/gnu.gcc.help/browse_thread/thread/12b32ed63949adde
 * \see http://www.fefe.de/intof.html
 * \see http://stackoverflow.com/questions/121240/saturating-addition-in-c
 * \see http://locklessinc.com/articles/sat_arithmetic/
 */

#pragma once
#include "pnw_types.h"
#include "clamp.h"
#include "cc_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \name Saturated Signed Addition: \p a - \p b */
/// @{

static inline spure int8_t int8_sadd_alt(int8_t a, int8_t b) {
  const int8_t c = (int8_t)(a + b);      /* sum */
  return (a < c && b < c) ? c : INT8_MAX;
}

static inline spure int8_t int8_sadd(int8_t a, int8_t b) {
  const int32_t c = (int32_t)a + b;
  return CLAMP(INT8_MIN, c, INT8_MAX);
}

static inline spure int16_t int16_sadd(int16_t a, int16_t b) {
  const int32_t c = (int32_t)a + b;
  return CLAMP(INT16_MIN, c, INT16_MAX);
}

static inline spure int32_t int32_sadd(int32_t a, int32_t b) {
  const int64_t c = (int64_t)a + b;
  return CLAMP(INT32_MIN, c, INT32_MAX);
}

static inline spure int64_t int64_sadd(int64_t a, int64_t b) {
  if (a >= 0) {
    if (b >= 0) {               /* a (P) + b (P) */
      return ((INT64_MAX - a < b) ? INT64_MAX : a + b);
    } else {                    /* a (P) + b (N) */
      return a + b;             /* no overflow possible */
    }
  } else {
    if (b >= 0) {               /* a (N) + b (P) */
      return a + b;             /* no overflow possible */
    } else {                    /* a (N) + b (N) */
      return ((a + INT64_MIN < b) ? INT64_MIN : a + b);
    }
  }
}
/// @}

/*! \name Saturated Unsigned Addition: \p a - \p b */
/// @{

static inline spure uint8_t uint8_sadd_alt(uint8_t a, uint8_t b) {
  const uint8_t c = (uint8_t)(a + b);      /* sum */
  return (a < c && b < c) ? c : UINT8_MAX;
}

static inline spure uint8_t  uint8_sadd (uint8_t a,  uint8_t b)  { return (uint8_t) ((UINT8_MAX  - a < b) ? UINT8_MAX  : a + b); }
static inline spure uint16_t uint16_sadd(uint16_t a, uint16_t b) { return (uint16_t)((UINT16_MAX - a < b) ? UINT16_MAX : a + b); }
static inline spure uint64_t uint64_sadd(uint64_t a, uint64_t b) { return (uint64_t)((UINT64_MAX - a < b) ? UINT64_MAX : a + b); }

/*! \name Saturated Unsigned Addition: \p a - \p b
  * \see http://stackoverflow.com/a/3431717/683710
  */
static inline spure uint32_t uint32_sadd(uint32_t a, uint32_t b) {
  const uint64_t s = (uint64_t)a+b;
  return -(s>>32) | (uint32_t)s;
}
static inline spure uint32_t uint32_sadd_alt(uint32_t a, uint32_t b) {
#if defined (__GNUC__) && defined (__i386__)
  if (__builtin_constant_p(a) && __builtin_constant_p(b)) { /* if both are constants (at compile time) */
    return (UINT32_MAX - a) < b ? UINT32_MAX : a + b; /* fold-optimize them */
  } else {
    uint32_t c;
    /* \see http://stackoverflow.com/a/121355/683710 */
    __asm__ volatile ("add %2, %0 \n\t" /* \c eax = \p a + \p b */
                      "cmovc %3, %0 \n\t" /* saturate if carry flag */
                      : "=r" (c)  /* %0 */
                      : "0" (a),  /* %1 */
                        "rm" (b), /* %2 */
                        "rm" (UINT32_MAX) /* %3 */
                      );
    /* Alternative Version: */
    /* __asm__ volatile ("add %2, %0 \n\t" /\* \c eax = \p a + \p b *\/ */
    /*                   "mov $0xffffffff, %%edx \n\t" */
    /*                   "cmovc %%edx, %0 \n\t" /\* if carry set \c eax to UINT32_MAX *\/ */
    /*                   : "=r" (c)                /\* output *\/ */
    /*                   : "0" (a), "rm" (b)        /\* input *\/ */
    /*                   ); */
    /* Alternative Version: */
    /* __asm__ volatile ("movl %1, %%eax \n\t" /\* \c eax = \p a *\/ */
    /*                   "addl %2, %%eax \n\t" /\* \c eax = \p a + \p b *\/ */
    /*                   "movl $0xffffffff, %%edx \n\t" */
    /*                   "cmovc %%edx, %%eax \n\t" /\* if carry set \c eax to UINT32_MAX *\/ */
    /*                   "movl %%eax, %0 \n\t"     /\* write \c eax to return value *\/ */
    /*                   : "=r" (c)                /\* output *\/ */
    /*                   : "r" (a), "r" (b)        /\* input *\/ */
    /*                   : "%eax", "%ecx"          /\* clobbered *\/ */
    /*                   ); */
    return c;
  }
#else
  return (uint32_t)((UINT32_MAX - a < b) ? UINT32_MAX : a + b);
#endif
}

/*! \name Saturated Unsigned Addition 4 uint32: \p a - \p b
 * \see http://stackoverflow.com/a/121323/683710
 */
static inline spure uint32_t uint32_sadd_4_uint8(uint32_t x, uint32_t y) {
  const uint32_t signmask = 0x80808080;
  const uint32_t t0 = (y ^ x) & signmask;
  uint32_t t1 = (y & x) & signmask;
  x &= ~signmask;
  y &= ~signmask;
  x += y;
  t1 |= t0 & x;
  t1 = (t1 << 1) - (t1 >> 7);
  return (x ^ t0) | t1;
}

/*! \name Saturated Unsigned Addition 2 uint16: \p a - \p b
 * \see http://stackoverflow.com/a/121323/683710
 */
static inline spure uint32_t uint32_sadd_2_uint16(uint32_t x, uint32_t y) {
  const uint32_t signmask = 0x80008000;
  const uint32_t t0 = (y ^ x) & signmask;
  uint32_t t1 = (y & x) & signmask;
  x &= ~signmask;
  y &= ~signmask;
  x += y;
  t1 |= t0 & x;
  t1 = (t1 << 1) - (t1 >> 15);
  return (x ^ t0) | t1;
}

/// @}

#ifdef __cplusplus
}
#endif
