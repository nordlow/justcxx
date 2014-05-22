#include "meman.h"

/* ========================================================================= */

int8_t **
int8AA_malloc(size_t m, size_t n)
{
  size_t y;
  int8_t **aa;
  aa = (int8_t **) malloc(m * sizeof(int8_t *));	/* Alloc row pointers. */
  aa[0] = int8array_malloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

int16_t **
int16AA_malloc(size_t m, size_t n)
{
  size_t y;
  int16_t **aa;
  aa = (int16_t **) malloc(m * sizeof(int16_t *));	/* Alloc row pointers. */
  aa[0] = int16array_malloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

int32_t **
int32AA_malloc(size_t m, size_t n)
{
  size_t y;
  int32_t **aa;
  aa = (int32_t **) malloc(m * sizeof(int32_t *));	/* Alloc row pointers. */
  aa[0] = int32array_malloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

short **
shortAA_malloc(size_t m, size_t n)
{
  size_t y;
  short **aa;
  aa = (short **) malloc(m * sizeof(short *));	/* Alloc row pointers. */
  aa[0] = shortarray_malloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

int **
intAA_malloc(size_t m, size_t n)
{
  size_t y;
  int **aa;
  aa = (int **) malloc(m * sizeof(int *));	/* Alloc row pointers. */
  aa[0] = intarray_malloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

float **
floatAA_malloc(size_t m, size_t n)
{
  size_t y;
  float **aa;
  aa = (float **) malloc(m * sizeof(float *));	/* Alloc row pointers. */
  aa[0] = farray_malloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

double **
doubleAA_malloc(size_t m, size_t n)
{
  size_t y;
  double **aa;
  aa = (double **) malloc(m * sizeof(double *));	/* Alloc row pointers. */
  aa[0] = darray_malloc(n * m);	/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

long double **
ldoubleAA_malloc(size_t m, size_t n)
{
  size_t y;
  long double **aa;
  aa = (long double **) malloc(m * sizeof(long double *));	/* Alloc row pointers. */
  aa[0] = ldarray_malloc(n * m);	/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

/* ---------------------------- Group Separator ---------------------------- */

int8_t **
int8AA_calloc(size_t m, size_t n)
{
  size_t y;
  int8_t **aa;
  aa = (int8_t **) malloc(m * sizeof(int8_t *));	/* Alloc row pointers. */
  aa[0] = int8array_calloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

int16_t **
int16AA_calloc(size_t m, size_t n)
{
  size_t y;
  int16_t **aa;
  aa = (int16_t **) malloc(m * sizeof(int16_t *));	/* Alloc row pointers. */
  aa[0] = int16array_calloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

int32_t **
int32AA_calloc(size_t m, size_t n)
{
  size_t y;
  int32_t **aa;
  aa = (int32_t **) malloc(m * sizeof(int32_t *));	/* Alloc row pointers. */
  aa[0] = int32array_calloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

short **
shortAA_calloc(size_t m, size_t n)
{
  size_t y;
  short **aa;
  aa = (short **) malloc(m * sizeof(short *));	/* Alloc row pointers. */
  aa[0] = shortarray_calloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

int **
intAA_calloc(size_t m, size_t n)
{
  size_t y;
  int **aa;
  aa = (int **) malloc(m * sizeof(int *));	/* Alloc row pointers. */
  aa[0] = intarray_calloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

float **
floatAA_calloc(size_t m, size_t n)
{
  size_t y;
  float **aa;
  aa = (float **) malloc(m * sizeof(float *));	/* Alloc row pointers. */
  aa[0] = farray_calloc(n * m);			/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

double **
doubleAA_calloc(size_t m, size_t n)
{
  size_t y;
  double **aa;
  aa = (double **) malloc(m * sizeof(double *)); /* Alloc row pointers. */
  aa[0] = darray_calloc(n * m);	/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}

long double **
ldoubleAA_calloc(size_t m, size_t n)
{
  size_t y;
  long double **aa;
  aa = (long double **) malloc(m * sizeof(long double *)); /* Alloc row pointers. */
  aa[0] = ldarray_calloc(n * m);	/* Allocate data. */
  for (y = 1; y < m; y++) {
    aa[y] = aa[y - 1] + n;	/* Set row pointers incrementally. */
  }
  return aa;
}
