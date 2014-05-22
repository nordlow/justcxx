/*!
 * \file sparse.h
 * \brief Compressed Sparse Row (CSR) format definition and related functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Compressed Sparse Row (CSR) format of spare matrices.
 *
 * - Influenced by data structures found in GNU MP's.

 * - IMPORTANT:
 *   - w, h, i, length --- must be set (by initializators) before operations
 *                         can be performed
 *
 *   - j, a            --- are dynamically csr_reserved when needed.
 *   - length          --- can also be changed.
 *
 * - Perhaps value (v), index (i), pointer (p) are better than a, j, i?
 * - All algorithms assume that all stored values are non-zero.
 *
 * - For efficiency reasons, avoid initializing and clearing out a sparse
 *   variable in a loop.  Instead, initialize it before entering the loop,
 *   and clear it out after the loop has exited.
 *
 * - NOTE: offs(rows), cols, vals perhaps more descriptive than i, j, a?
 */
typedef struct
{
  int w;			/**< Width. */
  int h;			/**< Height. */
  int *i;			/**< Indexes to row-starts in a. */
  int length;			/**< Allocation length of j and a buffers. */
  int *j;			/**< Column indexes and */
  double *a;		/**< their corresponding non-zero elements. */
} __CSRd_struct;

typedef __CSRd_struct CSRd[1];

/*!
 * \name Macros.
 */

/* @{ */

/*! Get the number of non-zero elements in matrix. */
#define CSRd_nnz(in) ((in)->i[(in)->h])

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name (Re) allocators.
 */

/* @{ */

void CSRd_reserve(CSRd out, const int new_len);

void CSRd_compress(CSRd out);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Initializators.
 * Sets w, h, allocates i, and sets length.
 */

/* @{ */

void CSRd_init_zeros(CSRd out, int w, int h);

/* Warning: Be sure that you have understood the underlying storage technique
 * of CSRd before you use this function. */
void CSRd_init_from_arrays(CSRd out, int w, int h,
			   const int *i, const int *j, const double *a);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Generators.
 */

/* @{ */

void CSRd_zeros(CSRd out);
void CSRd_ones(CSRd out);
void CSRd_ramp(CSRd out, double start, double step);
void CSRd_eye(CSRd out);
void CSRd_rand(CSRd out);

/*!
 * Generates a matrix out with the same sparsity
 * structure as in, but with ones in the nonzero positions.
 * Same as spones in matlab.
 */
void CSRd_sparse_ones(CSRd out, const CSRd in);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Clearers.
 */

/* @{ */

void CSRd_clear(CSRd out);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Copying.
 */

/* @{ */

void CSRd_copy(CSRd out, const CSRd in);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Conversion.
 */

/* @{ */

void dmatrix_from_dcsr(double **out, const CSRd in);
void CSRd_from_dblm(CSRd out, const double **in);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Total equality.
 */

/* @{ */

/*! Returns either TRUE or FALSE. */
int CSRd_toteq(const CSRd in1, const CSRd in2);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name I/O.
 */

/* @{ */

void CSRd_print(FILE * stream, const CSRd in, char *name);

void CSRd_densePrint(FILE * stream, const CSRd in, char *name);

void CSRd_internalPrint(FILE * stream, const CSRd in, char *name);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Structure operations.
 */

/* @{ */

/*! Horizontal concatenation of in1 (left) with in2 (right). */
void CSRd_x_concat(CSRd out, const CSRd in1, const CSRd in2);

/*! Vertical concatenation of in1 (upper) with in2 (lower). */
void CSRd_y_concat(CSRd out, const CSRd in1, const CSRd in2);

void CSRd_getRegion(CSRd out, const CSRd in, int x0, int y0);

void CSRd_transpose(CSRd out, const CSRd in);

/*!
 * Find indices of nonzero elements in a row-major sorted order.
 * out must be preallocated to contain at least in->length int's.
 */
void intarray_find_dcsr(int *out, const CSRd in);

/*!
 * Find x y coordinates of nonzero elements in a row-major sorted order.
 * out_x and out_y must be preallocated to contain at least in->length int's
 * each.
 */
void intarray_find_xycoords_dcsr(int *out_x, int *out_y, const CSRd in);

/*!
 * Returns an csr-matrix out formed from the elements of in1 specified by the
 * subscript vector in2.  The resulting array is the same size as in2 except
 * for the special case where in1 and in2 are both vectors.  In this case,
 * out gets the same number of elements as in2 but has the orientation of in1.
 */
void
CSRd_subscript_using_inta(CSRd out,
			  const CSRd in1, const int *in2, int nnz_in2);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Pointwise unary arithmetic operations.
 */

/* @{ */

int check_pointwise_unary_dims(CSRd out, const CSRd in);

/*! Constructs a matrix with the same structure as in. */
void CSRd_make_same(CSRd out, const CSRd in);

void CSRd_pwneg(CSRd out, const CSRd in);
void CSRd_pwabs(CSRd out, const CSRd in);

/*! Inverts non-zero elements. */
void CSRd_pwinv(CSRd out, const CSRd in);

void CSRd_pwmul_double(CSRd out, const CSRd in, const double scalar);

void CSRd_pwdiv_double(CSRd out, const CSRd in, const double scalar);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Pointwise binary arithmetic operations.
 */

/* @{ */

int check_pointwise_binary_dims(CSRd out,
				const CSRd in1, const CSRd in2);

void CSRd_pwadd(CSRd out, const CSRd in1, const CSRd in2);
void CSRd_pwsub(CSRd out, const CSRd in1, const CSRd in2);
void CSRd_pwmul(CSRd out, const CSRd in1, const CSRd in2);

/*!
 * Equivalent to CSRd_pwmul except that the scalar operation is divison
 * instead of multiplication.
 */
void CSRd_pwdiv(CSRd out, const CSRd in1, const CSRd in2);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Pointwise binary comparison operations.
 */

/* @{ */

/*! Equality. */
void CSRd_pweq(CSRd out, const CSRd in1, const CSRd in2);

/*! Min. */
void CSRd_pwmin(CSRd out, const CSRd in1, const CSRd in2);

/*! Max. */
void CSRd_elementwise_max(CSRd out, const CSRd in1, const CSRd in2);

/*! Less than. */
void CSRd_elementwise_lt(CSRd out, const CSRd in1, const CSRd in2);

/*! Greater than. */
void CSRd_elementwise_gt(CSRd out, const CSRd in1, const CSRd in2);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*! \name Pointwise dcsr-double comparison operations. */

/* @{ */

void CSRd_elementwise_eq_dbl(CSRd out, const CSRd in, double scalar);
void CSRd_elementwise_lt_dbl(CSRd out, const CSRd in, double scalar);
void CSRd_elementwise_gt_dbl(CSRd out, const CSRd in, double scalar);
void CSRd_elementwise_min_dbl(CSRd out, const CSRd in, double scalar);
void CSRd_elementwise_max_dbl(CSRd out, const CSRd in, double scalar);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Matrix - Transposed Matrix - Multiply.
 *
 * In Matlab: out = in1 * in2'
 *
 * Column-order access is expensive so we assume in2 to be the transpose of
 * the right argument.
 */
void CSRd_mmmul_tr(CSRd out, const CSRd in1, const CSRd in2);

/*!
 * Ordinary Matrix - Matrix - Multiply.
 */
void CSRd_mmmul(CSRd out, const CSRd in1, const CSRd in2);

/*!
 * Sparse matrix - Dense Vector - Multiply.
 */
void CSRd_mvmul_doublea(double *out, const CSRd in1, const double *in2);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name XY-Collective.
 */

/* @{ */

double CSRd_xy_sum(const CSRd in);
double CSRd_xy_product(const CSRd in);
double CSRd_xy_dot(const CSRd in1, const CSRd in2);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name X-Collective.
 */

/* @{ */

void CSRd_x_sum(CSRd out, const CSRd in);

/* @} */

#ifdef __cplusplus
}
#endif
