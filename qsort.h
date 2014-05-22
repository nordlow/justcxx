/*!
 * \file qsort.h
 * \brief Specialized Quick Sorting of C-type arrays alone and
 *        corresponding index integer array.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <stdbool.h>
#include <string.h>
#include "pnw_types.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Alternate QuickSort Algorithm.
 *
 * \WARNING: Argument function pointer compar() must return 0 if arguments are
 * equal otherwise function will never return.
 */
void aqsort(void *a, size_t n, size_t size,
           int(*cmp)(const void *, const void *));

/*!
 * Alternate QuickSort Algorithm with Index Sorting.
 *
 * Extends the standard sort functions qsort() in libc to also sort
 * the integers (indexes) in \p idx.
 *
 * If \p idx initially contains 0, 1, 2, ..., (\p nmemb-1) this will
 * produce a <em>permutation</em> that can be used to keep other arrays
 * consistent with the reordering of the items contained in base.
 *
 * \WARNING: Argument function pointer \p compar() must return 0 if
 *          arguments areequal otherwise function will never return.
 */
void iqsort(void *a, size_t *idx, size_t n, size_t size,
            int(*cmp)(const void *, const void *));

/*!
 * Extra Argument QuickSort Algorithm.
 *
 * Extends the standard sort functions qsort() in libc to also sort
 * the integers (indexes) in \p idx.
 *
 * If \p idx initially contains 0, 1, 2, ..., (\p nmemb-1) this will
 * produce a <em>permutation</em> that can be used to keep other arrays
 * consistent with the reordering of the items contained in base.
 *
 * \WARNING: Argument function pointer \p compar() must return 0 if
 *          arguments areequal otherwise function will never return.
 */
void xqsort(void *a, size_t *b, size_t n, size_t size_a, size_t size_b,
            int(*cmp)(const void *, const void *));

bool anyv_is_sorted(void *a, size_t size, size_t n,
                   int(*cmp)(const void *, const void *));

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Comparison used when sorting arrays of string pointers.
 */
static inline int
strp_cmp(const void *a, const void *b)
{
  const char * x = *((const char**)a);
  const char * y = *((const char**)b);
  return strcmp(x, y);
}

/* ---------------------------- Group Separator ---------------------------- */

#if 0
void farray_qsort_alt(float *a, size_t n);
void darray_qsort_alt(double *a, size_t n);
void ldarray_qsort_alt(ldouble *a, size_t n);
void shortarray_qsort_alt(short *a, size_t n);
void intarray_qsort_alt(int *a, size_t n);
#endif

/* ---------------------------- Group Separator ---------------------------- */

void farray_qsort(float *a, size_t n);
void farray_iqsort(float *a, size_t *idx, size_t n);
bool farray_is_sorted(float *a, size_t n);

/* ---------------------------- Group Separator ---------------------------- */

void darray_qsort(double *a, size_t n);
void darray_iqsort(double *a, size_t *idx, size_t n);
bool darray_is_sorted(double *a, size_t n);

/* ---------------------------- Group Separator ---------------------------- */

void ldarray_qsort(ldouble *a, size_t n);
void ldarray_iqsort(ldouble *a, size_t *idx, size_t n);
bool ldarray_is_sorted(ldouble *a, size_t n);

/* ---------------------------- Group Separator ---------------------------- */

void shortarray_qsort(short *a, size_t n);
void shortarray_iqsort(short *a, size_t *idx, size_t n);
bool shortarray_is_sorted(short *a, size_t n);

/* ---------------------------- Group Separator ---------------------------- */

void intarray_qsort(int *a, size_t n);
void intarray_iqsort(int *a, size_t *idx, size_t n);
bool intarray_is_sorted(int *a, size_t n);

#ifdef __cplusplus
}
#endif
