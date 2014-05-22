/*! \file substr_match.h
 * \brief SubString Matching Functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#include "extremes.h"
#include "xstr.h"
#include "stdio_x.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Check if \p dat ends with \p end (case-sensitive).
 * \return match length upon match, 0 otherwise.
 */
static inline pure int
cstr_end(const char * dat, const char * end)
{
  size_t n = strlen(dat);
  size_t nE = strlen(end);
  return (n >= nE &&
	  memcmp(&dat[n-nE], end, nE) == 0);
}

/*! Check if \p dat of length \p len, ends with \p end of length \p nE.
 * \return \p nE upon match, 0 otherwise.
 */
static inline pure int
cbuf_ends(const char * dat, size_t n, const char * end, size_t nE)
{
#ifndef NDEBUG
  if (strlen(end) != nE) { PTEST("strlen(end) != nE\n"); }
#endif
  return (n >= nE && strncasecmp(&dat[n-nE], end, nE) == 0) ? nE : 0;
}

/*! Check if \p dat, ends with \p end of length \p nE.
 * \return \p nE upon match, 0 otherwise.
 */
static inline pure int
xstr_end(const xStr * dat, const char * end, size_t nE)
{
  if (dat->wcF) {
    PERR("Handle this case!\n");
    return 0;
  } else {
    return cbuf_ends(xstr_get_buf(dat), dat->l, end, nE);
  }
}

/*! Check if \p dat ends (case-insensitive) with \p end.
 * \return match length upon match, 0 otherwise.
 */
static inline pure int
cstr_iend(const char * dat, const char * end)
{
  size_t n = strlen(dat);
  size_t nE = strlen(end);
  return (n >= nE && strncasecmp(&dat[n-nE], end, nE) == 0) ? nE : 0;
}

/*! Check if \p dat of length \p len, ends (case-sensitive) with \p end of length \p nE.
 * \return \p nE upon match, 0 otherwise.
 */
static inline pure int
cbuf_iends(const char * dat, size_t n, const char * end, size_t nE)
{
#ifndef NDEBUG
  if (strlen(end) != nE) { PTEST("strlen(end) != nE\n"); }
#endif
  return (n >= nE && strncasecmp(&dat[n-nE], end, nE) == 0) ? nE : 0;
}

/*! Check if \p dat, ends (case-sensitive) with \p end of length \p nE.
 */
static inline pure int
xstr_iend(const xStr * dat, const char * end, size_t nE)
{
  if (dat->wcF) {
    PERR("Handle this case!\n"); return 0;
  } else {
    return cbuf_iends(xstr_get_buf(dat), dat->l, end, nE);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Return non-zero if \p dat of length \p n begins with \p beg of
 * length \p nB.
 * \return \p nB upon match, 0 otherwise.
 */
static inline pure size_t
cbuf_begins(const char * dat, size_t n, const char * beg, size_t nB)
{
  return (n >= nB && memcmp(&dat[0], beg, nB) == 0) ? nB : 0;
}

/*! Check if \p dat, begins with \p end of length \p nE.
 * \return \p nE upon match, 0 otherwise.
 */
static inline pure size_t
xstr_beg(const xStr * dat, const char * end, size_t nE)
{
  if (dat->wcF) {
    PERR("Handle this case!\n");
    return 0;
  } else {
    return cbuf_begins(xstr_get_buf(dat), dat->l, end, nE);
  }
}

/*! Return non-zero if \p dat of length \p n begins (case-insensitive)
 * with \p beg of length \p nB.
 * \return \p nB upon match, 0 otherwise.
 */
static inline pure int
cbuf_ibegins(const char * dat, size_t n, const char * beg, size_t nB)
{
  return (n >= nB && strncasecmp(&dat[0], beg, nB) == 0) ? nB : 0;
}

/*! Check if \p dat, begins (case-sensitive) with \p end of length \p nE.
 * \return \p nB upon match, 0 otherwise.
 */
static inline pure int
xstr_ibeg(const xStr * dat, const char * end, size_t nE)
{
  if (dat->wcF) {
    PERR("Handle this case!\n");
    return 0;
  } else {
    return cbuf_ibegins(xstr_get_buf(dat), dat->l, end, nE);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Check if \p dat of length \p n begins with \p beg of length \p nB.
 * \return number of matched characters.
 */
static inline pure size_t
cbuf_findBeg(const char * dat, size_t n, const char * beg, size_t nB)
{
  n = MIN2(n, nB);
  size_t i;
  for (i = 0; i < nB && dat[i] == beg[i]; i++);
  return i;
}

/*! Check if \p dat of length \p n begins (case-insensitive) with @p
 * beg of \p length nB.
 * \return number of matched characters.
 */
static inline pure size_t
cbuf_ifindBeg(const char * dat, size_t n, const char * beg, size_t nB)
{
  n = MIN2(n, nB);
  size_t i;
  for (i = 0; i < nB && tolower(dat[i]) == tolower(beg[i]); i++);
  return i;
}

#ifdef __cplusplus
}
#endif
