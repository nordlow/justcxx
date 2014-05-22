/*! \file bitrev.h
 * \brief Bit Reversal.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see http://graphics.stanford.edu/~seander/bithacks.html
 * \see http://corner.squareup.com/2013/07/reversing-bits-on-arm.html
 * \see http://stackoverflow.com/questions/746171/best-algorithm-for-bit-reversal-from-msb-lsb-to-lsb-msb-in-c
 */

#pragma once
#include "pnw_types.h"
#include "byterev.h"
#ifdef __cplusplus
extern "C" {
#endif

/*! \name Bit Reversal. */

/* @{ */

/*! Lookup table used when reversing all the bits in a byte. */
extern uint8_t uint8array256_bitrev[256];

/*! Reverse all 8 bits in a through a table lookup and return the result. */
static inline uint8_t int8_bitrev(uint8_t a)
{
  return uint8array256_bitrev[a];
}

#define int16_bitrev(a)                           \
  (bswap_16 \
   (((b16)uint8array256_bitrev[(a & ((b16)0xff <<  0)) >>  0] <<  0) | \
    ((b16)uint8array256_bitrev[(a & ((b16)0xff <<  8)) >>  8] <<  8)))

/*! Reverse all 32 bits in \p a and return it. */
static inline int32_t int32_bitrev(int32_t a)
{
  return bswap_32
    (((int32_t)uint8array256_bitrev[(a & ((int32_t)0xff <<  0)) >>  0] <<  0) |
     ((int32_t)uint8array256_bitrev[(a & ((int32_t)0xff <<  8)) >>  8] <<  8) |
     ((int32_t)uint8array256_bitrev[(a & ((int32_t)0xff << 16)) >> 16] << 16) |
     ((int32_t)uint8array256_bitrev[(a & ((int32_t)0xff << 24)) >> 24] << 24));
}

/*! Reverse all 64 bits in \p a and return it. */
static inline int64_t int64_bitrev(int64_t a)
{
  return bswap_64
    (((int64_t)uint8array256_bitrev[(a & ((int64_t)0xff <<  0)) >>  0] <<  0) |
     ((int64_t)uint8array256_bitrev[(a & ((int64_t)0xff <<  8)) >>  8] <<  8) |
     ((int64_t)uint8array256_bitrev[(a & ((int64_t)0xff << 16)) >> 16] << 16) |
     ((int64_t)uint8array256_bitrev[(a & ((int64_t)0xff << 24)) >> 24] << 24) |
     ((int64_t)uint8array256_bitrev[(a & ((int64_t)0xff << 32)) >> 32] << 32) |
     ((int64_t)uint8array256_bitrev[(a & ((int64_t)0xff << 40)) >> 40] << 40) |
     ((int64_t)uint8array256_bitrev[(a & ((int64_t)0xff << 48)) >> 48] << 48) |
     ((int64_t)uint8array256_bitrev[(a & ((int64_t)0xff << 56)) >> 56] << 56));
}

#if (__SIZEOF_SIZE_T__ == 32)
static inline int bitrev(int a) { return int32_bitrev((int32_t) a); }
#endif

#if (__SIZEOF_SIZE_T__ == 64)
static inline int bitrev(int a) { return int64_bitrev((int64_t) a); }
#endif

/*! Slow variant of bitreversal. */
static inline int8_t int8_bitrevSlow(int8_t a)
{
  int8_t j, y;
  for (j = y = 0; j < (int)(8 * sizeof(int8_t)); j++) {
    y <<= 1;
    y |= a & 1;
    a >>= 1;
  }
  return y;
}

/*! Slow variant of bitreversal. */
static inline int int_bitrevSlow(int a)
{
  int j;
  int y;
  for (j = y = 0; j < (int)(8 * sizeof(int)); j++) {
    y <<= 1;
    y |= a & 1;
    a >>= 1;
  }
  return y;
}

/*! Initiate table for bitreversal. */
static inline void init_reverse(void)
{
  for (uint j = 0; j < 256; j++)
    uint8array256_bitrev[j] = int8_bitrevSlow(j);
}

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Reverse the first \p n Bit of \p x.
 * \return 0 when n is 0.
 */
static inline int
int_revnbits(int x, int n)
{
  int j, reverse;
  for (j = reverse = 0; j < n; j++) {
    reverse <<= 1;
    reverse |= x & 1;
    x >>= 1;
  }
  return reverse;
}

/*!
 * Returns bitswap(x) when nbits is 0.
 */
static inline int int_revnbitsAlt(int x, int nbits)
{
  return int_bitrev(x) >> (__SIZEOF_SIZE_T__ - nbits);
}

#ifdef __cplusplus
}
#endif
