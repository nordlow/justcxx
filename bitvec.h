/*! \file bitvec.h
 * \brief Bit Vector Structure.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#include "limits.h"
#include "rangerand.h"
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Bit Vector Block (Granularity). */
typedef uint64_t BitsBlock;
/*! Bit Vector all \em Zeros Constant. */
#define BITVEC_BLOCK_ZEROS UINT64_MIN
/*! Bit Vector all \em Ones Constant. */
#define BITVEC_BLOCK_ONES UINT64_MAX
/*! Bit Vector Block \em Byte \em Size. */
#define BITVEC_BLOCK_BYTESIZE (sizeof(BitsBlock))
/*! Bit Vector Block \em Bit \em Size. */
#define BITVEC_BLOCK_BITSIZE (8 * sizeof(BitsBlock))

/* ---------------------------- Group Separator ---------------------------- */

static inline void bitsblock_set1(BitsBlock * a, size_t i)
{
  const size_t q = i / BITVEC_BLOCK_BITSIZE, r = i % BITVEC_BLOCK_BITSIZE;
  a[q] |= ((BitsBlock)1 << r);
}

/*! Randomizer.
 * \return a random Bit Vector Block.
 */
static inline BitsBlock bitsblock_rand(void) { return int64_rand(); }

/*! Random Uniform Crossover of Bit Vector Block \p a and \p b.
 *
 * Randomly mix the bits in \p a and \p b similar to the way
 * DNA-strings merge in natural mating.
 *
 * \see http://en.wikipedia.org/wiki/Crossover_(genetic_algorithm)
 * \return the mixed block.
 */
static inline BitsBlock bitsblock_uniform_crossover(BitsBlock a, BitsBlock b)
{
  BitsBlock r = bitsblock_rand();
  return ((a ^ b) & r) | (a & b);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Bit Vector. */
typedef struct Bitvec
{
  size_t l;			/**< Length of bitvector in bits. */
  /*! Bitvector data.
   * \em Aligned for auto-vectorization.
   */
  BitsBlock *d __attribute__ ((aligned(16)));
} Bitvec;

/* ---------------------------- Group Separator ---------------------------- */

/*! Get internal size of \p l in number of internal blocks (of
 * bit-length \c BITVEC_BLOCK_BITSIZE).
 */
static inline size_t bitvec_get_data_blocknum(size_t l) {
  if (l == 0) {
    return 0;
  } else {
    const size_t q = l / BITVEC_BLOCK_BITSIZE, r = l % BITVEC_BLOCK_BITSIZE;
    return (q + (r > 0 ? 1 : 0));
  }
}

/*! Get internal size of \p l in number of bytes. */
static inline size_t bitvec_get_data_bytesize(size_t l) {
  return bitvec_get_data_blocknum(l) * BITVEC_BLOCK_BYTESIZE;
}

static inline void bitvec_boundscheck(const Bitvec * a, size_t i) {
#ifdef DEBUG_CHECK_INDEX
  if (i >= a->l) { PERR("index:%zd >= bitvector length %zd\n", i, a->l); }
#endif
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Set bit \p i in \p a (to 1). */
static inline void bitvec_set1(Bitvec * a, size_t i) {
  bitvec_boundscheck(a, i);
  const size_t q = i / BITVEC_BLOCK_BITSIZE, r = i % BITVEC_BLOCK_BITSIZE;
  a->d[q] |= ((BitsBlock)1 << r);
}

/*! Clear bit \p i in \p a (to 0).  */
static inline void bitvec_set0(Bitvec * a, size_t i)
{
  bitvec_boundscheck(a, i);
  const size_t q = i / BITVEC_BLOCK_BITSIZE, r = i % BITVEC_BLOCK_BITSIZE;
  a->d[q] &= ~((BitsBlock)1 << r);
}

/*! Flip/Toggle bit \p i in \p a.
 * \return the new value of the flipped bit.
 */
static inline uint bitvec_toggle(Bitvec * a, size_t i)
{
  bitvec_boundscheck(a, i);
  const size_t q = i / BITVEC_BLOCK_BITSIZE, r = i % BITVEC_BLOCK_BITSIZE;
  return ((a->d[q] ^= ((BitsBlock)1 << r)) != (BitsBlock)0);
}

/*! Put bit \p i in \p a to \p bt. */
static inline void bitvec_set(Bitvec * a, size_t i, uint bt)
{
  bitvec_boundscheck(a, i);
  bt ? bitvec_set1(a, i) : bitvec_set0(a, i);
}

/*! Get bit \p i in \p a and return it. */
static inline uint bitsblock_get(const BitsBlock * a, size_t i)
{
  const size_t q = i / BITVEC_BLOCK_BITSIZE, r = i % BITVEC_BLOCK_BITSIZE;
  return ((a[q] & ((BitsBlock)1 << r)) != (BitsBlock)0);
}

/*! Get bit \p i in \p a and return it. */
static inline uint bitvec_get(const Bitvec * a, size_t i)
{
  bitvec_boundscheck(a, i);
  return bitsblock_get(a->d, i);
}

/*! Copy bit \p bi in \p b to bit \p ai in \p a. */
static inline void bitvec_copybit(Bitvec * a, size_t ai, const Bitvec * b, size_t bi)
{
  bitvec_boundscheck(a, ai);
  bitvec_boundscheck(b, bi);
  bitvec_set(a, ai, bitvec_get(b, bi));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! \name Predicates. */
/* @{ */
/*! Check if \em no bit is set (only 0) in \p a. */
bool bitvec_none1(const Bitvec * a);
/*! Check if \em all bits are set (only 1) in \p a. */
bool bitvec_all1(const Bitvec * a);
/*! Check if \em any bit is set (at least one 1) in \p a. */
static inline bool bitvec_any1(const Bitvec * a) { return !bitvec_none1(a); }
/* @} */

/*! Randomize all bits in \p a. */
void bitvec_rand(Bitvec * a);
/*! Set all bits in \p a (to 1). */
void bitvec_set1all(Bitvec * a);
/*! Clear all bits in \p a (to 0). */
void bitvec_set0all(Bitvec * a);

/*! Init \p b with \p l number of \em undefined bits. */
static inline void bitvec_init(Bitvec * b, size_t l) {
  if (l) {
    b->d = (BitsBlock*)malloc_aligned(16, bitvec_get_data_bytesize(l));
  } else {
    b->d = NULL;
  }
  b->l = l;
}

/*! Init \p a with \p l number of \em zeroed bits. */
static inline void bitvec_initZ(Bitvec * b, size_t l) {
  if (l) {
    b->d = (BitsBlock*)calloc_aligned(16, bitvec_get_data_bytesize(l));
  } else {
    b->d = NULL;
  }
  b->l = l;
}

/*! Init \p b with \p bytenum number of \em defined bits contained in \p bytes. */
static inline void bitvec_initbytes(Bitvec * b, size_t bytenum, const char * bytes) {
  bitvec_initZ(b, 8 * bytenum);
  memcpy(b->d, bytes, bytenum);
}

/*! Init \p b with \p bitnum number of \em defined bits contained in @p bits. */
static inline void bitvec_initbits(Bitvec * b, size_t bitnum, const char * bits) {
  bitvec_initZ(b, bitnum);
  memcpy(b->d, bits,
         bitnum / 8 +
         ((bitnum % 8) ? 1 : 0));
}

/*! Create a new Bitvec with \p l number of \p em undefined bits. */
static inline Bitvec * bitvec_new(size_t l) {
  Bitvec * btv = (Bitvec*)malloc(sizeof(Bitvec));
  bitvec_init(btv, l);
  return btv;
}

/*! Create a new Bitvec with \p l number of \em zeroed bits. */
static inline Bitvec * bitvec_newZ(size_t l)
{
  Bitvec * btv = (Bitvec*)malloc(sizeof(Bitvec));
  bitvec_initZ(btv, l);
  return btv;
}

/*! Clear contents of \em a. */
static inline void bitvec_clear(Bitvec * b) {
  if (b->l) { free(b->d); b->d = NULL; }
}

/*! Delete \em a. */
static inline void bitvec_delete(Bitvec * a)
{
  bitvec_clear(a);
  free(a);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Reinitialize \p a to contain \p l \em undefined bits. */
static inline void bitvec_reinit(Bitvec * b, size_t l) {
  if (b->l != l) {
    bitvec_clear(b);
    bitvec_init(b, l);
  }
}

/*! Reinitialize \p a to contain \p l \em zeroed bits. */
static inline void
bitvec_zreinit(Bitvec * b, size_t l)
{
  if (b->l != l) {
    bitvec_clear(b);
    bitvec_initZ(b, l);
  } else {
    bitvec_set1all(b);
  }
}

void bitvec_parsestr(Bitvec * b, const char *str);

/* ---------------------------- Group Separator ---------------------------- */

/*! Copy \p b to \p a. */
void bitvec_copyall(Bitvec * a, const Bitvec * b);

/*! Append \p b at end of \p a. */
void bitvec_append(Bitvec * a, const Bitvec * b);

/* ---------------------------- Group Separator ---------------------------- */

/*! Bitwise AND \p b with \p c and put result in \p a. */
void bitvec_and(Bitvec * a, const Bitvec * b, const Bitvec * c);

/*! Bitwise OR \p b with \p c and put result in \p a. */
void bitvec_or(Bitvec * a, const Bitvec * b, const Bitvec * c);

/*! Bitwise XOR \p b with \p c and put result in \p a. */
void bitvec_xor(Bitvec * a, const Bitvec * b, const Bitvec * c);

/*! Bitwise NOT \p b and put result in \p a. */
void bitvec_not(Bitvec * a, const Bitvec * b);

/* ---------------------------- Group Separator ---------------------------- */

/*! Bitwise Reverse \p b and put result in \p a. */
void bitvec_rev(Bitvec * a, const Bitvec * b);

/*! Perform a Uniform Crossover of \p b and \p c and put result in \p a.
 *
 * \see http://en.wikipedia.org/wiki/Crossover_(genetic_algorithm)
 */
void bitvec_uniform_crossover(Bitvec * a, const Bitvec * b, const Bitvec * c);

/*! Mutate (randomly flip) the first \p n bits in \p a. */
void bitvec_mutate(Bitvec * a, size_t n);

/* ---------------------------- Group Separator ---------------------------- */

/*! Print a to \p stream. */
void bitvec_fprint(FILE * stream, const Bitvec * b);

/*! Print a to \c stdout. */
static inline void
bitvec_print(const Bitvec * b)
{
  bitvec_fprint(stdout, b);
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
