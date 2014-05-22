/*! \file timefmt.h
 * \brief Time Format Readings and Conversions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <sys/time.h>
#include <time.h>
#include "timing.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Convert \p x seconds (daytime) to \p d days, \p h hours, \p m
 * minutes and \p s seconds.
 * \param[out] d whole days (0 - ...),
 * \param[out] h hours (0 - 23),
 * \param[out] m minutes (0 - 59) and
 * \param[out] s seconds (0 - 59).
 */
static inline void timefmt_conv_S_to_DHMS(uint* d,
                                          uint* h,
                                          uint* m,
                                          uint* s,
                                          uint x)
{
  *s = x;

  *m = *s / 60;                 // minutes
  *s -= *m * 60;                // rest of seconds

  *h = *m / 60;                 // hours
  *m -= *h * 60;                // rest of minutes

  *d = *h / 24;                 // days
  *h -= *d * 24;                // rest of hours
}

/*! Convert \p x nanoseconds to \p ms milli-, \p us micro- and \p ns nanoseconds.
 * \param[out] ms milliseconds (0 - ...),
 * \param[out] us microseconds and (0 - 999999),
 * \param[out] ns nanoseconds (0 - 999999999)
 */
static inline void timefmt_conv_N_to_MUN(uint* ms,
                                         uint* us,
                                         uint* ns,
                                         uint x)
{
  *ns  = x;
  *us  = *ns / 1000;            // microseconds
  *ns -= *us * 1000;            // rest of nanoseconds
  *ms  = *us / 1000;            // milliseconds
  *us -= *ms * 1000;            // rest of microseconds
}

/*! Convert daytime in \p tv to \p h hours, \p m minutes, \p s seconds
 * and \p u microseconds.
 * \param[out] h hours (0 - 23),
 * \param[out] m minutes (0 - 59) and
 * \param[out] s seconds (0 - 59).
 * \param[out] u microseconds (0 - 999999).
 * \param[in] local_flag should be set (non-zero) if we want local time,
 * otherwise we get UTC-time.
 */
static inline void timefmt_conv_timeval_to_HMSU(const struct timeval * tv,
                                                uint* h,
                                                uint* m,
                                                uint* s,
                                                uint* u,
                                                bool local_flag)
{
  struct tm tm;
  if (local_flag) { localtime_r(&tv->tv_sec, &tm); } else { gmtime_r(&tv->tv_sec, &tm); }
  *h = tm.tm_hour;
  *m = tm.tm_min;
  *s = tm.tm_sec;
  *u = tv->tv_usec;
}

/*! Convert daytime in \p tc to \p h hours, \p m minutes, \p s seconds
 * and \p u microseconds.
 * \param[out] h hours (0 - 23),
 * \param[out] m minutes (0 - 59) and
 * \param[out] s seconds (0 - 59).
 * \param[out] n nanoseconds (0 - 999999999).
 * \param[in] local_flag should be set (non-zero) if we want local
 *       time, otherwise we get UTC-time.
 */
static inline void timefmt_conv_timespec_to_HMSN(const struct timespec * tc,
                                                 uint* h,
                                                 uint* m,
                                                 uint* s,
                                                 uint* n,
                                                 bool local_flag)
{
  struct tm tm;
  if (local_flag) { localtime_r(&tc->tv_sec, &tm); } else { gmtime_r(&tc->tv_sec, &tm); }
  *h = tm.tm_hour;
  *m = tm.tm_min;
  *s = tm.tm_sec;
  *n = tc->tv_nsec;
}

/*! Convert daytime in \p tc to \p h hours, \p m minutes, \p s seconds
 * and \p u microseconds.
 *
 * \param[out] Y years,
 * \param[out] M month,
 * \param[out] D day of month,

 * \param[out] h hours (0 - 23),
 * \param[out] m minutes (0 - 59) and
 * \param[out] s seconds (0 - 59).
 * \param[out] n nanoseconds (0 - 999999999).
 *
 * \param[in] local_flag should be set (non-zero) if we want local
 *       time, otherwise we get UTC-time.
 */
static inline void timefmt_conv_timespec_to_YMDHMSN(const struct timespec * tc,
                                                    uint* Y,
                                                    uint* M,
                                                    uint* D,
                                                    uint* h,
                                                    uint* m,
                                                    uint* s,
                                                    uint* n,
                                                    bool local_flag)
{
  struct tm tm;
  if (local_flag) { localtime_r(&tc->tv_sec, &tm); } else { gmtime_r(&tc->tv_sec, &tm); }
  *Y = tm.tm_year;
  *M = tm.tm_mon;
  *D = tm.tm_mday;

  *h = tm.tm_hour;
  *m = tm.tm_min;
  *s = tm.tm_sec;
  *n = tc->tv_nsec;
}

/*! Read daytime into \p h hours, \p m minutes, \p s seconds and
 * \p u microseconds.
 *
 * \param[out] h hours (0 - 23),
 * \param[out] m minutes (0 - 59) and
 * \param[out] s seconds (0 - 59).
 * \param[out] u microseconds (0 - 999999).
 *
 * \param[in] local_flag should be set (non-zero) if we want local
 *       time, otherwise we get UTC-time.
 */
static inline int daytime_HMSU(uint* h,
                               uint* m,
                               uint* s,
                               uint* u,
                               bool local_flag)
{
  struct timeval tv;
  int ret = gettimeofday(&tv, 0);
  timefmt_conv_timeval_to_HMSU(&tv, h, m, s, u, local_flag);
  return ret;
}

/*! Read daytime into \p h hours, \p m minutes, \p s seconds and
 * \p n nanoseconds.
 *
 * \param[out] h hours (0 - 23),
 * \param[out] m minutes (0 - 59) and
 * \param[out] s seconds (0 - 59).
 * \param[out] n nanoseconds (0 - 999999999).
 *
 * \param[in] local_flag should be set (non-zero) if we want local
 *       time, otherwise we get UTC-time.
 */
static inline int daytime_HMSN(uint* h,
                               uint* m,
                               uint* s,
                               uint* n,
                               bool local_flag)
{
  struct timespec tc;
  int ret = clock_gettime(CLOCK_REALTIME, &tc);
  timefmt_conv_timespec_to_HMSN(&tc, h, m, s, n, local_flag);
  return ret;
}

#ifdef __cplusplus
}
#endif
