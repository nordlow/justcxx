/*!
 * \file meman.h
 * \brief Memory Management of Arrays and Arrays of Pointers to Arrays.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-06-05 10:21
 *
 * Matrix Dimension: m columns (image width), n rows (image height).
 */

#pragma once

#include <stdlib.h>
#include "pnw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * \name Memory Allocation of C Arrays, C Array-Arrays, etc.
 */
/* @{ */

static inline int8_t * int8array_malloc(size_t n) { return (int8_t*)malloc(n * sizeof(int8_t)); }
static inline int16_t * int16array_malloc(size_t n) { return (int16_t*)malloc(n * sizeof(int16_t)); }
static inline int32_t * int32array_malloc(size_t n) { return (int32_t*)malloc(n * sizeof(int32_t)); }
static inline short * shortarray_malloc(size_t n) { return (short*)malloc(n * sizeof(short)); }
static inline int * intarray_malloc(size_t n) { return (int*)malloc(n * sizeof(int)); }
static inline size_t * sizearray_malloc(size_t n) { return (size_t*)malloc(n * sizeof(size_t)); }
static inline float * farray_malloc(size_t n) { return (float*)malloc(n * sizeof(float)); }
static inline double * darray_malloc(size_t n) { return (double*)malloc(n * sizeof(double)); }
static inline long double * ldarray_malloc(size_t n) { return (long double*)malloc(n * sizeof(long double)); }

/* ---------------------------- Group Separator ---------------------------- */

static inline int8_t * int8array_calloc(size_t n) { return (int8_t*)calloc(n, sizeof(int8_t)); }
static inline int16_t * int16array_calloc(size_t n) { return (int16_t*)calloc(n, sizeof(int16_t)); }
static inline int32_t * int32array_calloc(size_t n) { return (int32_t*)calloc(n, sizeof(int32_t)); }
static inline short * shortarray_calloc(size_t n) { return (short*)calloc(n, sizeof(short)); }
static inline int * intarray_calloc(size_t n) { return (int*)calloc(n, sizeof(int)); }
static inline size_t * sizearray_calloc(size_t n) { return (size_t*)calloc(n, sizeof(size_t)); }
static inline float * farray_calloc(size_t n) { return (float*)calloc(n, sizeof(float)); }
static inline double * darray_calloc(size_t n) { return (double*)calloc(n, sizeof(double)); }
static inline long double * ldarray_calloc(size_t n) { return (long double*)calloc(n, sizeof(long double)); }

/* ---------------------------- Group Separator ---------------------------- */

static inline int8_t * int8array_realloc(int8_t *a, size_t n) { return (int8_t*)realloc(a, n * sizeof(int8_t)); }
static inline int16_t * int16array_realloc(int16_t *a, size_t n) { return (int16_t*)realloc(a, n * sizeof(int16_t)); }
static inline int32_t * int32array_realloc(int32_t *a, size_t n) { return (int32_t*)realloc(a, n * sizeof(int32_t)); }
static inline short * shortarray_realloc(short *a, size_t n) { return (short*)realloc(a, n * sizeof(short)); }
static inline int * intarray_realloc(int *a, size_t n) { return (int*)realloc(a, n * sizeof(int)); }
static inline int * sizearray_realloc(size_t *a, size_t n) { return (int*)realloc(a, n * sizeof(size_t)); }
static inline float * farray_realloc(float *a, size_t n) { return (float*)realloc(a, n * sizeof(float)); }
static inline double * darray_realloc(double *a, size_t n) { return (double*)realloc(a, n * sizeof(double)); }
static inline long double * ldarray_realloc(long double *a, size_t n) { return (long double*)realloc(a, n * sizeof(long double)); }

/* ---------------------------- Group Separator ---------------------------- */

int8_t **int8AA_malloc(size_t m, size_t n);
int16_t **int16AA_malloc(size_t m, size_t n);
int32_t **int32AA_malloc(size_t m, size_t n);
short **shortAA_malloc(size_t m, size_t n);
int **intAA_malloc(size_t m, size_t n);
float **floatAA_malloc(size_t m, size_t n);
double **doubleAA_malloc(size_t m, size_t n);
long double **ldoubleAA_malloc(size_t m, size_t n);

/* ---------------------------- Group Separator ---------------------------- */

int8_t **int8AA_calloc(size_t m, size_t n);
int16_t **int16AA_calloc(size_t m, size_t n);
int32_t **int32AA_calloc(size_t m, size_t n);
short **shortAA_calloc(size_t m, size_t n);
int **intAA_calloc(size_t m, size_t n);
float **floatAA_calloc(size_t m, size_t n);
double **doubleAA_calloc(size_t m, size_t n);
long double **ldoubleAA_calloc(size_t m, size_t n);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Memory Freeing.
 */
/* @{ */

static inline void int8matrix_free(int8_t **a) { free(a[0]); free(a); }
static inline void int16matrix_free(int16_t **a) { free(a[0]); free(a); }
static inline void int32matrix_free(int32_t **a) { free(a[0]); free(a); }
static inline void intmatrix_free(int **a) { free(a[0]); free(a); }
static inline void fmatrix_free(float **a) { free(a[0]); free(a); }
static inline void dmatrix_free(double **a) { free(a[0]); free(a); }
static inline void ldmatrix_free(long double **a) { free(a[0]); free(a); }

/* @} */

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
