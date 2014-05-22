/*!
 * \file is_aligned.h
 * \brief Memory Alignment Queries. Used to make it simpler to define
 *        arguments to SIMD-optimized functions that require its
 *        arguments to be correctly aligned in memory.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \todo Merge with C++11 \c alignof.
 *
 * \see http://stackoverflow.com/questions/9608171/how-to-tell-gcc-that-a-pointer-argument-is-always-double-word-aligned
 * \ref __builtin_assume_aligned (GCC-4.7)
 */

#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define _MM_ALIGN2 __attribute__ ((aligned (2)))
#define _MM_ALIGN4 __attribute__ ((aligned (4)))
#define _MM_ALIGN8 __attribute__ ((aligned (8)))
#define _MM_ALIGN16 __attribute__ ((aligned (16)))
#else
#define _MM_ALIGN2
#define _MM_ALIGN4
#define _MM_ALIGN8
#define _MM_ALIGN16
#endif

/*! Wrapper to ensure 2-byte alignment of float arrays. */
typedef struct { float * dat __attribute__ ((aligned(2))); } farray_aligned2;
/*! Wrapper to ensure 4-byte alignment of float arrays. */
typedef struct { float * dat __attribute__ ((aligned(4))); } farray_aligned4;
/*! Wrapper to ensure 8-byte alignment of float arrays. */
typedef struct { float * dat __attribute__ ((aligned(8))); } farray_aligned8;
/*! Wrapper to ensure 16-byte alignment of float arrays. */
typedef struct { float * dat __attribute__ ((aligned(16))); } farray_aligned16;

/*! Wrapper to ensure 2-byte alignment of double arrays. */
typedef struct { double * dat __attribute__ ((aligned(2))); } darray_aligned2;
/*! Wrapper to ensure 4-byte alignment of double arrays. */
typedef struct { double * dat __attribute__ ((aligned(4))); } darray_aligned4;
/*! Wrapper to ensure 8-byte alignment of double arrays. */
typedef struct { double * dat __attribute__ ((aligned(8))); } darray_aligned8;
/*! Wrapper to ensure 16-byte alignment of double arrays. */
typedef struct { double * dat __attribute__ ((aligned(16))); } darray_aligned16;

/*! Check whether the pointer \p a is aligned to a 2-byte boundary.
 * \return non-zero if argument is aligned, 0 otherwise.
 */
static inline int is_aligned2(const void * a) { return ((size_t)a & 0x1) == 0; }

/*! Check whether the pointer \p a is aligned to a 4-byte boundary.
 * \return non-zero if argument is aligned, 0 otherwise.
 */
static inline int is_aligned4(const void * a) { return ((size_t)a & 0x3) == 0; }

/*! Check whether the pointer \p a is aligned to a 8-byte boundary.
 * \return non-zero if argument is aligned, 0 otherwise.
 */
static inline int is_aligned8(const void * a) { return ((size_t)a & 0x7) == 0; }

/*! Check whether the pointer \p a is aligned to a 16-byte boundary.
 * \return non-zero if argument is aligned, 0 otherwise.
 */
static inline int is_aligned16(const void * a) { return ((size_t)a & 0xf) == 0; }

#ifdef __cplusplus
}
#endif
