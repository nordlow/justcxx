/*! \file byterev.h
 * \brief Byte Reversal/Swapping.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
 */

#pragma once
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "cc_features.h"
#include "pnw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_SYS_BYTEORDER_H
#include <sys/byteorder.h>
#endif

#ifdef HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif

#ifdef _MSC_VER
#include <stdlib.h>    // or header file \c intrin.h
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)
// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(HAVE_BYTESWAP_H) || defined(__GLIBC__)
/* The GNU C Library provides: bswap_16, bswap_32, bswap_64 */
#include <byteswap.h>

/*! Override with GCC Builtins.
 * \see http://stackoverflow.com/a/105339/683710
 */
#if GCC_VERSION_AT_LEAST(4,3)
#undef bswap_32
#undef bswap_64
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)
#endif

#elif defined(bswap32)
// FreeBSD defines bswap{16,32,64} in <sys/endian.h> (already #included).
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)

#elif defined(BSWAP_64)
// Solaris 10 defines BSWAP_{16,32,64} in <sys/byteorder.h> (already #included).
#define bswap_16(x) BSWAP_16(x)
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)

#else

static inline uint16 bswap_16(uint16 x) {
  return (x << 8) | (x >> 8);
}
static inline uint32 bswap_32(uint32 x) {
  x = ((x & 0xff00ff00UL) >> 8) | ((x & 0x00ff00ffUL) << 8);
  return (x >> 16) | (x << 16);
}
static inline uint64 bswap_64(uint64 x) {
  x = ((x & 0xff00ff00ff00ff00ULL) >> 8) | ((x & 0x00ff00ff00ff00ffULL) << 8);
  x = ((x & 0xffff0000ffff0000ULL) >> 16) | ((x & 0x0000ffff0000ffffULL) << 16);
  return (x >> 32) | (x << 32);
}

/* static inline uint16_t bswap_16(uint16_t a) { return ((((uint16_t) a & ((uint16_t) 0xff << 0 * 8)) << 1 * 8) | */
/*                                                       (((uint16_t) a & ((uint16_t) 0xff << 1 * 8)) >> 1 * 8)); */
/* } */
/* static inline uint32_t bswap_32(uint32_t a) { return ((((uint32_t) a & ((uint32_t) 0xff << 0 * 8)) << 3 * 8) | */
/*                                                       (((uint32_t) a & ((uint32_t) 0xff << 1 * 8)) << 1 * 8) | */
/*                                                       (((uint32_t) a & ((uint32_t) 0xff << 2 * 8)) >> 1 * 8) | */
/*                                                       (((uint32_t) a & ((uint32_t) 0xff << 3 * 8)) >> 3 * 8)); */
/* } */
/* static inline uint64_t bswap_64(uint64_t a) { return ((((uint64_t) a & ((uint64_t) 0xff << 0 * 8)) << 7 * 8) | */
/*                                                       (((uint64_t) a & ((uint64_t) 0xff << 1 * 8)) << 5 * 8) | */
/*                                                       (((uint64_t) a & ((uint64_t) 0xff << 2 * 8)) << 3 * 8) | */
/*                                                       (((uint64_t) a & ((uint64_t) 0xff << 3 * 8)) << 1 * 8) | */
/*                                                       (((uint64_t) a & ((uint64_t) 0xff << 4 * 8)) >> 1 * 8) | */
/*                                                       (((uint64_t) a & ((uint64_t) 0xff << 5 * 8)) >> 3 * 8) | */
/*                                                       (((uint64_t) a & ((uint64_t) 0xff << 6 * 8)) >> 5 * 8) | */
/*                                                       (((uint64_t) a & ((uint64_t) 0xff << 7 * 8)) >> 7 * 8)); */
/* } */

/*! For compilers with no 64-bit support. */
static inline void bswap_88(uint8_t * a, uint8_t * b) {
  a[7] = b[0];
  a[6] = b[1];
  a[5] = b[2];
  a[4] = b[3];
  a[3] = b[4];
  a[2] = b[5];
  a[1] = b[6];
  a[0] = b[7];
}
#endif

/*! Swap Lower 16-bits with the upper 16-bits of \p a. */
static inline spure uint32_t uint32_reverseTwoU16(uint32_t a) {
  return ((((uint32_t) a & ((uint32_t) 0xffff << 0 * 16)) << 1 * 16) |
          (((uint32_t) a & ((uint32_t) 0xffff << 1 * 16)) >> 1 * 16));
}
/*! Swap Lower 32-bits with the upper 32-bits of \p a. */
static inline spure uint64_t uint64_reverseTwoU32(uint64_t a) {
  return (a >> 8 * sizeof(uint64_t) / 2) + (a << 8 * sizeof(uint64_t) / 2);
}

#ifdef __cplusplus
}
#endif
