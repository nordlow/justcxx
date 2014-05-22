/*!
 * \file qsort_mt.h
 * \brief Multi-Threaded Quicksort (qsort).
 * \date 2007-10-12 10:19
 */

#pragma once
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int cmp_function_t(const void *, const void *);

/*! Defining the following macro will cause all pthreads API invocation
 * to be checked, against invocation errors (e.g. trying to lock an
 * uninitialized mutex.  Other errors (e.g. unavailable resources) are
 * always checked and acted upon.
 */
#define DEBUG_API 0

/*! Defining the followin macro will print on stderr the results of various sort
 * phases.
 */
#define DEBUG_SORT 0

/*! Defining the following macro will produce logging information on
 * the algorithm's progress
 */
#define DEBUG_LOG 0

/*! Multi-Threaded Quicksort.
 * Interface identical to \c qsort() but with two extra parameters
 * \p maxthreads and \p forkelem added.
 * \param maxthreads Maximum number of threads. Use 0 to default to
 *                   number of CPUs online.
 * \param forkelem Minimum number of elements for a new thread. Use 0
 *                 to use internal default value.
 */
void qsort_mt(void *base, size_t nmemb, size_t size,
	      int(*compar)(const void *, const void *),
	      unsigned int maxthreads, unsigned int forkelem);

#ifdef __cplusplus
}
#endif
