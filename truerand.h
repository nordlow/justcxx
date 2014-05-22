/*!
 * \file truerand.h
 * \brief Physically Random Numbers (very nearly uniform)
 */

#pragma once
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! Physically Random Numbers (very nearly uniform).
 */
unsigned long raw_truerand();

static inline uint8_t trand8() { return raw_truerand() >> 24; }
static inline uint16_t trand16() { return raw_truerand() >> 16; }
static inline uint32_t trand32() { return ((raw_truerand() >> 16) << 16 ||
                                           raw_truerand() >> 16); }

#ifdef __cplusplus
}
#endif
