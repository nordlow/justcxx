/*!
 * \file str_eq.h
 * \brief String Equality of constant lengths.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "xstr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Check if the C-string \p a is empty.
 */
static inline int
cstr_eq0(const char * a)
{
  return (a[0] == '\0');
}

/*!
 * Check if the C-string \p a is of length 1 and equals \p ch.
 */
static inline int
cstr_eq1(const char * a, char ch)
{
#ifndef NDEBUG
  if (ch == '\0') { PERR("some ch was 0!\n"); return 0; }
#endif
  return (a[0] == ch &&		/* first is ch */
	  a[1] == '\0');	/* of correct length */
}

/*!
 * Check if the C-string \p a is of length 2 and equals [\p ch0 \p ch1].
 */
static inline int
cstr_eq2(const char * a, char ch0, char ch1)
{
#ifndef NDEBUG
  if (ch0 == '\0' || ch1 == '\0') { PERR("some ch was 0!\n"); return 0; }
#endif
  return (a[0] == ch0 &&	/* char match */
	  a[1] == ch1 &&	/* char match */
	  a[2] == '\0');	/* of correct length */
}

/*!
 * Check if the C-string \p a is of length 3 and equals [\p ch0 \p ch1 \p ch2].
 */
static inline int
cstr_eq3(const char * a, char ch0, char ch1, char ch2)
{
#ifndef NDEBUG
  if (ch0 == '\0' ||
      ch1 == '\0' ||
      ch2 == '\0') { PERR("some ch was 0!\n"); return 0; }
#endif
  return (a[0] == ch0 &&	/* char match */
	  a[1] == ch1 &&	/* char match */
	  a[2] == ch2 &&	/* char match */
	  a[3] == '\0');	/* of correct length */
}

/*!
 * Check if the C-string \p a is of length 4 and equals [\p ch0 \p ch1 \p ch2 \p ch3].
 */
static inline int
cstr_eq4(const char * a, char ch0, char ch1, char ch2, char ch3)
{
#ifndef NDEBUG
  if (ch0 == '\0' || ch1 == '\0' || ch2 == '\0' || ch3 == '\0') { PERR("some ch was 0!\n"); return 0; }
#endif
  return (a[0] == ch0 &&	/* char match */
	  a[1] == ch1 &&	/* char match */
	  a[2] == ch2 &&	/* char match */
	  a[3] == ch3 &&	/* char match */
	  a[4] == '\0');	/* of correct length */
}

/*!
 * Check if the C-string \p a is of length 5 and equals [\p ch0 \p ch1 \p ch2 \p ch3 \p ch4].
 */
static inline int
cstr_eq5(const char * a, char ch0, char ch1, char ch2, char ch3, char ch4)
{
#ifndef NDEBUG
  if (ch0 == '\0' || ch1 == '\0' || ch2 == '\0' || ch3 == '\0' || ch4 == '\0') { PERR("some ch was 0!\n"); return 0; }
#endif
  return (a[0] == ch0 &&	/* char match */
	  a[1] == ch1 &&	/* char match */
	  a[2] == ch2 &&	/* char match */
	  a[3] == ch3 &&	/* char match */
	  a[4] == ch4 &&	/* char match */
	  a[5] == '\0');	/* of correct length */
}

/*!
 * Check if the C-string \p a is of length 6 and equals [\p ch0 \p ch1 \p ch2 \p ch3 \p ch4 \p ch5].
 */
static inline int
cstr_eq6(const char * a,
       char ch0, char ch1, char ch2, char ch3, char ch4, char ch5)
{
#ifndef NDEBUG
  if (ch0 == '\0' || ch1 == '\0' || ch2 == '\0' || ch3 == '\0' || ch4 == '\0' || ch5 == '\0') { PERR("some ch was 0!\n"); return 0; }
#endif
  return (a[0] == ch0 &&	/* char match */
	  a[1] == ch1 &&	/* char match */
	  a[2] == ch2 &&	/* char match */
	  a[3] == ch3 &&	/* char match */
	  a[4] == ch4 &&	/* char match */
	  a[5] == ch5 &&	/* char match */
	  a[6] == '\0');	/* of correct length */
}

/*!
 * Check if the C-string \p a is of length 7 and equals
 * [\p ch0 \p ch1 \p ch2 \p ch3 \p ch4 \p ch5 \p ch6].
 */
static inline int
cstr_eq7(const char * a,
       char ch0, char ch1, char ch2, char ch3, char ch4, char ch5, char ch6)
{
#ifndef NDEBUG
  if (ch0 == '\0' || ch1 == '\0' || ch2 == '\0' || ch3 == '\0' || ch4 == '\0' || ch5 == '\0' || ch6 == '\0') { PERR("some ch was 0!\n"); return 0; }
#endif
  return (a[0] == ch0 &&	/* char match */
	  a[1] == ch1 &&	/* char match */
	  a[2] == ch2 &&	/* char match */
	  a[3] == ch3 &&	/* char match */
	  a[4] == ch4 &&	/* char match */
	  a[5] == ch5 &&	/* char match */
	  a[6] == ch6 &&	/* char match */
	  a[7] == '\0');	/* of correct length */
}

/*!
 * Check if the C-string \p a is equal to "." or "..".
 */
static inline int
cstr_eq_DorDD(const char * a)
{
  return (cstr_eq1(a, '.') ||
          cstr_eq2(a, '.', '.'));
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Check whether \p a is empty.
 */
static inline int
xstr_eq0(const xStr * a)
{
  return (a->l == 0);
}

/*!
 * Check whether \p a is of length 1 and equals \p ch.
 */
static inline int
xstr_eq1(const xStr * a, char ch)
{
  int ret = 0;
  if (a->l == 1) {               /* of correct length */
    if (a->wcF) {
      ret = (a->p.wch[0] == ch);
    } else {
      ret = (a->p.ch[0] == ch);
    }
  }
  return ret;
}

/*!
 * Check whether \p a is of length 2 and equals [\p ch0 \p ch1].
 */
static inline int
xstr_eq2(const xStr * a, char ch0, char ch1)
{
  int ret = 0;
  if (a->l == 2) {               /* of correct length */
    if (a->wcF) {
      ret = (a->p.wch[0] == ch0 &&
             a->p.wch[1] == ch1);
    } else {
      ret = (a->p.ch[0] == ch0 &&
             a->p.ch[1] == ch1);
    }
  }
  return ret;
}

/*!
 * Check whether \p a is equal to "." or "..".
 */
static inline int
xstr_eq_DorDD(const xStr * a)
{
  return (xstr_eq1(a, '.') ||
	  xstr_eq2(a, '.', '.'));
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
