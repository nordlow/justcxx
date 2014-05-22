/*! \file binlog.h
 * \brief Binary Logarithm.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "bitwise.h"
#include "stdio_x.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Mask Out the Least Significant Bit Set in \p a. */
#undef  LSBSET
#define LSBSET(a) ((a) & -(a))

/*! Get Least Signficant Bit (LSBit) Set in \p a.
 * \return bit position on success, -1 upon error.
 */
static inline pure int int_LSBSet(int a) { return (a & (-a)); }

/*! Get Least Signficant Bit (LSBit) Set in \p a.
 * \return bit position on success, -1 upon error.
 */
static inline pure int8_t int8_LSBSet(int8_t a) { return (a & (-a)); }

/*! Get Least Signficant Bit (LSBit) Set in \p a.
 * \return bit position on success, -1 upon error.
 */
static inline pure int16_t int16_LSBSet(int16_t a) { return (a & (-a)); }

/*! Get Least Signficant Bit (LSBit) Set in \p a.
 * \return bit position on success, -1 upon error.
 */
static inline pure int32_t int32_LSBSet(int32_t a) { return (a & (-a)); }

/*! Get Least Signficant Bit (LSBit) Set in \p a.
 * \return bit position on success, -1 upon error.
 */
static inline pure int64_t int64_LSBSet(int64_t a) { return (a & (-a)); }

/*!
 * Get Bit-Position of Least Signficant Bit (LSBit) Set in \p a.
 * \return bit position on success, -1 upon error.
 */
static inline pure int int8_LSBSetPos(int8_t a)
{
  int m = int8_LSBSet(a);
  int p = -1;
  /* up to the compiler to find a smart way to optimize this */
  switch (m) {
  case (1 <<  0): p =  0; break;
  case (1 <<  1): p =  1; break;
  case (1 <<  2): p =  2; break;
  case (1 <<  3): p =  3; break;
  case (1 <<  4): p =  4; break;
  case (1 <<  5): p =  5; break;
  case (1 <<  6): p =  6; break;
  case (1 <<  7): p =  7; break;
  default: PWARN("m:%d not a binary power\n", m); break;
  }
  return p;
}

/*! Return non-zero if \p a is a Binary Power (of two), zero otherwise. */
#undef  IS_BINPOW
#define IS_BINPOW(a) ((a) && (a) == LSBSET(a))

/*! Get Logarithm of the integer \p a and round downwards.
 * log2(a)
 */
static inline pure int int_floorlog2(int a)
{
  int l;
  for (l = 0; a > 1; l++) { a >>= 1; }
  return l;
}

/*! Divide Integer \p a by the power of 2 \p b and round upwards.
 * \p a divided by 2 ^ \p b.
 */
static inline pure int int_ceildivpow2(int a, int b)
{
  return (a + (1 << b) - 1) >> b;
}

/*! Divide Integer \p a with \p b and round upwards.
 * \p a divided by \p b.
 */
static inline pure int int_ceildiv(int a, int b)
{
  return (a + b - 1) / b;
}

extern uint8_t g_int8_binlog_LUT[256];

/*! Initialize Look-Up-Table (LUT) for
 * Bit-Position of Least Signficant Bit (LSBit) Set.
 */
void int_binlog_LUT_init(void);

/*! Get Bit-Position of Least Signficant Bit (LSBit) Set in \p a using the
 * Look-Up-Table (LUT) g_int8_binlog_LUT.
 * \return bit position on success, -1 upon error.
 */
static inline pure uint8_t int8_binlog_LUT(int8_t a)
{
  return g_int8_binlog_LUT[(uint8_t)a];
}

/* \name Calculating the Nearest Power of 2:
 * http://jeffreystedfast.blogspot.com/2008/06/calculating-nearest-power-of-2.html
 */

/* @{ */

/*! Binary (Power) Ceiling of \p a.
 */
static inline pure int
int_binceil(int a)
{
  int n = a > 0 ? a - 1 : 0;
  n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16;
  n++; return n;
}

/*! Binary (Power) Ceiling of \p a.
 */
static inline pure int64_t
int64_binceil(int64_t a)
{
  int64_t n = a > 0 ? a - 1 : 0;
  n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16; n |= n >> 32;
  n++; return n;
}

/*! Binary (Power) Ceiling of \p a.
 * \TODO: Reuse this logic to implement \c binlog() in the same way.
 */
static inline pure uint32_t
uint32_binceil_x86(uint32_t a)
{
#if defined (__GNUC__) && defined (__i386__)
  int bit = 0;
  __asm__("bsrl %1,%0 \n\t"
          "jnz 1f \n\t"
          "movl $-1,%0 \n\t"
          "1:"
          : "=r" (bit) : "rm" (a));
  return (1 << (bit + 1));
#else
  return 0;
#endif
}

static inline pure int
int_binceil_alt(int a)
{
  int j, k;
  /* NOTE: Untested! */
  (j = a & 0xFFFF0000); (j = a & 0x0000FFFF);
  (k = j & 0xFF00FF00); (k = j & 0x00FF00FF);
  (j = k & 0xF0F0F0F0); (j = k & 0x0F0F0F0F);
  (k = j & 0xCCCCCCCC); (k = j & 0x33333333);
  (j = k & 0xAAAAAAAA); (j = k & 0x55555555);
  return j << 1;
}

/*! Binary (Power) Ceiling of \p a.
 */
int int_binceil_slow(int a);

/*! Binary (Power) Ceiling of \p a.
 */
int64_t int64_binceil_slow(int64_t a);

/* @} */

void test_binlog(void);

extern uint8_t g_int8_MSBPos_LUT[256];

/*! Initialize Look-Up-Table (LUT) for
 * Bit-Position of Least Signficant Bit (LSBit) Set.
 */
static inline void int_LSBSetPos_LUT_init(void)
{
  for (uint i = 0; i < 256; i++) {
    g_int8_MSBPos_LUT[i] = int8_LSBSetPos((int8_t)i);
  }
}

/*! Get Bit-Position of Least Signficant Bit (LSBit) Set in \p a using
 * the Look-Up-Table (LUT) \c g_int8_MSBPos_LUT.
 * \return bit position on success, -1 upon error.
 */
static inline pure uint8_t int8_LSBSetPos_LUT(int8_t a)
{
  return g_int8_MSBPos_LUT[(uint8_t)a];
}

/*! Get Bit-Position of Least Signficant Bit (LSBit) Set in \p a.
 * \return bit position on success, -1 upon error.
 */
static inline pure int int16_LSBSetPos(int a)
{
  int m = int16_LSBSet(a);
  int p = -1;
  /* NOTE: cup to the compiler to find a smart way to optimize this */
  switch (m) {
  case (1 <<  0): p =  0; break;
  case (1 <<  1): p =  1; break;
  case (1 <<  2): p =  2; break;
  case (1 <<  3): p =  3; break;
  case (1 <<  4): p =  4; break;
  case (1 <<  5): p =  5; break;
  case (1 <<  6): p =  6; break;
  case (1 <<  7): p =  7; break;
  case (1 <<  8): p =  8; break;
  case (1 <<  9): p =  9; break;
  case (1 << 10): p = 10; break;
  case (1 << 11): p = 11; break;
  case (1 << 12): p = 12; break;
  case (1 << 13): p = 13; break;
  case (1 << 14): p = 14; break;
  case (1 << 15): p = 15; break;
  default: PWARN("m:%d not a binary power\n", m); break;
  }
  return p;
}

/*! Get Bit-Position of Least Signficant Bit (LSBit) Set in \p a.
 * \return bit position on success, -1 upon error.
 */
static inline pure int int32_LSBSetPos(int a)
{
  int m = int32_LSBSet(a);
  int p = -1;
  /* NOTE: cup to the compiler to find a smart way to optimize this */
  switch (m) {
  case (1 <<  0): p =  0; break;
  case (1 <<  1): p =  1; break;
  case (1 <<  2): p =  2; break;
  case (1 <<  3): p =  3; break;
  case (1 <<  4): p =  4; break;
  case (1 <<  5): p =  5; break;
  case (1 <<  6): p =  6; break;
  case (1 <<  7): p =  7; break;
  case (1 <<  8): p =  8; break;
  case (1 <<  9): p =  9; break;
  case (1 << 10): p = 10; break;
  case (1 << 11): p = 11; break;
  case (1 << 12): p = 12; break;
  case (1 << 13): p = 13; break;
  case (1 << 14): p = 14; break;
  case (1 << 15): p = 15; break;
  case (1 << 16): p = 16; break;
  case (1 << 17): p = 17; break;
  case (1 << 18): p = 18; break;
  case (1 << 19): p = 19; break;
  case (1 << 20): p = 20; break;
  case (1 << 21): p = 21; break;
  case (1 << 22): p = 22; break;
  case (1 << 23): p = 23; break;
  case (1 << 24): p = 24; break;
  case (1 << 25): p = 25; break;
  case (1 << 26): p = 26; break;
  case (1 << 27): p = 27; break;
  case (1 << 28): p = 28; break;
  case (1 << 29): p = 29; break;
  case (1 << 30): p = 30; break;
  case (1 << 31): p = 31; break;
  default: PWARN("m:%d not a binary power\n", m); break;
  }
  return p;
}

/*! Get Bit-Position of Least Signficant Bit (LSBit) Set in \p a.
 * \return bit position on success, -1 upon error.
 */
static inline pure int int64_LSBSetPos(int64_t a)
{
  int64_t m = int64_LSBSet(a);
  int p = -1;
  /* NOTE: Up to the compiler to find a smart way to optimize this */
  switch (m) {
  case (1LL <<  0): p =  0; break;
  case (1LL <<  1): p =  1; break;
  case (1LL <<  2): p =  2; break;
  case (1LL <<  3): p =  3; break;
  case (1LL <<  4): p =  4; break;
  case (1LL <<  5): p =  5; break;
  case (1LL <<  6): p =  6; break;
  case (1LL <<  7): p =  7; break;
  case (1LL <<  8): p =  8; break;
  case (1LL <<  9): p =  9; break;

  case (1LL << 10): p = 10; break;
  case (1LL << 11): p = 11; break;
  case (1LL << 12): p = 12; break;
  case (1LL << 13): p = 13; break;
  case (1LL << 14): p = 14; break;
  case (1LL << 15): p = 15; break;
  case (1LL << 16): p = 16; break;
  case (1LL << 17): p = 17; break;
  case (1LL << 18): p = 18; break;
  case (1LL << 19): p = 19; break;

  case (1LL << 20): p = 20; break;
  case (1LL << 21): p = 21; break;
  case (1LL << 22): p = 22; break;
  case (1LL << 23): p = 23; break;
  case (1LL << 24): p = 24; break;
  case (1LL << 25): p = 25; break;
  case (1LL << 26): p = 26; break;
  case (1LL << 27): p = 27; break;
  case (1LL << 28): p = 28; break;
  case (1LL << 29): p = 29; break;

  case (1LL << 30): p = 30; break;
  case (1LL << 31): p = 31; break;
  case (1LL << 32): p = 32; break;
  case (1LL << 33): p = 33; break;
  case (1LL << 34): p = 34; break;
  case (1LL << 35): p = 35; break;
  case (1LL << 36): p = 36; break;
  case (1LL << 37): p = 37; break;
  case (1LL << 38): p = 38; break;
  case (1LL << 39): p = 39; break;

  case (1LL << 40): p = 40; break;
  case (1LL << 41): p = 41; break;
  case (1LL << 42): p = 42; break;
  case (1LL << 43): p = 43; break;
  case (1LL << 44): p = 44; break;
  case (1LL << 45): p = 45; break;
  case (1LL << 46): p = 46; break;
  case (1LL << 47): p = 47; break;
  case (1LL << 48): p = 48; break;
  case (1LL << 49): p = 49; break;

  case (1LL << 50): p = 50; break;
  case (1LL << 51): p = 51; break;
  case (1LL << 52): p = 52; break;
  case (1LL << 53): p = 53; break;
  case (1LL << 54): p = 54; break;
  case (1LL << 55): p = 55; break;
  case (1LL << 56): p = 56; break;
  case (1LL << 57): p = 57; break;
  case (1LL << 58): p = 58; break;
  case (1LL << 59): p = 59; break;

  case (1LL << 60): p = 60; break;
  case (1LL << 61): p = 61; break;
  case (1LL << 62): p = 62; break;
  case (1LL << 63): p = 63; break;

  default: PWARN("m:%lld not a binary power. returning %d\n",
		    (long long int)m, p); break;
  }
  return p;
}

/*! Check if \p a is an exact Binary Power.
 * \return bit position on success, -1 upon error.
 */
static inline pure int int_is_binpow(int a) { return (int_LSBSet(a) == a); }

/*! Check if \p a is an exact Binary Power.
 * \return bit position on success, -1 upon error.
 */
static inline pure int int16_is_binpow(int16_t a) { return (int16_LSBSet(a) == a); }

/*! Check if \p a is an exact Binary Power.
 * \return bit position on success, -1 upon error.
 */
static inline pure int int32_is_binpow(int32_t a) { return (int32_LSBSet(a) == a); }

/*! Check if \p a is an exact Binary Power.
 * \return bit position on success, -1 upon error.
 */
static inline pure int int64_is_binpow(int64_t a) { return (int64_LSBSet(a) == a); }

/* ---------------------------- Group Separator ---------------------------- */

/*! Binary (Power Ceiling) Logarithm of \p a.
 * \return logarithm on success, -1 upon error.
 */
static inline pure
int int_binlog(int a)
{
  /* \TODO: Can we optimize this like with \c binceil()? */
  const uint bitsize = sizeof(a) * 8;
  if (int_get_bit(a, bitsize-1)) { /* most signficant bit is set */
    if (int_is_binpow(a)) { return bitsize-1; } else { return bitsize; }
  }
  int p = 0;			/* power */
  int c = 1;			/* ceiling */
  while (c < a) { p++; c <<= 1; }
  return p;
}

/*! Binary (Power Ceiling) Logarithm of \p a.
 * \return logarithm on success, -1 upon error.
 */
static inline pure
int int64_binlog(int64_t a)
{
  /* \TODO: Can we optimize this like with \c binceil()? */
  const uint bitsize = sizeof(a) * 8;
  if (int64_get_bit(a, bitsize-1)) { /* most signficant bit is set */
    if (int64_is_binpow(a)) { return bitsize-1; } else { return bitsize; }
  }
  int p = 0;			/* power */
  int64_t c = 1;		/* ceiling */
  while (c < a) { p++; c <<= 1; }
  return p;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Some More "Power Of 2" Utility Functions
 *
 * For OpenGL texturing, I was in need of some "customized" (read: not
 * really mathematically correct but convenient) "Power Of 2" utility
 * functions, which I'd like to present here.
 *
 * For example, I wanted the functions to return 0 for an input of 0,
 * although it is 2^0 = 1 != 0, while taking care of the BSR
 * instruction's property of being undefined for an input of 0. Both
 * GNU and MSVC versions are available. These functions are part of my
 * light-weight C++ wrapper for OpenGL called GALE which will be
 * available at my homepage in some time.
 */
static inline pure int int32_is_binpow_alt(int32_t a)
{
  if (a < 1) {
    return FALSE;
  } else {
    return (a & (a - 1)) == 0;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Returns the Least Power of 2 greater than or equal to \p x.
 *
 * Note that for \p x = 0 and for \p x > 2147483648 this returns 0!
 *
 * \NOTE DOES NOT ASSEMBLE! Continue testing it in function test_binpow int
 * tests/t_printf.c
 */
#ifdef __GNUC__
static inline pure int32_t int32_binlog_x86(int32_t x)
{
  int32_t eax;
  __asm__(
	  "xor eax,eax\n"
	  "dec ecx\n"
	  "bsr ecx,ecx\n"
	  "cmovz ecx,eax\n"
	  "setnz al\n"
	  "inc eax\n"
	  "shl eax,cl\n"
	  : "=a" (eax)
	  : "c" (x)
	  );
  return eax;
}
#else
__declspec(naked) int32_t __fastcall int32_binlog_x86(int32_t x)
{
  __asm {
    xor eax,eax
      dec ecx
      bsr ecx,ecx
      cmovz ecx,eax
      setnz al
      inc eax
      shl eax,cl
      ret
      }
}
#endif

/*!
 * Returns the greatest power of 2 less than or equal to \p x.
 *
 * Note that for \p x = 0 this returns 0!
 *
 * \NOTE UNTESTED!
 */
#ifdef __GNUC__
static inline pure int32_t int32_floorBinPow(int32_t x)
{
  int32_t eax;
  __asm__(
	  "xor eax,eax\n"
	  "bsr ecx,ecx\n"
	  "setnz al\n"
	  "shl eax,cl\n"
	  : "=a" (eax)
	  : "c" (x)
	  );
  return eax;
}
#else
__declspec(naked) int32_t __fastcall int32_floorBinPow(int32_t x)
{
  __asm {
    xor eax,eax
      bsr ecx,ecx
      setnz al
      shl eax,cl
      ret
      }
}
#endif

#ifdef __cplusplus
}
#endif
