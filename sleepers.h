/*!
 * \file sleepers.h
 * \brief Wrappers for platform independent sleeps.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Some code calls select with all three sets empty, n zero,
 * and a non-null timeout as a fairly portable way to sleep
 * with subsecond precision.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

#ifdef __linux__
#include <time.h>
/*!
 * Sleep for n microseconds.
 */
static inline int
pnw_usleep(ulong n)
{
  struct timespec req, rem;
  req.tv_sec = 0;
  req.tv_nsec = 1000 * n;
  return nanosleep(&req, &rem);
}
#else
/*!
 * Sleep for n microseconds.
 */
static inline int
pnw_usleep(ulong n)
{
  return usleep(n);
}
#endif

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
