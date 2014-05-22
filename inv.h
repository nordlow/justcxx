/*!
 * \file inv.h
 * \brief Invert
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Gauss-Jordan Invert
 */
void dmatrix_gjinv(double **a, double **b, int n);

/*!
 * Computes the inverse of a 3D affine matrix (of size 4x4).
 *
 * \param[in] in 3D Affine 4x4 Matrix (C-array of length 16)
 * \param[out] out Inverse of 3D Affine 4x4 matrix (C-array of length 16)
 */
int darray_aff4x4inv(double out[4][4], const double in[4][4]);

/*!
 * Solve linear equation.
 *
 * \TODO: Does not work.
 */
void dmatrix_gjsolve(double **a, double **b, int m, int n);

#ifdef __cplusplus
}
#endif
