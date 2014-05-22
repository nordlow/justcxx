/*! \file bitwise.h
 * \brief Bitwise operations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see http://graphics.stanford.edu/~seander/bithacks.html
 * \see http://corner.squareup.com/2013/07/reversing-bits-on-arm.html
 * \see http://stackoverflow.com/questions/746171/best-algorithm-for-bit-reversal-from-msb-lsb-to-lsb-msb-in-c
 * \see http://www.ibm.com/developerworks/library/l-port64.html
 * \todo Implement http://bmagic.sourceforge.net/articles.html
 */

#pragma once
#include "utils.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void bitcounts_init(void);

/*! Set the \p i:th bit in \p a. */
#define SET_BIT(a, i) ((a) |= (1 << (i)))

/*! Set the \p i:th and \p j:th bit in \p a. */
#define SET_BITS2(a, i, j) ((a) |= (1 << (i) |	\
                                    1 << (j)))

/*! Set the \p i:th, \p j:th, k:th and l:th bit in \p a. */
#define SET_BITS4(a, i, j, k, l) ((a) |= (1 << (i) |	\
                                          1 << (j) |	\
                                          1 << (k) |	\
                                          1 << (l)))

/*! Clear the \p i:th bit in \p a. */
#define CLR_BIT(a, i) ((a) &= ~(1 << (i)));

/*! Return 1 if the \p i:th bit in a is set, 0 otherwise. */
#define GET_BIT(a, i) (((a) & (1 << (i))) != 0)

/*! Return non-zero if both the \p i:th bit and the \p j:th bit is set. */
#define HAS_BITS2(a, i, j) (GET_BIT(a, i) && GET_BIT(a, j))

/*! Set the \p i:th bit in \p a. */
static inline spure int int_set_bit(int a, uint i) { return (a | (1 << i)); }
/*! Clear the \p i:th bit in \p a. */
static inline spure int int_clr_bit(int a, uint i) { return (a & ~(1 << i)); }
/*! Return 1 if the \p i:th bit in a is set, 0 otherwise. */
static inline spure bool int_get_bit(int a, uint i) { return (a & (1 << i)) != 0; }
/*! Set the \p i:th and \p j:th bit of \p a to 1 if \p x is non-zero or to 0 if
 *  \p x is zero. */
static inline spure int int_set4bits(int a, uint i, uint j, uint k, uint l) { return (a | (1 << i) | (1 << j) | (1 << k) | (1 << l)); }

/*! Set the \p i:th bit in \p a. */
static inline spure int8_t int8_set_bit(int8_t a, uint i) { return (a | (1 << i)); }
/*! Clear the \p i:th bit in \p a. */
static inline spure int8_t int8_clr_bit(int8_t a, uint i) { return (a & ~(1 << i)); }
/*! Return 1 if the \p i:th bit in a is set, 0 otherwise. */
static inline spure bool int8_get_bit(int8_t a, uint i) { return (a & (1 << i)) != 0LL; }
/*! Set the \p i:th and \p j:th bit of \p a to 1 if \p a is non-zero or to 0 if
 *  \p a is zero. */
static inline spure int8_t int8_set2Bits(int8_t a, uint i, uint j) { return (a | (1 << i) | (1 << j)); }
/*! Set the \p i:th and \p j:th bit of \p a to 1 if \p a is non-zero or to 0 if
 *  \p a is zero. */
static inline spure int8_t int8_set4Bits(int8_t a, uint i, uint j, uint k, uint l) { return (a | (1 << i) | (1 << j) | (1 << k) | (1 << l)); }
/*! Clear the \p i:th and \p j:th bit of \p a to 1 if \p a is non-zero or to 0
 *  if \p a is zero. */
static inline spure int8_t int8_clear2Bits(int8_t a, uint i, uint j) { return (a & ~(1 << i) & ~(1 << j)); }
/*! Clear the \p i:th and \p j:th bit of \p a to 1 if \p a is non-zero or to 0
 *  if \p a is zero. */
static inline spure int8_t int8_clear4Bits(int8_t a, uint i, uint j, uint k, uint l) { return (a & ~(1 << i) & ~(1 << j) & ~(1 << k) & ~(1 << l)); }
/*! Set the \p i:th bit of \p a to 1 if \p b is non-zero or to 0 if \p x is
 *  zero. */
static inline spure int8_t int8_set_bitTo(int8_t a, uint i, int b) { return (b ? int8_set_bit(a, i) : int8_clr_bit(a, i)); }
/*! Set the \p i:th bit of \p a to 1 if \p b is non-zero or to 0 if \p b is
 *  zero. */
static inline spure int8_t int8_set2BitsTo(int8_t a, uint i, uint j, int b)
{
  a = int8_set_bitTo(a, i, b);
  a = int8_set_bitTo(a, j, b);
  return a;
}

/*! Set the \p i:th bit of \p a to 1 if \p b is non-zero or to 0 if \p b is
 *  zero. */
static inline spure int8_t int8_set4BitsTo(int8_t a, uint i, uint j, uint k, uint l,
                                                            int b)
{
  a = int8_set2BitsTo(a, i, j, b);
  a = int8_set2BitsTo(a, k, l, b);
  return a;
}

/*! Set the \p i:th bit in \p a. */
static inline spure int32_t int32_set_bit(int32_t a, uint i) { return (a | (1 << i)); }
/*! Clear the \p i:th bit in \p a. */
static inline spure int32_t int32_clr_bit(int32_t a, uint i) { return (a & ~(1 << i)); }
/*! Return 1 if the \p i:th bit in a is set, 0 otherwise. */
static inline spure bool int32_get_bit(int32_t a, uint i) { return (a & (1 << i)) != 0LL; }

/*! Set the \p i:th bit in \p a. */
static inline spure int64_t int64_set_bit(int64_t a, uint i) { return (a | (1LL << i)); }
/*! Clear the \p i:th bit in \p a. */
static inline spure int64_t int64_clr_bit(int64_t a, uint i) { return (a & ~(1LL << i)); }
/*! Return 1 if the \p i:th bit in a is set, 0 otherwise. */
static inline spure bool int64_get_bit(int64_t a, uint i) { return (a & (1LL << i)) != 0LL; }

/*! \name Returns number of bits set in each argument.
 * \see SSE4.2 instruction \c POPCNT (http://en.wikipedia.org/wiki/SSE4#SSE4.2).
 */

/* @{ */

/*! Get Number of Bits that are set to 1 in \p a (Parallel version). */
static inline spure int uint32_countBits_parallel(uint32_t a)
{
  a = (a & 0x55555555) + (a >> 1 & 0x55555555);
  a = (a & 0x33333333) + (a >> 2 & 0x33333333);
  a = (a + (a >> 4)) & 0x0F0F0F0F;
  a = a + (a >> 8);
  a = (a + (a >> 16)) & 0x0000003F;
  return (int) a;
}

/*! Get Number of Bits that are set to 1 in \p a (Parallel version).
 * \see http://bmagic.sourceforge.net/bm64opt.html function \c count().
 */
static inline spure int uint64_countBits_parallel(uint64_t a)
{
  a = (a & 0x5555555555555555ULL) + (a >> 1 & 0x5555555555555555ULL);
  a = (a & 0x3333333333333333ULL) + (a >> 2 & 0x3333333333333333ULL);
  a = (a + (a >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  a = a + (a >> 8);
  a = a + (a >> 16);
  a = (a + (a >> 32)) & 0x0000007F;
  return (int) a;
}

/*! Get Number of Bits that are set to 1 among \p n first bits in \p a (Serial
 *  loop version). */
static inline spure int int_countBitsB_serial(int a, size_t n)
{
  int cnt = 0;
  for (size_t j = 0; j < n; j++) {
    if (a & 1)
      cnt++;
    a >>= 1;
  }
  return cnt;
}

/*! Get Number of Bits that are set to 1 in \p a (Serial loop version). */
static inline spure int uint32_countBits_serial(int a)
{
  int cnt = 0;
  for (size_t j = 0; j < 8 * sizeof(a); j++) {
    cnt += (a >> j) & 1;
  }
  return cnt;
}

/*! Get Number of Bits that are set to 1 in \p a (Serial loop version). */
static inline spure int uint64_countBits_serial(uint64_t a)
{
  int cnt = 0;
  for (size_t j = 0; j < 8 * sizeof(a); j++) {
    cnt += (a >> j) & 1;
  }
  return cnt;
}

extern uint8_t g_bitcounts_8[256];
extern uint8_t g_bitcounts_16[65536];

static inline pure int
uint8_countBits(uint8_t a)
{
  return g_bitcounts_8[(a & (0xff << 0)) >> 0];
}

static inline pure int
uint16_countBits(uint16_t a)
{
#if 1
  return g_bitcounts_16[a];
#else
  return (g_bitcounts_8[(a & (0xff << 0)) >> 0] +
          g_bitcounts_8[(a & (0xff << 8)) >> 8]);
#endif
}

static inline pure int
uint32_countBits(uint32_t a)
{
#if 1
  return (g_bitcounts_16[(a & (0xffff <<  0)) >>  0] +
          g_bitcounts_16[(a & (0xffff << 16)) >> 16]);
#else
  return (g_bitcounts_8[(a & (0xff <<  0)) >>  0] +
          g_bitcounts_8[(a & (0xff <<  8)) >>  8] +
          g_bitcounts_8[(a & (0xff << 16)) >> 16] +
          g_bitcounts_8[(a & (0xff << 24)) >> 24]);
#endif
}

static inline pure int
uint64_countBits(uint64_t a)
{
  return (g_bitcounts_16[(a & (0xffffULL <<  0)) >> 0] +
          g_bitcounts_16[(a & (0xffffULL << 16)) >> 16] +
          g_bitcounts_16[(a & (0xffffULL << 32)) >> 32] +
          g_bitcounts_16[(a & (0xffffULL << 48)) >> 48]);
}

/* @} */

/*! \em Conversions between machines with different byte orders. Hints in glib.h. */

/* Target independent macros.
   Three separate conversion functions per bit-size (\binomial{3}{2}). */

#define int16_toggleLittleBigEndian(a) (bswap_16(a))
#define int16_toggleBigPDPEndian(a) (bswap_16(a))
#define int16_togglePDPLittleEndian(a) (a)

#define int32_toggleLittleBigEndian(a) (bswap_32(a))
/* FIXME: Alternatively defined using ... byteReverse. */
#define int32_toggleBigPDPEndian(a)             \
  ((((a) & (0xff << 0 * 8)) << 1 * 8) |         \
   (((a) & (0xff << 1 * 8)) >> 1 * 8) |         \
   (((a) & (0xff << 2 * 8)) << 1 * 8) |         \
   (((a) & (0xff << 3 * 8)) >> 1 * 8))
/* FIXME: Alternatively defined using ..._byterev. */
#define int32_togglePDPLittleEndian(a)          \
  ((((a) & (0xff << 0 * 8)) << 2 * 8) |         \
   (((a) & (0xff << 1 * 8)) << 2 * 8) |         \
   (((a) & (0xff << 2 * 8)) >> 2 * 8) |         \
   (((a) & (0xff << 3 * 8)) >> 2 * 8))

#define int64_toggleLittleBigEndian(a) (bswap_64(a))
#define int64_toggleBigPDPEndian(a)    (a) /* FIXME: ? */
#define int64_togglePDPLittleEndian(a) (a) /* FIXME: ? */

/* Target dependent macros. */

#if defined(CONFIG_LITTLE_ENDIAN)
#define int16_toggleLittleEndian(a) (a)
#define int16_toggleBigEndian(a) (int16_toggleLittleBigEndian(a))
#define int16_togglePDPEndian(a) (int16_togglePDPLittleEndian(a))
#define int32_toggleLittleEndian(a) (a)
#define int32_toggleBigEndian(a) (int32_toggleLittleBigEndian(a))
#define int32_togglePDPEndian(a) (int32_togglePDPLittleEndian(a))
#define int64_toggleLittleEndian(a) (a)
#define int64_toggleBigEndian(a) (int64_toggleLittleBigEndian(a))
#define int64_togglePDPEndian(a) (int64_togglePDPLittleEndian(a))
#elif defined(CONFIG_BIG_ENDIAN)
#define int16_toggleLittleEndian(a) (int16_toggleLittleBigEndian(a))
#define int16_toggleBigEndian(a) (a)
#define int16_togglePDPEndian(a) (int16_toggleBigPDPEndian(a))
#define int32_toggleLittleEndian(a) (int32_toggleLittleBigEndian(a))
#define int32_toggleBigEndian(a) (a)
#define int32_togglePDPEndian(a) (int32_toggleBigPDPEndian(a))
#define int64_toggleLittleEndian(a) (int64_toggleLittleBigEndian(a))
#define int64_toggleBigEndian(a) (a)
#define int64_togglePDPEndian(a) (int64_toggleBigPDPEndian(a))
#elif defined(CONFIG_PDP_ENDIAN)
#define int16_toggleLittleEndian(a) (int16_togglePDPLittleEndian(a))
#define int16_toggleBigEndian(a) (int16_toggleBigPDPEndian(a))
#define int16_togglePDPEndian(a) (a)
#define int32_toggleLittleEndian(a) (int32_togglePDPLittleEndian(a))
#define int32_toggleBigEndian(a) (int32_toggleBigPDPEndian(a))
#define int32_togglePDPEndian(a) (a)
#define int64_toggleLittleEndian(a) (int64_togglePDPLittleEndian(a))
#define int64_toggleBigEndian(a) (int64_toggleBigPDPEndian(a))
#define int64_togglePDPEndian(a) (a)
#else
/* FIXME: Are there any more? */
#error Sorry, byte order conversion not supported on this target.
#endif

void b16v_toggleLittleEndian(uint16_t * out, const uint16_t * in, int n);
void b16v_toggleBigEndian(uint16_t * out, const uint16_t * in, int n);
void b16v_togglePDPEndian(uint16_t * out, const uint16_t * in, int n);

void b32v_toggleLittleEndian(uint32_t * out, const uint32_t * in, int n);
void b32v_toggleBigEndian(uint32_t * out, const uint32_t * in, int n);
void b32v_togglePDPEndian(uint32_t * out, const uint32_t * in, int n);

/*! \name Host-Network and Network Conversions. */
/* @{ */
#define uint16_toNetwork(a) htons(a)
#define int32_toNetwork(a) htonl(a)
#define int16_fromNetwork(a) ntohs(a)
#define int32_fromNetwork(a) ntohl(a)
/* @} */

/*! Get number of bits needed to store the value \p power_of_two. */
pure int int_bits_needed(int power_of_two);

/*! Rotate \p x Left by \p n bits. */
int32_t inline spure int32_bit_rotl(int32_t x, int8_t n) { return (x << n) | (x >> (32 - n)); }
#define rotl int32_bit_rotl     /**< Alias */
/*! Rotate \p x Left by \p n bits. */
int64_t inline spure int64_bit_rotl(int64_t x, int8_t n) { return (x << n) | (x >> (64 - n)); }
#define rotl64 int64_bit_rotl   /**< Alias */
/*! Rotate \p x Right by \p n bits. */
int32_t inline spure int32_bit_rotr(int32_t x, int8_t n) { return (x >> n) | (x << (32 - n)); }
#define rotr int32_bit_rotr     /**< Alias */
/*! Rotate \p x Right by \p n bits. */
int64_t inline spure int64_bit_rotr(int64_t x, int8_t n) { return (x >> n) | (x << (64 - n)); }
#define rotr64 int64_bit_rotr     /**< Alias */

/*!
 * \todo This alternative is NOT faster on my machine also for uint32_t.
 * Does anyone understand why?
 */
#if defined (__GNUC__) && __GNUC__ >= 2 && defined (__i386__)
inline spure uint32_t uint32_bit_rotl(uint32_t a, uint n)
{
  __asm__ volatile ("roll %%cl, %0"
                    : "=r" (a)
                    : "0" (a), "c" (n));
  return a;
}
#endif

#if defined (__GNUC__) && __GNUC__ >= 2 && defined (__i386__)
inline spure uint32_t uint32_bit_rotr(uint32_t a, uint n)
{
  __asm__ volatile ("rorl %%cl, %0"
                    : "=r" (a)
                    : "0" (a), "c" (n));
  return a;
}
#endif

#ifdef __cplusplus
}
#endif
