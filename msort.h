/*!
 * \file msort.h
 * \brief Merge Sort.
 * \date 2011-04-09 09:13
 * \see http://jeffreystedfast.blogspot.com/2011/04/optimizing-merge-sort.html
 */

#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Merge Sort.
 * Same interface as for \c qsort().
 */
int
msort (void *base, size_t nmemb, size_t size,
       int (* cmp) (const void *, const void *));

#ifdef __cplusplus
}
#endif
