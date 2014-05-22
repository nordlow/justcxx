/*! \file timing.h
 * \brief Time and Timer.
 *
 * Most functions implemented as inline functions for minimum \em overhead in profile-result.
 * See: http://stackoverflow.com/questions/275004/c-timer-function-to-provide-time-in-nano-seconds
 * See: http://www.google.se/search?sourceid=chrome&ie=UTF-8&q=boost::timer
 */

#pragma once
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include "cc_features.h"

#if defined(HAVE_CONFIG_H)
#  include "config.h"
#endif
#if defined(HAVE_INTTYPES_H)
#  include <inttypes.h>
#endif
#if defined(HAVE_STDINT_H)
#  include <stdint.h>
#endif
#if defined(HAVE_STDBOOL_H)
#  include <stdbool.h>
#endif
#ifdef VXWORKS
#  include "pnw_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Read the on-chip High Performance \em Timer (CPU Clock Counter).
 * See: http://stackoverflow.com/questions/275004/c-timer-function-to-provide-time-in-nano-seconds
 */
static inline uint64_t rdtsc(void)
{
#if DIGITAL_MARS
// Digital Mars C++:
  uint64_t rdtsc() { _asm { rdtsc } }
#else
#define RDTSC(low, high) __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))
  uint32_t low, high;
  RDTSC(low, high);
  return ((uint64_t)high << 32) | low;
#endif
}

/* ========================================================================= */

/*! Time
 *
 * \TODO: For windows you want to use the \c QueryPerformanceCounter.
 * And here is more on \c QPC: http://msdn.microsoft.com/en-us/library/ms979201.aspx
 *
 * Representation of time.
 */
typedef struct timespec pTime;

/* ---------------------------- Group Separator ---------------------------- */

/*! Initialize time \p a with \p sec seconds and \p msec milliseconds. */
static inline void ptime_init_sec_msec(pTime * a, uint32_t sec, uint32_t msec) {
  a->tv_sec  = sec;
  a->tv_nsec = msec * 1e6;
}

/*! Initialize time \p a with \p sec seconds and \p usec microseconds. */
static inline void ptime_init_sec_usec(pTime * a, uint32_t sec, uint32_t usec) {
  a->tv_sec  = sec;
  a->tv_nsec = usec * 1e3;
}

/*! Initialize time \p a with \p sec seconds and \p nsec nanoseconds. */
static inline void ptime_init_sec_nsec(pTime * a, uint32_t sec, uint32_t nsec) {
  a->tv_sec  = sec;
  a->tv_nsec = nsec;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Initialize time with \p msec milliseconds. */
static inline void ptime_init_msec(pTime * a, uint32_t msec) {
  uint32_t sec  = msec / 1e3;	/* whole seconds */
  msec    -= sec * 1e3;		/* partial second milliseconds */
  ptime_init_sec_nsec(a, sec, msec * 1e6); /* init duration */
}

/*! Initialize time with \p usec microseconds. */
static inline void ptime_init_usec(pTime * a, uint32_t usec) {
  uint32_t sec  = usec / 1e6;	/* whole seconds */
  usec	  -= sec * 1e6;		/* partial second microsecondss */
  ptime_init_sec_nsec(a, sec, usec * 1e3);	/* init duration */
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Initialize time \p a as being undefined.*/
static inline void ptime_init_undefined(pTime * a) {
  a->tv_sec  = 0;
  a->tv_nsec = 0;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Read System-wide Realtime Clock Time into \p a.
 *
 * \return 0 on success, -1 on failure (in which case errno is set
 *         appropriately).
 */
static inline int ptime_read_CLOCK(pTime * a, clockid_t clk_id) { return clock_gettime(clk_id, a); }

/* ---------------------------- Group Separator ---------------------------- */

/*! Read System-wide \em Realtime Clock Time into \p a. */
static inline int ptime_read_CLOCK_REALTIME(pTime * a) { return clock_gettime(CLOCK_REALTIME, a); }

/**< NOTE: VxWorks currenly only supports CLOCK_REALTIME */
#ifndef VXWORKS

/*! Read \em Monotonic Clock Time into \p a.
 *
 * Clock that cannot be set and represents monotonic time since some
 * unspecified starting point.
 */
static inline int ptime_read_CLOCK_MONOTONIC(pTime * a) { return clock_gettime(CLOCK_MONOTONIC, a); }

/*! Read \em Process-Specific Timer from CPU into \p a. */
static inline int ptime_read_CLOCK_PROCESS_CPUTIME_ID(pTime * a) { return clock_gettime(CLOCK_PROCESS_CPUTIME_ID, a); }

/*! Read \em Thread-Specific Timer from CPU into \p a. */
static inline int ptime_read_CLOCK_THREAD_CPUTIME_ID(pTime * a) { return clock_gettime(CLOCK_THREAD_CPUTIME_ID, a); }

#endif

/* ---------------------------- Group Separator ---------------------------- */

/*! Read \em Resolution of System-wide \em Realtime Clock Time into \p a. */
static inline int ptime_read_resolution_CLOCK_REALTIME(pTime * a) { return clock_getres(CLOCK_REALTIME, a); }

/* ---------------------------- Group Separator ---------------------------- */

/*! Set System Time to \p a.
 *
 * \NOTE Only for superusers!
 * \WARNING Modifies system time. Be careful!
 */
static inline int ptime_write(const pTime * a) { return clock_settime(CLOCK_REALTIME, a); }

/* ---------------------------- Group Separator ---------------------------- */

/*! Get Fractional number of seconds in \p a as whole milliseconds. */
static inline uint32_t ptime_get_frac_msec(pTime a) { return a.tv_nsec / 1e6; }
/*! Get Fractional number of seconds in \p a as whole microseconds. */
static inline uint32_t ptime_get_frac_usec(pTime a) { return a.tv_nsec / 1e3; }
/*! Get Fractional number of seconds in \p a as whole nanoseconds. */
static inline uint32_t ptime_get_frac_nsec(pTime a) { return a.tv_nsec; }

/* ---------------------------- Group Separator ---------------------------- */

/*! Get internal seconds of \p a. */
static inline pure double ptime_get_sec(pTime a) {
  return ((double)a.tv_sec +
          (double)a.tv_nsec * 1e-9);
}

/*! Get internal seconds of \p a. */
static inline pure float ptime_get_secf(pTime a) {
  return ((float)a.tv_sec +
          (float)a.tv_nsec * 1e-9f);
}

/*! Get internal milliseconds of \p a. */
static inline pure double ptime_get_msec(pTime a) {
  return ((double)a.tv_sec * 1e3 +
          (double)a.tv_nsec * 1e-6);
}

/*! Get internal milliseconds of \p a. */
static inline pure float ptime_get_msecf(pTime a) {
  return ((float)a.tv_sec * 1e3f +
          (float)a.tv_nsec * 1e-6f);
}

/*! Get internal microseconds of \p a. */
static inline pure uint64_t ptime_get_usec(pTime a) {
  return (((uint64_t) a.tv_sec * 1e6) +
          ((uint64_t) a.tv_nsec * 1e-3));
}

/*! Get internal nanoseconds of \p a. */
static inline pure uint64_t ptime_get_nsec(pTime a) {
  return (((uint64_t) a.tv_sec * 1e9) +
          ((uint64_t) a.tv_nsec));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Add \p sec seconds and \p nsec nanoseconds to the time \p a.
 * \return 0 upon success, -1 on overflow.
 */
int ptime_add_sec_nsec(pTime * a, uint32_t sec, uint32_t nsec);

/*! Add \p sec seconds and \p usec microseconds to the time \p a.
 * \return 0 upon success, -1 on overflow.
 */
static inline int ptime_add_sec_usec(pTime * a, uint32_t sec, uint32_t usec) {
  return ptime_add_sec_nsec(a, sec, usec * 1e3);
}

/*! Add \p b to the time \p a.
 * \return 0 upon success, -1 on overflow.
 */
static inline int ptime_add(pTime * a, const pTime * b) {
  return ptime_add_sec_nsec(a, b->tv_sec, b->tv_nsec);
}

/*! Add \p msec milliseconds to the time \p a.
 * \return 0 upon success, -1 on overflow.
 */
static inline int ptime_add_msec(pTime * a, int msec) {
  time_t sec  = msec / 1e3;	/* whole seconds */
  msec       -= sec * 1e3;	/* partial second milliseconds */
  return ptime_add_sec_usec(a, sec, msec * 1e3); /* add it */
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Subtract \p sec seconds and \p nsec nanoseconds from the time \p a.
 * \return 0 upon success, -1 on underflow.
 */
int ptime_sub_sec_nsec(pTime * a, uint32_t sec, uint32_t nsec);

/*! Subtract \p sec seconds and \p usec microseconds from the time \p a.
 * \return 0 upon success, -1 on underflow.
 */
static inline int ptime_sub_sec_usec(pTime * a, uint32_t sec, uint32_t usec) {
  return ptime_sub_sec_nsec(a, sec, usec * 1e3);
}

/*! Subtract \p b from the time \p a. Calculates \p a - \p b.
 * \return 0 upon success, -1 on overflow.
 */
static inline int ptime_sub(pTime * a, const pTime * b) {
  return ptime_sub_sec_nsec(a, b->tv_sec, b->tv_nsec);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Span from \p a to \p b and put the result in \p span. */
void ptime_rdSpan(pTime a, pTime b, pTime * span);

/*! Span from \p a to \p b in seconds. */
static inline double ptime_getSpan_sec(pTime a, pTime b) {
  pTime span; ptime_rdSpan(a, b, &span);
  return ptime_get_sec(span);
}

/*! Span from \p a to \p b in milliseconds. */
static inline float ptime_getSpan_msecf(pTime a, pTime b) {
  pTime span; ptime_rdSpan(a, b, &span);
  return ptime_get_msec(span);
}

/*! Sum \p a and \p b and put the result in \p c. */
static inline void ptime_sum(pTime a, pTime b, pTime * c) {
  *c = a;
  ptime_add_sec_nsec(c, b.tv_sec, b.tv_nsec);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Time Equality of \p a and \p b.
 * \return 1 if \p a and \p b are equal, 0 otherwise.
 */
static inline int ptime_eq(const pTime * a, const pTime * b) {
  return (a->tv_sec  == b->tv_sec &&
          a->tv_nsec == b->tv_nsec);
}

/*! Time Non-Equality of \p a and \p b.
 * \return 1 if \p a and \p b are not equal, 0 otherwise.
 */
static inline int ptime_neq(const pTime * a, const pTime * b) {
  return (a->tv_sec  != b->tv_sec ||
          a->tv_nsec != b->tv_nsec);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Check whether \p a is less than \p b.
 * \return 1 if \p a lies before \p b in time, 0 otherwise.
 */
static inline int ptime_lt(const pTime * a, const pTime * b) {
  return ((a->tv_sec < b->tv_sec) ||
          ((a->tv_sec == b->tv_sec) &&
           (a->tv_nsec < b->tv_nsec)));
}

/*! Check whether \p a is greater than \p b.
 * \return 1 if \p a lies after \p b in time, 0 otherwise.
 */
static inline int ptime_gt(const pTime * a, const pTime * b) {
  return ((a->tv_sec > b->tv_sec) ||
          ((a->tv_sec == b->tv_sec) &&
           (a->tv_nsec > b->tv_nsec)));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Check whether \p a is less than or equal \p b.
 * \return 1 if \p a lies before or equal to \p b in time, 0 otherwise.
 */
static inline int ptime_lte(const pTime * a, const pTime * b) {
  return (ptime_lt(a, b) ||
          ptime_eq(a, b));
}

/*! Check whether \p a is greater than or equal \p b.
 * \return 1 if \p a lies after or equal to \p b in time, 0 otherwise.
 */
static inline int ptime_gte(const pTime * a, const pTime * b) {
  return (ptime_gt(a, b) ||
          ptime_eq(a, b));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Time Compare of \p a and \p b.
 *
 * \return 0 if \p a and \p b are equal, -1 if \p a is before \p b,
 * +1 if \p a is after \p b.
 */
static inline int ptime_cmp(const pTime * a, const pTime * b) {
  if (ptime_lt(a, b)) {
    return -1;
  } else if (ptime_lt(b, a)) {
    return 1;
  } else {
    return 0;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Compute Minimum of \p a and \p b and put result in \p c.
 */
static inline void ptime_min(const pTime * a, const pTime * b, pTime * c) {
  if (ptime_lt(a, b)) { *c = *a; } else { *c = *b; }
}

/*! Compute Maximum of \p a and \p b and put result in \p c.
 */
static inline void ptime_max(const pTime * a, const pTime * b, pTime * c) {
  if (ptime_gt(a, b)) { *c = *a; } else { *c = *b; }
}

/*! Compute Minimum and Maximum of \p a and \p b
 * and put minimum in \p c and maximum in \p d.
 */
static inline void ptime_minmax(const pTime * a, const pTime * b,
                                pTime * c, pTime * d) {
  if (ptime_lt(a, b)) {
    *c = *a;
    *d = *b;
  } else {
    *c = *b;
    *d = *a;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Check whether time is defined.
 * \return 1 if \p a is defined, 0 otherwise.
 */
static inline int ptime_is_defined(const pTime * a) {
  return (a->tv_sec  != 0 &&
          a->tv_nsec != 0);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Print Time \p a to \p stream in the format sec.nsec where \em sec
 * is the seconds and \em nsec is the nanoseconds zero-filled to @em
 * nine decimal digits.
 */
static inline void ptime_fprint_sec_usec9(FILE * stream, const pTime * a) {
  fprintf(stream, "%ld.%09d", (time_t)a->tv_sec, (int)a->tv_nsec);
}

/*!
 * Print Time \p a to \c stdout.
 * \see \c ptime_fprint_sec_usec9()
 */
static inline void ptime_print_sec_usec9(const pTime * a) {
  ptime_fprint_sec_usec9(stdout, a);
}

/*! Print Time \p a to \p stream into the format
 * YYYY-MM-DD_HH:MM:SS.NNN:NNN:NNN.
 *
 * \param[in] local_flag should be set (non-zero) if we want local
 *       time, otherwise we get UTC-time.
 *
 * \example 2007-01-01_12:00:00.000000000 means first of January 2007
 *          at exactly twelve oclock.
 */
void ptime_fprint_YYYY_MM_DD_HMSN(FILE * stream, const pTime * a, int local_flag);

/* ---------------------------- Group Separator ---------------------------- */

/*! Timer. *
 * StopWatch that keeps track of begin (\c tic) and end time (\c toc).
 */
typedef struct {
  pTime      tic;		/**< Timer start time. */
  pTime      toc;		/**< Timer stop time. */
  clockid_t clk_id; /**< Type of time measurement. */
} pTimer;

/*! Initialize timer with clock type \p clk_id. */
static inline void ptimer_init(pTimer * a, clockid_t clk_id) { a->clk_id = clk_id; }

static inline void ptimer_init_REALTIME(pTimer * a) { ptimer_init(a, CLOCK_REALTIME); }

/*! Start timer \p a. */
static inline void ptimer_tic(pTimer * a) { ptime_read_CLOCK(&a->tic, a->clk_id); }

/*! Stop timer \p a. */
static inline void ptimer_toc(pTimer * a) { ptime_read_CLOCK(&a->toc, a->clk_id); }

/*! Get timer \p a value in \em seconds (double precision). */
static inline pure double ptimer_get_sec(const pTimer * a) {
  return (((double) a->toc.tv_sec + (double) a->toc.tv_nsec * 1e-9) -
          ((double) a->tic.tv_sec + (double) a->tic.tv_nsec * 1e-9)) ;
}
/*! Get timer \p a value in \em seconds (float precision). */
static inline pure float ptimer_get_secf(const pTimer * a) {
  return (((float) a->toc.tv_sec + (float) a->toc.tv_nsec * 1e-9f) -
          ((float) a->tic.tv_sec + (float) a->tic.tv_nsec * 1e-9f)) ;
}

/*! Get timer \p a value in exact \em microseconds. */
static inline pure uint64_t ptimer_get_usec(const pTimer * a) {
  return ((((uint64_t) a->toc.tv_sec * 1e6) + (uint64_t) a->toc.tv_nsec / 1e3) -
          (((uint64_t) a->tic.tv_sec * 1e6) + (uint64_t) a->tic.tv_nsec / 1e3)) ;
}
/*! Get timer \p a value in exact \em nanoseconds. */
static inline uint64_t ptimer_get_nsec(const pTimer * a) {
  return ((((uint64_t) a->toc.tv_sec * 1e9) + (uint64_t) a->toc.tv_nsec) -
          (((uint64_t) a->tic.tv_sec * 1e9) + (uint64_t) a->tic.tv_nsec)) ;
}

/*! Get timer \p a value in \em minutes. */
static inline double ptimer_get_min(const pTimer * a) {
  return ptimer_get_sec(a) / 60;
}
/*! Get timer \p a value in \em hours. */
static inline double ptimer_get_hour(const pTimer * a) {
  return ptimer_get_sec(a) / (60 * 60);
}

/*! Read span of \p a into \p span. */
static inline void ptimer_rdSpan(const pTimer * a, pTime * span) {
  ptime_rdSpan(a->tic, a->toc, span);
}

/*! Get span of \p a. */
static inline pTime ptimer_getSpan(pTimer a) {
  pTime span;
  ptime_rdSpan(a.tic, a.toc, &span);
  return span;
}

/*! Get span of \p a in seconds. */
static inline double ptimer_getSpan_sec(pTimer a) {
  return ptime_get_sec(ptimer_getSpan(a));
}

/*! Read timer \p a into \p s seconds and \p u microseconds. */
static inline void
ptimer_rdSpan_sec_usec(pTimer a, int * s, int * u)
{
  pTime span;
  ptime_rdSpan(a.tic, a.toc, &span);
  *s = span.tv_sec;
  *u = span.tv_nsec / 1e3;
}

/*! Read timer \p a into \p sec seconds and \p nsec nanoseconds. */
static inline void
ptimer_rdSpan_sec_nsec(pTimer a, int * sec, int * nsec)
{
  pTime span;
  ptime_rdSpan(a.tic, a.toc, &span);
  *sec = span.tv_sec;
  *nsec = span.tv_nsec / 1e3;
}

/*! Print timer \p a to \p stream in the format sec.nsec where \p sec
 * is the seconds and \p nsec is the microseconds zero-filled to @p
 * nine decimal digits.
 */
static inline void
ptimer_fprint_sec_usec9(FILE * stream, pTimer a)
{
  pTime span;
  ptime_rdSpan(a.tic, a.toc, &span);
  fprintf(stream, "%ld.%09ld", span.tv_sec, span.tv_nsec);
}

/*! Print timer \p a to \c stdout in the format s.u where \p s is the seconds
 * and \p u is the microseconds zero-filled to \em six decimal digits.
 */
static inline void ptimer_print_sec_usec9(pTimer a)
{
  ptimer_fprint_sec_usec9(stdout, a);
}

#ifdef __cplusplus
}
#endif
