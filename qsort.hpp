/*!
 * \file qsort.hpp
 * \brief Specialized Quick Sorting of C-type arrays.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <algorithm>
#include "sortn.hpp"

#define QSORT_USE_SORTN (1)

/* ---------------------------- Group Separator ---------------------------- */

/* Sort narrays into ascending order. The sort algorithm is an
 * optimised quicksort, as described in Jon L. Bentley and M. Douglas
 * McIlroy's Engineering a Sort Function", Software-Practice and
 * Experience, Vol. 23(11) P. 1249-1265 (November 1993). This
 * algorithm gives n log (n) performance on many arrays that would
 * take quadratic time with a standard quicksort.
 */

/* ---------------------------- Group Separator ---------------------------- */

/* Calculate Index of the Median of the three Indexed Values. */
template<class T> inline size_t Ti_median3(const T *x, size_t a, size_t b, size_t c)
{
    return (x[a] < x[b] ?
            (x[b] < x[c] ? b : x[a] < x[c] ? c : a) :
            (x[b] > x[c] ? b : x[a] > x[c] ? c : a));
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
template<class T> static inline void Ti_swap(T *x, size_t a, size_t b)
{
    T t = x[a]; x[a] = x[b]; x[b] = t;
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
template<class T> static inline void Ti_swapi(T *x, size_t *idx, size_t a, size_t b)
{
    Ti_swap(x, a, b);
    size_t t2 = idx[a]; idx[a] = idx[b]; idx[b] = t2;
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
template<class T> inline void Ti_rangeswap(T *x, size_t a, size_t b, size_t n)
{
    for (size_t i = 0; i < n; i++, a++, b++) { Ti_swap(x, a, b); }
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
template<class T> inline void Ti_rangeswapi(T *x, size_t *idx, size_t a, size_t b, size_t n)
{
    for (size_t i = 0; i < n; i++, a++, b++) { Ti_swapi(x, idx, a, b); }
}

template<class T> inline void Tarray_qsort1(T *x, size_t off, size_t len)
{
    T v;

#if QSORT_USE_SORTN
    if (len <= SORTN_MAX) { SORTN(x+off, len); return; }
#endif

    /* choose a partition element, v */
    size_t m = off + len / 2;		/* small arrays, middle element */
    if (len > 7) {
        size_t l = off;
        size_t n = off + len - 1;
        if (len > 40) {		/* big arrays, pseudomedian of 9 */
            size_t s = len / 8;
            l = Ti_median3(x, l, l + s, l + 2 * s);
            m = Ti_median3(x, m - s, m, m + s);
            n = Ti_median3(x, n - 2 * s, n - s, n);
        }
        m = Ti_median3(x, l, m, n); /* mid-size, med of 3 */
    }
    v = x[m];

    /* establish Invariant: v* (<v)* (>v)* v* */
    size_t a = off;
    size_t b = a;
    size_t c = off + len - 1;
    size_t d = c;
    while (TRUE) {
        while (b <= c and x[b] <= v) {
            if (x[b] == v)
                Ti_swap(x, a++, b);
            b++;
        }
        while (c >= b and x[c] >= v) {
            if (x[c] == v)
                Ti_swap(x, c, d--);
            c--;
        }
        if (b > c)
            break;
        Ti_swap(x, b++, c--);
    }

    /* swap partition elements back to middle */
    size_t n = off + len;
    size_t s = std::min(a - off, b - a);
    Ti_rangeswap(x, off, b - s, s);
    s = std::min(d - c, n - d - 1);
    Ti_rangeswap(x, b, n - s, s);

    /* recursively sort non-partition-elements */
#if QSORT_USE_SORTN
    const size_t t = b - a;
    if (t <= SORTN_MAX) { SORTN(x+off, t); }
    else        { Tarray_qsort1(x, off, t); }

    const size_t u = d - c;
    const size_t off1 = n - u;
    if (u <= SORTN_MAX) { SORTN(x+off1, u); }
    else        { Tarray_qsort1(x, off1, u); }
#else
    if ((s = b - a) > 1)
        Tarray_qsort1(x, off, s);
    if ((s = d - c) > 1)
        Tarray_qsort1(x, n - s, s);
#endif
}

template<class T>
inline void
Tarray_qsort(T *a, size_t n)
{
    Tarray_qsort1(a, 0, n);
}

template<class T>
inline void
Tarray_iqsort1(T *x, size_t *idx, size_t off, size_t len)
{
    T v;

    /* choose a partition element, v */
    size_t m = off + len / 2;		/* small arrays, middle element */
    if (len > 7) {
        size_t l = off;
        size_t n = off + len - 1;
        if (len > 40) {		/* big arrays, pseudomedian of 9 */
            size_t s = len / 8;
            l = Ti_median3(x, l, l + s, l + 2 * s);
            m = Ti_median3(x, m - s, m, m + s);
            n = Ti_median3(x, n - 2 * s, n - s, n);
        }
        m = Ti_median3(x, l, m, n);	/* mid-size, med of 3 */
    }
    v = x[m];

    /* establish Invariant: v* (<v)* (>v)* v* */
    size_t a = off;
    size_t b = a;
    size_t c = off + len - 1;
    size_t d = c;
    while (TRUE) {
        while (b <= c and x[b] <= v) {
            if (x[b] == v)
                Ti_swapi(x, idx, a++, b);
            b++;
        }
        while (c >= b and x[c] >= v) {
            if (x[c] == v)
                Ti_swapi(x, idx, c, d--);
            c--;
        }
        if (b > c)
            break;
        Ti_swapi(x, idx, b++, c--);
    }

    /* swap partition elements back to middle */
    size_t n = off + len;
    size_t s = std::min(a - off, b - a);
    Ti_rangeswapi(x, idx, off, b - s, s);
    s = std::min(d - c, n - d - 1);
    Ti_rangeswapi(x, idx, b, n - s, s);

    /* recursively sort non-partition-elements */
    if ((s = b - a) > 1)
        Tarray_iqsort1(x, idx, off, s);
    if ((s = d - c) > 1)
        Tarray_iqsort1(x, idx, n - s, s);
}

template<class T> void Tarray_iqsort(T *a, size_t *idx, size_t n) {
    Tarray_iqsort1(a, idx, 0, n);
}

template<class T> bool Tarray_is_sorted(T *a, size_t n) {
    for (size_t j = 0; j < n - 1; j++) {
        if (a[j] > a[j + 1]) {
            return false;
        }
    }
    return true;
}
