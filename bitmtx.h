/*!
 * \file bitmtx.h
 * \brief Bit Matrix Structure.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "bitvec.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Bit Matrix.
 */
typedef struct
{
  size_t m, n;			/**< Number of rows and columns. */
  BitsBlock *d __attribute__((aligned(16))); /**< Data */
} Bitmtx;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Initialize a Bitmtx of size \p m x \p n with undefined bits.
 */
void bitmtx_init(Bitmtx * a, size_t m, size_t n);

/*!
 * Initialize a Bitmtx of size \p m x \p n with zeroed bits.
 */
void bitmtx_initZ(Bitmtx * a, size_t m, size_t n);

/*!
 * Create a new Bitmtx of size \p m x \p n with undefined bits.
 */
Bitmtx *bitmtx_new(size_t m, size_t n);

/*!
 * Create a new Bitmtx of size \p m x \p n with zeroed bits.
 */
Bitmtx *bitmtx_newZ(size_t m, size_t n);

/*!
 * Clear contents of \p a.
 */
void bitmtx_clear(Bitmtx * a);

/*!
 * Delete \p a.
 */
void bitmtx_delete(Bitmtx * a);

/*!
 * Resize a to have dimension \p m x \p n.
 */
void bitmtx_resize(Bitmtx * a, size_t m, size_t n);

/*!
 * Read bit at row \p i, column \p j and return it.
 */
uint bitmtx_atIJ(Bitmtx * a, size_t i, size_t j);

/*!
 * Clear all elements in \p a (to 0).
 */
void bitmtx_set0all(Bitmtx * a);

/*!
 * Set all elements in \p a (to 1).
 */
void bitmtx_set1all(Bitmtx * a);

/*!
 * Randomize all elements in \p a.
 */
void bitmtx_rand(Bitmtx * a);

/*!
 * Initialize \p a to be a random \em adjacency matrix.
 */
void bitmtx_randadj(Bitmtx * a);

/*!
 * Print \p a to \p stream.
 */
void bitmtx_fprint(FILE * stream, Bitmtx * a);

/*!
 * Print \p a to \c stdout.
 */
void bitmtx_print(Bitmtx * a);

/*!
 * Run a Bit Matrix test.
 */
void bitmtx_test(void);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
