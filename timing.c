#include "timing.h"
#include "extremes.h"
#include "stdio_x.h"
#include <limits.h>

/* ========================================================================= */

int
ptime_add_sec_nsec(pTime * a, uint32_t sec, uint32_t nsec)
{
  time_t old_sec = a->tv_sec; 	/* save current time */

  a->tv_sec += sec;		/* forward seconds */

  nsec = MIN(1e9, nsec);	/* truncate to one billion */
  a->tv_nsec += nsec;		/* forward microseconds */

  if (a->tv_nsec >= 1e9) {	/* adjust for overflow into seconds */
    a->tv_nsec -= 1e9;		/* wrap around microseconds */
    a->tv_sec++;		/* one more second */
  }

  if (a->tv_sec < old_sec) {
    leprintf("overflow truncated to maximum time value\n");
    a->tv_sec = INT_MAX;
    a->tv_nsec = INT_MAX;
    return -1;			/* overflow truncated to maximum */
  } else {
    return 0;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

int
ptime_sub_sec_nsec(pTime * a, uint32_t sec, uint32_t nsec)
{
  time_t old_sec = a->tv_sec; 	/* save current time */

  a->tv_sec -= sec;		/* backward seconds */

  nsec = MIN2(1e9, nsec);	/* truncate to one billion */

  if (nsec > a->tv_nsec) {	/* adjust for underflow into seconds */
    nsec -= a->tv_nsec;		/* wrap around microseconds */
    a->tv_nsec = 1e9 - nsec; /* backward microseconds */
    a->tv_sec--;		/* one less second */
  } else {
    a->tv_nsec -= nsec;
  }

  if (a->tv_sec > old_sec) {
    leprintf("underflow truncated to minimum time value\n");
    a->tv_sec = 0;
    a->tv_nsec = 0;
    return -1;			/* underflow truncated to minimum */
  } else {
    return 0;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
ptime_rdSpan(pTime a, pTime b, pTime * span)
{
  const time_t s1 = a.tv_sec; long n1 = a.tv_nsec;
  const time_t s2 = b.tv_sec; long n2 = b.tv_nsec;
  /* printf("s1:%u n1:%9u\n", s1,n1); */
  /* printf("s2:%u n2:%9u\n", s2,n2); */
  span->tv_sec = s2 - s1;
  if (n1 <= n2) {
    span->tv_nsec = n2 - n1;
  } else {
    span->tv_nsec = 1e9 + n2 - n1;
    span->tv_sec--;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
ptime_fprint_YYYY_MM_DD_HMSN(FILE * stream, const pTime * a, int local_flag)
{
  struct tm tm;
  if (local_flag) { localtime_r(&a->tv_sec, &tm); } else { gmtime_r(&a->tv_sec, &tm); }
  fprintf(stream, "%d-%02d-%02d_%d:%d:%d.%09ld",
	  1900+tm.tm_year, 1+tm.tm_mon, tm.tm_mday,
	  tm.tm_hour, tm.tm_min, tm.tm_sec,
	  a->tv_nsec);
}

/* ---------------------------- Group Separator ---------------------------- */
