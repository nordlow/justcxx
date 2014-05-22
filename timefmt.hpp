/*! \file timefmt.hpp
 * \brief Time Readings and Conversions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

extern "C"
{
#include <sys/time.h>
#include <time.h>
}

/*! Convert \p x seconds (daytime) to days, hours, minutes and seconds.
 * \param[out] d whole days (0 - ...),
 * \param[out] h hours (0 - 23),
 * \param[out] m minutes (0 - 59) and
 * \param[out] s seconds (0 - 59).
 */
inline void timefmt_conv_S_to_DHMS(uint & d,
                                   uint & h,
                                   uint & m,
                                   uint & s,
                                   uint x)
{
    s  = x;
    m  = s / 60;                // minutes
    s -= m * 60;                // rest of seconds
    h  = m / 60;                // hours
    m -= h * 60;                // rest of minutes
    d  = h / 24;                // days
    h -= d * 24;                // rest of hours
}

/*! Convert \p x nanoseconds to milli-, micro- and nanoseconds.
 * \param[out] ms milliseconds (0 - ...),
 * \param[out] us microseconds and (0 - 999),
 * \param[out] ns nanoseconds (0 - 999)
 */
inline void timefmt_conv_N_to_MUN(uint & ms,
                                  uint & us,
                                  uint & ns,
                                  uint x)
{
    ns  = x;
    us  = ns / 1000;            // microseconds
    ns -= us * 1000;            // rest of nanoseconds
    ms  = us / 1000;            // milliseconds
    us -= ms * 1000;            // rest of microseconds
}

/*! Read daytime.
 * \param[out] h hours (0 - 23),
 * \param[out] m minutes (0 - 59) and
 * \param[out] s seconds (0 - 59).
 * \param[out] u seconds (0 - 59).
 * \param[in] local_flag should be set (non-zeros) if we want local time,
 * otherwise we get UTC-time.
 */
inline void daytime_HMSU(uint & h,
                         uint & m,
                         uint & s,
                         uint & u,
                         bool local_flag)
{
    struct timeval tv;
    struct tm tm;
    gettimeofday(&tv, 0);
    if (local_flag) {
        localtime_r(&tv.tv_sec, &tm);
    } else {
        gmtime_r(&tv.tv_sec, &tm);
    }
    h = tm.tm_hour;
    m = tm.tm_min;
    s = tm.tm_sec;
    u = tv.tv_usec;
}
