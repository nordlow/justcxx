#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAVE_CONFIG_H)
#  include "config.h"
#endif

#ifdef __linux__
#  include <endian.h>           /* Note: Linux has an elegant interface. */
#  if BYTE_ORDER == LITTLE_ENDIAN
#    ifndef CONFIG_LITTLE_ENDIAN
#      define CONFIG_LITTLE_ENDIAN (1)
#    endif
#  else
#    ifndef CONFIG_BIG_ENDIAN
#      define CONFIG_BIG_ENDIAN (1)
#    endif
#  endif
#else

/* Pardon the mess, I'm trying to determine the endianness of this host.
   I'm doing it by preprocessor defines rather than some sort of configure
   script so that application code can use this too.  The "right" way would
   be to dynamically generate this file on install, but that's a lot of work.
*/
#if (defined(__i386__) || defined(__i386)) ||		\
  defined(__ia64__) || defined(WIN32) ||                  \
  (defined(__alpha__) || defined(__alpha)) ||		\
  (defined(__arm__) || defined(__thumb__)) ||		\
  (defined(__sh__) || defined(__sh64__)) ||		\
  (defined(__mips__) && defined(__MIPSEL__)) ||		\
  defined(__SYMBIAN32__) || defined(__x86_64__) ||	\
  defined(__OS2__) || defined(__LITTLE_ENDIAN__)
#  ifndef CONFIG_LITTLE_ENDIAN
#    define CONFIG_LITTLE_ENDIAN (1)
#  endif
#else
#  ifndef CONFIG_BIG_ENDIAN
#    define CONFIG_BIG_ENDIAN (1)
#  endif
#endif

#endif

/*! CPU Byteorder (Endianess). */
typedef enum {
  CPU_LE = 0,                 /**< Little Endian (LE), Least Signficant Byte First (LSB) */
  CPU_BE = 1,                  /**< Big Endian (BE), Most Signficant Byte First (MSB) */
  CPU_NATIVE = 2,              /**< Native Endian */
} ENDIAN_t;

/*! Determine Host CPU byteorder (endianness) and return it. */
ENDIAN_t get_ENDIAN(void);

#include "byterev.h"

/*! \name Endian Conversions. */
/* @{ */

#if defined(CONFIG_LITTLE_ENDIAN)
#define INT16_TOGGLE_LE(a) (a)
#define INT32_TOGGLE_LE(a) (a)
#define INT64_TOGGLE_LE(a) (a)
#define INT16_TOGGLE_BE(a) (bswap_16(a))
#define INT32_TOGGLE_BE(a) (bswap_32(a))
#define INT64_TOGGLE_BE(a) (bswap_64(a))
#endif

#if defined(CONFIG_BIG_ENDIAN)
#define INT16_TOGGLE_LE(a) (bswap_16(a))
#define INT32_TOGGLE_LE(a) (bswap_32(a))
#define INT64_TOGGLE_LE(a) (bswap_64(a))
#define INT16_TOGGLE_BE(a) (a)
#define INT32_TOGGLE_BE(a) (a)
#define INT64_TOGGLE_BE(a) (a)
#endif

/* ------------------------------------------------------------------------- */

/*!
 * Construct 16-bit Int from \em Big-Endian (Big-End First) Bytes
 * Representation from arguments \p a, \p b.
 */
static inline uint16_t uint16_cat2be(uint8_t a, uint8_t b) { return (((uint16_t)b) | ((uint16_t)a << 8)); }
/*!
 * Construct 16-bit Int from \em Little-Endian (Little-End First) Bytes
 * Representation from arguments \p a, \p b.
 */
static inline uint16_t uint16_cat2le(uint8_t a, uint8_t b) { return (((uint16_t)a) | ((uint16_t)b << 8)); }

/*!
 * Construct 32-bit Int from \em Big-Endian (Big-End First) Bytes
 * Representation from arguments \p a, \p b, \p c.
 */
static inline uint32_t uint32_cat3be(uint8_t a, uint8_t b, uint8_t c) { return (((uint32_t)c) | ((uint32_t)b << 8) | ((uint32_t)a << 16)); }
/*!
 * Construct 32-bit Int from \em Little-Endian (Little-End First) Bytes
 * Representation from arguments \p a, \p b, \p c.
 */
static inline uint32_t uint32_cat3le(uint8_t a, uint8_t b, uint8_t c) { return (((uint32_t)a) | ((uint32_t)b << 8) | ((uint32_t)c << 16)); }

/*!
 * Construct 32-bit Int from \em Big-Endian (Big-End First) Bytes
 * Representation from arguments \p a, \p b, \p c, \p d.
 */
static inline uint32_t uint32_cat4be(uint8_t a, uint8_t b, uint8_t c, uint8_t d) { return (((uint32_t)d) | ((uint32_t)c << 8) | ((uint32_t)b << 16) | ((uint32_t)a << 24)); }
/*!
 * Construct 32-bit Int from \em Little-Endian (Little-End First) Bytes
 * Representation from arguments \p a, \p b, \p c, \p d.
 */
static inline uint32_t uint32_cat4le(uint8_t a, uint8_t b, uint8_t c, uint8_t d) { return (((uint32_t)a) | ((uint32_t)b << 8) | ((uint32_t)c << 16) | ((uint32_t)d << 24)); }

/*!
 * Construct 64-bit Int from \em Big-Endian (Big-End First) Bytes
 * Representation from arguments \p a, \p b, \p c, \p d, \p e, \p f, \p g, \p h.
 */
static inline uint64_t uint64_cat8be(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h) {
  return (((uint64_t)h <<  0) | ((uint64_t)g  << 8) | ((uint64_t)f << 16) | ((uint64_t)e << 24) |
          ((uint64_t)d << 32) | ((uint64_t)c << 40) | ((uint64_t)b << 48) | ((uint64_t)a << 56)); }

static inline uint64_t uint64_cat8le(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h) {
  return (((uint64_t)a <<  0) | ((uint64_t)b <<  8) | ((uint64_t)c << 16) | ((uint64_t)d << 24) |
          ((uint64_t)e << 32) | ((uint64_t)f << 40) | ((uint64_t)g << 48) | ((uint64_t)h << 56)); }

#ifdef __cplusplus
}
#endif
