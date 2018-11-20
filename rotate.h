#pragma once

/* See: https://clang.llvm.org/docs/LanguageExtensions.html#builtin-rotateleft */

// For real use, probably use a rotate intrinsic for MSVC, or this idiom for other compilers.  This pattern of #ifdefs may be helpful
#if defined(__x86_64__) || defined(__i386__)
#ifdef __MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>  // Not just <immintrin.h> for compilers other than icc
#endif
#endif

#include "inttypes.h"

#ifdef __LP64__

inline
uint64_t rotl64(uint64_t x, unsigned n)
{
#ifdef __clang__
  const unsigned int mask = (8*sizeof(x) - 1); // assumes width is a power of 2.
  // assert ( (n<=mask) &&"rotate by type width or more");
  n &= mask;
  return (x << n) | (x >> ((-n)&mask));
#else
  return __rolq(x, n);  // GCC, ICC, MSVC. Intel-defined.
#endif
}

inline
uint64_t rotr64(uint64_t x, unsigned n) {
#ifdef __clang__
  const unsigned int mask = (8*sizeof(x) - 1);
  // assert ( (n<=mask) &&"rotate by type width or more");
  n &= mask;
  return (x >> n) | (x << ((-n)&mask));
#else
  return __rorq(x, n);  // GCC, ICC, MSVC. Intel-defined.
#endif
}

#else

#endif
