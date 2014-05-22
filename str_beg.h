/*!
 * \file str_beg.h
 * \brief Check whether char buffer begins with fixed number of characters.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "xstr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Check whether \p a begins with \p ch.
 */
static inline int
cbuf_begins1(const char * a, char ch)
{
  return (a[0] == ch);
}

/*!
 * Check whether \p a begins with [\p ch0 \p ch1].
 */
static inline int
cbuf_begins2(const char * a, char ch0, char ch1)
{
  return (a[0] == ch0 &&
	  a[1] == ch1);
}

/*!
 * Check whether \p a begins with [\p ch0 \p ch1 \p ch2].
 */
static inline int
cbuf_begins3(const char * a, char ch0, char ch1, char ch2)
{
  return (a[0] == ch0 &&
	  a[1] == ch1 &&
	  a[2] == ch2);
}

/*!
 * Check whether \p a begins with [\p ch0 \p ch1 \p ch2 \p ch3].
 */
static inline int
cbuf_begins4(const char * a, char ch0, char ch1, char ch2, char ch3)
{
  return (a[0] == ch0 &&
	  a[1] == ch1 &&
	  a[2] == ch2 &&
	  a[3] == ch3);
}

/*!
 * Check whether \p a begins with [\p ch0 \p ch1 \p ch2 \p ch3 \p ch4].
 */
static inline int
cbuf_begins5(const char * a, char ch0, char ch1, char ch2, char ch3, char ch4)
{
  return (a[0] == ch0 &&
	  a[1] == ch1 &&
	  a[2] == ch2 &&
	  a[3] == ch3 &&
	  a[4] == ch4);
}

/*!
 * Check whether \p a begins with [\p ch0 \p ch1 \p ch2 \p ch3 \p ch4 \p ch5].
 */
static inline int
cbuf_begins6(const char * a,
          char ch0, char ch1, char ch2, char ch3, char ch4, char ch5)
{
  return (a[0] == ch0 &&
	  a[1] == ch1 &&
	  a[2] == ch2 &&
	  a[3] == ch3 &&
	  a[4] == ch4 &&
	  a[5] == ch5);
}

/*!
 * Check whether \p a begins with [\p ch0 \p ch1 \p ch2 \p ch3 \p ch4 \p ch5 \p ch6].
 */
static inline int
cbuf_begins7(const char * a,
          char ch0, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6)
{
  return (a[0] == ch0 &&
	  a[1] == ch1 &&
	  a[2] == ch2 &&
	  a[3] == ch3 &&
	  a[4] == ch4 &&
	  a[5] == ch5 &&
	  a[6] == ch6);
}

/*!
 * Check whether \p a begins with [\p ch0 \p ch1 \p ch2 \p ch3 \p ch4 \p ch5 \p ch6 \p ch7].
 */
static inline int
cbuf_begins8(const char * a,
          char ch0, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6, char ch7)
{
  return (a[0] == ch0 &&
	  a[1] == ch1 &&
	  a[2] == ch2 &&
	  a[3] == ch3 &&
	  a[4] == ch4 &&
	  a[5] == ch5 &&
	  a[6] == ch6 &&
	  a[7] == ch7);
}

/*!
 * Check whether \p a begins with [\p ch0 \p ch1 \p ch2 \p ch3 \p ch4 \p ch5 \p ch6 \p ch7 \p ch8].
 */
static inline int
cbuf_begins9(const char * a,
          char ch0, char ch1, char ch2, char ch3, char ch4,
          char ch5, char ch6, char ch7, char ch8, char ch9)
{
  return (a[0] == ch0 &&
	  a[1] == ch1 &&
	  a[2] == ch2 &&
	  a[3] == ch3 &&
	  a[4] == ch4 &&
	  a[5] == ch5 &&
	  a[6] == ch6 &&
	  a[7] == ch7 &&
	  a[8] == ch8 &&
	  a[9] == ch9);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Check whether \p a has a length >= 1 and begins with \p ch.
 */
static inline int
xstr_beg1(const xStr * a, char ch)
{
  int ret = 0;
  if (a->l >= 1) {               /* of correct length */
    if (a->wcF) {
      ret = (a->p.wch[0] == ch);
    } else {
      ret = (a->p.ch[0] == ch);
    }
  }
  return ret;
}

/*!
 * Check whether \p a has a length >= 2 and begins with [\p ch0 \p ch1].
 */
static inline int
xstr_beg2(const xStr * a, char ch0, char ch1)
{
  int ret = 0;
  if (a->l >= 2) {               /* of correct length */
    if (a->wcF) {
      ret = (a->p.wch[0] == ch0 && a->p.wch[1] == ch1);
    } else {
      ret = (a->p.ch[0] == ch0 && a->p.ch[1] == ch1);
    }
  }
  return ret;
}

/*!
 * Check whether \p a has a length >= 3 and begins with [\p ch0 \p ch1 \p ch2].
 */
static inline int
xstr_beg2(const xStr * a, char ch0, char ch1, char ch2)
{
  int ret = 0;
  if (a->l >= 2) {               /* of correct length */
    if (a->wcF) {
      ret = (a->p.wch[0] == ch0 && a->p.wch[1] == ch1 && a->p.wch[2] == ch2);
    } else {
      ret = (a->p.ch[0] == ch0 && a->p.ch[1] == ch1 && a->p.ch[2] == ch2);
    }
  }
  return ret;
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
