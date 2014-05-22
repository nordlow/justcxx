/*!
 * \file iqsort.hpp
 * \brief Specialized Quick Sorting of C-type arrays alone and corresponding
 * index integer array.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <algorithm>
#include "sortn.hpp"

/* ========================================================================= */

/*!
 * Calculate index of the median of the three values in \p x indexed
   by the three indexes \p a \p b \p c.
 * \return the calculated index
 */
template<typename T>
inline size_t
indexed_median3(const T *x, size_t a, size_t b, size_t c)
{
    return (x[a] < x[b] ?
	    (x[b] < x[c] ? b : x[a] < x[c] ? c : a) :
	    (x[b] > x[c] ? b : x[a] > x[c] ? c : a));
}

/*!
 * Swaps \p x[\p a] with \p x[\p b].
 */
template<typename T>
inline void
indexed_swap(T *x, size_t a, size_t b) {
    std::swap(x[a], x[b]);
}

/*!
 * Swaps \p x[\p a] with \p x[\p b].
 */
template<typename T>
inline void
indexed_swapi(T *x, size_t *idx, size_t a, size_t b)
{
    std::swap(x[a], x[b]);
    std::swap(idx[a], idx[b]);
}

/*!
 * Swaps \p x[\p a .. (\p a+\p n-1)] with \p x[\p b .. (\p b+\p n-1)].
 */
template<typename T>
inline void
indexed_rangeswap(T *x, size_t a, size_t b, size_t n)
{
    for (size_t i = 0; i < n; i++, a++, b++)
    {
	indexed_swap(x, a, b);
    }
}

/*!
 * Swaps \p x[\p a .. (\p a+\p n-1)] with \p x[\p b .. (\p b+\p n-1)].
 */
template<typename T>
inline void
indexed_rangeswapi(T *x, size_t *idx, size_t a, size_t b, size_t n)
{
    for (size_t i = 0; i < n; i++, a++, b++)
    {
	indexed_swapi(x, idx, a, b);
    }
}

template<typename T>
inline void
carray_qsort1(T *x, size_t off, size_t len)
{
    /* choose a partition element, v */
    size_t m = off + len / 2;	/* small arrays, middle element */
    if (len > 7) {
	size_t l = off;
	size_t n = off + len - 1;
	if (len > 40) {		/* big arrays, pseudomedian of 9 */
	    size_t s = len / 8;
	    l = indexed_median3(x, l, l + s, l + 2 * s);
	    m = indexed_median3(x, m - s, m, m + s);
	    n = indexed_median3(x, n - 2 * s, n - s, n);
	}
	m = indexed_median3(x, l, m, n); /* mid-size, med of 3 */
    }
    T v = x[m];

    /* establish Invariant: v* (<v)* (>v)* v* */
    size_t a = off;
    size_t b = a;
    size_t c = off + len - 1;
    size_t d = c;
    while (TRUE) {
	while (b <= c && x[b] <= v) {
	    if (x[b] == v)
		indexed_swap(x, a++, b);
	    b++;
	}
	while (c >= b && x[c] >= v) {
	    if (x[c] == v)
		indexed_swap(x, c, d--);
	    c--;
	}
	if (b > c)
	    break;
	indexed_swap(x, b++, c--);
    }

    /* swap partition elements back to middle */
    size_t n = off + len;
    size_t s = std::min(a - off, b - a);
    indexed_rangeswap(x, off, b - s, s);
    s = std::min(d - c, n - d - 1);
    indexed_rangeswap(x, b, n - s, s);

    /* recursively sort non-partition-elements */
    if ((s = b - a) > 1)
	carray_qsort1(x, off, s);
    if ((s = d - c) > 1)
	carray_qsort1(x, n - s, s);
}

template<typename T>
inline void
carray_qsort(T *a, size_t n)
{
    carray_qsort1(a, 0, n);
}

template<typename T>
inline void
carray_iqsort1(T *x, size_t *idx, size_t off, size_t len)
{
    /* choose a partition element, v */
    size_t m = off + len / 2;		/* small arrays, middle element */
    if (len > 7) {
	size_t l = off;
	size_t n = off + len - 1;
	if (len > 40) {		/* big arrays, pseudomedian of 9 */
	    size_t s = len / 8;
	    l = indexed_median3(x, l, l + s, l + 2 * s);
	    m = indexed_median3(x, m - s, m, m + s);
	    n = indexed_median3(x, n - 2 * s, n - s, n);
	}
	m = indexed_median3(x, l, m, n);	/* mid-size, med of 3 */
    }
    T v = x[m];

    /* establish Invariant: v* (<v)* (>v)* v* */
    size_t a = off;
    size_t b = a;
    size_t c = off + len - 1;
    size_t d = c;
    while (TRUE) {
	while (b <= c && x[b] <= v) {
	    if (x[b] == v)
		indexed_swapi(x, idx, a++, b);
	    b++;
	}
	while (c >= b && x[c] >= v) {
	    if (x[c] == v)
		indexed_swapi(x, idx, c, d--);
	    c--;
	}
	if (b > c)
	    break;
	indexed_swapi(x, idx, b++, c--);
    }

    /* swap partition elements back to middle */
    size_t n = off + len;
    size_t s = std::min(a - off, b - a);
    indexed_rangeswapi(x, idx, off, b - s, s);
    s = std::min(d - c, n - d - 1);
    indexed_rangeswapi(x, idx, b, n - s, s);

    /* recursively sort non-partition-elements */
    if ((s = b - a) > 1)
	carray_iqsort1(x, idx, off, s);
    if ((s = d - c) > 1)
	carray_iqsort1(x, idx, n - s, s);

}

template<typename T>
inline void
carray_iqsort(T *a, size_t *idx, size_t n)
{
    carray_iqsort1(a, idx, 0, n);
}

template<typename T>
bool
carray_is_sorted(T *a, size_t n)
{
    for (size_t j = 0; j < n - 1; j++)
    {
	if (a[j] > a[j + 1])
	{
	    return false;
	}
    }
    return true;
}
