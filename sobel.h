/*!
 * \file sobel.h
 * \brief Image Processing Sobel and Blur Operations on Images.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Images are currently implemented as C Array-of-Arrays.
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

void fmatrix_sobel3x3(float **out, const float **in, int w_out, int h_out);
void fmatrix_blur3x1(float **out, const float **in, int w_out, int h_out);
void fmatrix_blur3x3(float **out, const float **in, int w_out, int h_out);

#ifdef __cplusplus
}
#endif
