/*!
 * \file inv.c
 * \brief Invert
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include "utils.h"
#include "extremes.h"
#include "sortn.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>

/*!
 * Gauss-Jordan Matrix Invert.
 */
void
dmatrix_gjinv1(double **a, int n)
{
  int i, j, k, max;
  for (i = 0; i < n; i++) {
    max = i;
    for (j = i + 1; j < n; j++) {
      if (fabs(a[j][i]) > fabs(a[max][i])) {
	max = j;
      }
    }
    for (k = i; k < n; k++) {
      double_swap(&a[i][k], &a[max][k]);
    }
    for (j = i + 1; j < n; j++) {
      for (k = n - 1; k >= i; k--) {
	a[j][k] -= a[i][k] * a[j][i] / a[i][i];
      }
    }
  }
}

void
dmatrix_gjinv(double **a, const double **b, int n)
{
  dmatrix_copy(a, b, n, n);
  dmatrix_gjinv1(a, n);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \see Graphics Gems II p. 348
 *
 * Computes the inverse of a 3D affine matrix; i.e. a matrix with a dimen-
 * sionality of 4 where the right column has the entries (0, 0, 0, 1).
 *
 * This procedure treats the 4 by 4 matrix as a block matrix and
 * calculates the inverse of one submatrix for a significant perform-
 * ance improvement over a general procedure that can invert any non-
 * singular matrix:
 *
 *          --        --          --          --
 *          |          | -1       |    -1      |
 *          | A      0 |          |   A      0 |
 *    -1    |          |          |            |
 *   M   =  |          |     =    |     -1     |
 *          | C      1 |          | -C A     1 |
 *          |          |          |            |
 *          --        --          --          --
 *
 *  where     M is a 4 by 4 matrix,
 *            A is the 3 by 3 upper left submatrix of M,
 *            C is the 1 by 3 lower left submatrix of M.
 *
 * \param[in] in 3D Affine 4x4 Matrix (C-array of length 16)
 * \param[out] out Inverse of 3D Affine 4x4 matrix (C-array of length 16)
 *
 * Returned value:
 *   TRUE   if input matrix is nonsingular
 *   FALSE  otherwise
 */
int
darray_aff4x4inv(double out[4][4], const double in[4][4])
{
  double det_1, pos_sum, neg_sum, tmp;

  /* Accumulate macro. */
#define ACC()    \
    if (tmp >= 0.0) { \
        pos_sum += tmp;   \
    } else {          \
        neg_sum += tmp;   \
    }

  /*
   * Calculate the determinant of submatrix A and determine if the
   * the matrix is singular as limited by the precision of the
   * floating-point data representation.
   */
  pos_sum = neg_sum = 0.0;
  tmp = in[0][0] * in[1][1] * in[2][2];
  ACC();
  tmp = in[0][1] * in[1][2] * in[2][0];
  ACC();
  tmp = in[0][2] * in[1][0] * in[2][1];
  ACC();
  tmp = -in[0][2] * in[1][1] * in[2][0];
  ACC();
  tmp = -in[0][1] * in[1][0] * in[2][2];
  ACC();
  tmp = -in[0][0] * in[1][2] * in[2][1];
  ACC();

  det_1 = pos_sum + neg_sum;

  /* Is the submatrix A singular? Either check
   * (... < DBL_MIN) or
   * (... == 0)
   */
  if ((det_1 == 0.0) || (ABS(det_1 / (pos_sum - neg_sum)) < DBL_MIN)) {
    leprintf("affine_matrix4_inverse: singular matrix\n");
    return FALSE;
  } else {

    /* Calculate inverse(A) = adj(A) / det(A). */

    det_1 = 1.0 / det_1;

    out[0][0] = (in[1][1] * in[2][2] - in[1][2] * in[2][1]) * det_1;
    out[1][0] = -(in[1][0] * in[2][2] - in[1][2] * in[2][0]) * det_1;
    out[2][0] = (in[1][0] * in[2][1] - in[1][1] * in[2][0]) * det_1;

    out[0][1] = -(in[0][1] * in[2][2] - in[0][2] * in[2][1]) * det_1;
    out[1][1] = (in[0][0] * in[2][2] - in[0][2] * in[2][0]) * det_1;
    out[2][1] = -(in[0][0] * in[2][1] - in[0][1] * in[2][0]) * det_1;

    out[0][2] = (in[0][1] * in[1][2] - in[0][2] * in[1][1]) * det_1;
    out[1][2] = -(in[0][0] * in[1][2] - in[0][2] * in[1][0]) * det_1;
    out[2][2] = (in[0][0] * in[1][1] - in[0][1] * in[1][0]) * det_1;

    /* Calculate -C * inverse(A). */

    out[3][0] = -(in[3][0] * out[0][0] +
		  in[3][1] * out[1][0] + in[3][2] * out[2][0]);
    out[3][1] = -(in[3][0] * out[0][1] +
		  in[3][1] * out[1][1] + in[3][2] * out[2][1]);
    out[3][2] = -(in[3][0] * out[0][2] +
		  in[3][1] * out[1][2] + in[3][2] * out[2][2]);

    /* Fill in last column */
    out[0][3] = out[1][3] = out[2][3] = 0.0;
    out[3][3] = 1.0;

    return TRUE;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Solve Linear Equation.
 *
 * Linear Equation Solution by Gauss-Jordan Elimination with Full
 * Pivoting.
 *
 * \p a is replaced by its matrix inverse, and \p b is replaced by the
 * correspoding set of solution vectors.
 *
 * \param[in,out] a is (n x n)
 * \param[in,out] b is (n x m)
 *
 * \cite{crecipes} 2.1 p. 39
 *
 * \TODO: Does not work.
 */
void
dmatrix_gjsolve(double **a, double **b, int m, int n)
{
  int i, j, k, l, ll;
  int colI = 0, rowI = 0;

  /* used for bookkeeping on the pivoting */
  int *colidx = (int *) malloc(n * sizeof(int));
  int *rowidx = (int *) malloc(n * sizeof(int));
  int *ipiv = (int *) malloc(n * sizeof(int));

  for (j = 0; j < n; j++) {
    ipiv[j] = -1;
  }

  /* main loop over the columns to be reduced */
  for (i = 0; i < n; i++) {
    double max = 0.0;

    /* outer loop of the search for a pivot element */
    for (j = 0; j < n; j++) {
      if (ipiv[j] != 0) {
	for (k = 0; k < n; k++) {
	  if (ipiv[k] == -1) {
	    if (fabs(a[j][k]) >= max) {
	      max = fabs(a[j][k]);
	      rowI = j;
	      colI = k;
	    }
	  } else if (ipiv[k] > 0) {
	    leprintf("%s: singular matrix-1\n", __FUNCTION__);
	    return;
	  }
	}
      }
    }
    ipiv[colI]++;

    /* We now have the pivot element, so we interchange rows,
     * if needed, to put the pivot element on the diagonal.
     * The columns are not physically interchanged, only relabeled:
     * colidx[i], the column of the ith pivot elemnt, is the ith
     * column that is reduced, while rowidx[i] is the row in which
     * that pivot elemnt was originally located.
     * If rowidx[i] != colidx[i] there is an implied column interchange.
     * With this form of bookkeeping, the solution b's will end up
     * in the correct order, and the inverse matrix will be scramlbed
     * by columns.
     */

    if (rowI != colI) {
      for (l = 0; l < n; l++) {
	double_swap(&a[rowI][l], &a[colI][l]);
      }
      for (l = 0; l < m; l++) {
	double_swap(&b[rowI][l], &b[colI][l]);
      }
    }
    rowidx[i] = rowI;
    colidx[i] = colI;

    /* we are now ready to divide the pivot row by the pivot elemnt,
     * located at rowI and colI */

    if (a[colI][colI] == 0.0) {
      leprintf("%s: singular matrix-2\n", __FUNCTION__);
      return;
    }
    double tmp = 1.0 / a[colI][colI];	/* inverse of pivot element */
    a[colI][colI] = 1.0;
    for (l = 0; l < n; l++) {
      a[colI][i] *= tmp;
    }
    for (l = 0; l < m; l++) {
      b[colI][i] *= tmp;
    }
    /* Next, we reduce the rows */
    for (ll = 0; ll < n; ll++) {
      if (ll != colI) {
	/* except for the pivot one, of course */
	tmp = a[ll][colI];
	a[ll][colI] = 0.0;
	for (l = 0; l < n; l++) {
	  a[ll][l] -= a[colI][l] * tmp;
	}
	for (l = 0; l < m; l++) {
	  b[ll][l] -= b[colI][l] * tmp;
	}
      }
    }
  }

  /* This is the end of the main loop over columns of the reduction.
   * It only remains to unscramble the solution in view of the column
   * interchanges. We do this by interchanging pairs of columns in
   * the reverse order that the permutation was built up.
   */
  for (l = n - 1; l >= 0; l--) {
    if (rowidx[l] != colidx[l]) {
      for (k = 0; k < n; k++) {
	double_swap(&a[k][rowidx[l]], &a[k][colidx[l]]);
      }
    }
  }

  free(colidx);
  free(rowidx);
  free(ipiv);
}
