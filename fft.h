/*!
 * \file fft.h
 * \brief Fast Fourier Transform
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * (I)DFT - (Inverse) Discrete Fourier Transform. O(n*n) time-complexity
 * (I)FFT - (Inverse) Fast Fourier Transform.  O(n*log(n)) time-complexity
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * DFT
 */
void darray_DFT(double *realO, double *imagO,
                double *realI, double *imagI, int n);

/*!
 * IDFT
 */
void darray_IDFT(double *realO, double *imagO,
                 double *realI, double *imagI, int n);

/* ---------------------------- Group Separator ---------------------------- */

/*! 1-point FFT and IFFT */

void darray1_FFT(double *realO, double *imagO,
                 double *realI, double *imagI);

void darray1_IFFT(double *realO, double *imagO,
                  double *realI, double *imagI);

/* ---------------------------- Group Separator ---------------------------- */

/*! 2-point FFT and IFFT */

void darray2_FFT(double *realO, double *imagO,
                 double *realI, double *imagI);

void darray2_IFFT(double *realO, double *imagO,
                  double *realI, double *imagI);

/* ---------------------------- Group Separator ---------------------------- */

/*! 3-point FFT and IFFT */

void darray3_FFT(double *realO, double *imagO,
                 double *realI, double *imagI);

void darray3_IFFT(double *realO, double *imagO,
                  double *realI, double *imagI);

/* ---------------------------- Group Separator ---------------------------- */

/*! 4-point FFT and IFFT */

void darray4_FFT(double *realO, double *imagO,
                 double *realI, double *imagI);

void darray4_IFFT(double *realO, double *imagO,
                  double *realI, double *imagI);

/* ---------------------------- Group Separator ---------------------------- */

/*! 5-point FFT and IFFT */

void darray5_FFT(double *realO, double *imagO,
                 double *realI, double *imagI);

void darray5_IFFT(double *realO, double *imagO,
                  double *realI, double *imagI);

/* ---------------------------- Group Separator ---------------------------- */

/*! 8-point FFT and IFFT */

void darray8_FFT(double *realO, double *imagO,
                 double *realI, double *imagI);

void darray8_IFFT(double *realO, double *imagO,
                  double *realI, double *imagI);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * n-point FFT
 */
void darray_FFT(double *realO, double *imagO,
                double *realI, double *imagI, int n);

/*!
 * n-point IFFT
 */
void darray_IFFT(double *realO, double *imagO,
                 double *realI, double *imagI, int n);

/*!
 * n-point FFT and IFFT of pair of two
 */
void darray_realPairFFT(double *out0_r, double *out0_i,
                        double *out1_r, double *out1_i,
                        double *in0_r, double *in1_r, int n);

/*!
 * hermitian-pair IFFT
 */
void darray_hermitianPairIFFT(double *out0_r,
                              double *out1_r,
                              double *in0_r, double *in0_i,
                              double *in1_r, double *in1_i, int n);

/*!
 * hermitian-pair IDFT
 */
void darray_hermitianPairIDFT(double *out0_r,
                              double *out1_r,
                              double *in0_r, double *in0_i,
                              double *in1_r, double *in1_i, int n);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Real-Valued FFT
 */
void darray_realFFT(double *out0_r, double *out0_i,
                    double *in0_r, int n);

/*!
 * Real-Valued IFFT
 */
void darray_realIFFT(double *out0_r, double *out0_i,
                     double *in0_r, int n);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * two-dimensional FFT
 */
voublematrix_2DFFT(double **realO, double **imagO,
                   double **realI, double **imagI,
                   int dim0, int dim1, int is_inverse);

/*!
 * two-dimensional real FFT
 */
void dmatrix_real2DFFT(double **realO, double **imagO,
                       double **realI,
                       int dim0, int dim1, int is_inverse);

/* ---------------------------- Group Separator ---------------------------- */

void test_FFT(int n, int show);

#ifdef __cplusplus
}
#endif
