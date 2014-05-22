/*! \file reorg.h
 * \brief Reorganize Functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "utils.h"
#ifdef __cplusplus
extern "C" {
#endif

/*! Flip */
void intarray_flip(int *a, size_t n);
/*! Flip */
void sizearray_flip(size_t *a, size_t n);
/*! Flip */
void darray_flip(double *a, size_t n);
/*! Random Sub Flip */
void intarray_rsflip(int *a, size_t n);
/*! Cyclic Random Sub Flip */
void intarray_crsflip(int *a, size_t n);

/*! \em Cyclic Random Sub Swap */
void intarray_crsswap(int *a, size_t n);

/*! Shuffle \p a of length \p n */
void intarray_shuffle(int *a, size_t n);
/*! \em Partial \em Shuffle \p a of length \p n */
void intarray_partial_shuffle(int *a, size_t n, size_t nswaps);

/*! Generate a Random Permutation
 * Out: Writes a random permutation of [0 ... n-1] of length \p n into \p a.
 */
void intarray_random_permutation(int *a, size_t n);

/*!
 * Generate a Fast (Pseudo) Random Permutation (fast)
 * Out: Writes a random permutation of [0 ... n-1] of length \p n into \p a.
 */
void intarray_fast_random_permutation(int *a, size_t n);

/*! Check if argument is a permutation.
 * In: a is an integer vector of length \p n.
 * \return \c true if \p a is a permutation of [0 ... \p n-1], \c false otherwise.
 */
int intarray_is_permutation(const int *a, size_t n);

/*! Check if argument is a permutation.
 * In: a is an integer vector of length \p n.
 * \return \c true if \p a is a permutation of [0 ... \p n-1], \c false otherwise.
 */
int sizearray_is_permutation(const size_t *a, size_t n);

/*!
 * Quad-Tree Indexing (in 2 Dimensions).
 * In:
 * - x and y are the 2D-coordinate arrays, both of length \p n.
 * - idx is a permutation of [0 ... n-1], indicating the current ordering
 *   of x and y.
 * Out: Replaces idx, x and y with their reordered variants.
 * \complexity O(n*(log(n))^2).
 */
void darray_qtidx(size_t *idx, const double *x, const double *y, size_t n);
void test_qtidx8(void);
void test_qtidx(size_t n, int show);
void test_qtidxLoop(void);

/*!
 * Remainder Stochastic Sampling (RSS) most efficiently implemented
 * using Stochastic Uniform Sampling (SUS).
 *
 * \see (p. 32 A Genetic Algorithm Tutorial, Darrell Whitley).
 *
 * In: idxnum, fits, fits_sum
 * Out: idx
 * \return -1 if fits_sum is smaller than the sum of the fits.
 */
int darray_sus(int *idx, int idxnum, const double *fits, int fits_sum);

#ifdef __cplusplus
}
#endif
