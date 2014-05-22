#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

void dmatrix_mul_orig(double **a, double **b, double **c,
		   int am, int an, int bn);

/*!
 * This function classifies a 3x3 matrix according to its zero structure.
 * It returns an uinteger in which each bit signifies a zero
 * structure that describes the given matrix.  If all bits are zero it
 * means the matrix is dense or does not fit any of these 15 forms.
 *
 *
 *  Permutations:                                                          *
 *                                                                         *
 *  * 0 0     0 * 0     0 0 *     0 * 0     * 0 0      0 0 *               *
 *  0 * 0     0 0 *     * 0 0     * 0 0     0 0 *      0 * 0               *
 *  0 0 *     * 0 0     0 * 0     0 0 *     0 * 0      * 0 0               *
 *                                                                         *
 *   P1        P2        P3        P4        P5         P6                 *
 *                                                                         *
 *                                                                         *
 *  Simple Rotations:                                                      *
 *                                                                         *
 *  * 0 0     * 0 *     * * 0                                              *
 *  0 * *     0 * 0     * * 0                                              *
 *  0 * *     * 0 *     0 0 *                                              *
 *                                                                         *
 *   RX        RY        RZ                                                *
 *                                                                         *
 *                                                                         *
 *  Permutations of the simple rotations:                                  *
 *                                                                         *
 *  0 0 *     0 * *     0 * 0     * * 0     * 0 *     0 * *                *
 *  * * 0     0 * *     * 0 *     0 0 *     * 0 *     * 0 0                *
 *  * * 0     * 0 0     * 0 *     * * 0     0 * 0     0 * *                *
 *                                                                         *
 *   C1        C2        C3        C4        C5        C6                  *
 *                                                                         *
 */
int dmatrix_classify3x3(const double **in);

void double_transform3x3(double *y0, double *y1, double *y2,
		      const double **m, uint form,
		      double *x0, double *x1, double *x2);

void dmatrix_mmul3x3(double **y,
		  const double **m, uint form, const double **x,
		  int n);

void mvmul4x4f(float out[4], const float in1[4][4], const float in2[4]);

void dmatrix_mmul_straight(double **out,
			const double **in1, const double **in2,
			int w1, int h1, int w2);

void dmatrix_mmul_tr(double **out,
		  const double **in1, const double **in2,
		  int w1, int h1, int h2);

/*!
 * Perform a standard matrix - matrix multiply.
 */
void dmatrix_mmul(double **out, const double **in1, const double **in2,
	       int w1, int h1, int w2);

/*!
 * Perform a Strassen optimized matrix - transposed-matrix multiply.
 */
void dmatrix_mmul_tr_strassen(double **out,
			   const double **in1, const double **in2,
			   int w1, int h1, int w2);

/*!
 * Perform a Strassen optimized matrix - matrix multiply.
 */
void dmatrix_mmul_strassen(double **out,
			const double **in1, const double **in2,
			int w1, int h1, int w2);

#ifdef __cplusplus
}
#endif
