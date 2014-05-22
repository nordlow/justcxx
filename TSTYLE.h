/*!
 * \file TSTYLE.h
 * \brief Text Style.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#include "xstr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Text \em Style \em Masks.
 */
/* @{ */
#define TSTYLE_NONE       (0 << 0) /**< None. */
#define TSTYLE_BOLD       (1 << 0) /**< Bold. */
#define TSTYLE_ITALIC     (1 << 1) /**< Italic. */
#define TSTYLE_UNDERLINED (1 << 2) /**< Underlined. */
#define TSTYLE_FIXED      (1 << 3) /**< Fixed Width if 1, Variable Width if 0. */
#define TSTYLE_SERIF      (1 << 4) /**< With Serifs if 1, Sans Serif if 0. */
/* @} */

/*!
 * Text \em Style (bit_ored Masks).
 */
typedef uint8_t TSTYLE_t;

/*!
 * Lookup Text Style name as a C-string.
 * \return C-string, 0 NULL if type was unknown.
 */
static inline int
TSTYLE_getName(TSTYLE_t tstyle, xStr * name_ret)
{
  int ret = 0;
  xstr_init(name_ret);
  if (TSTYLE_BOLD & tstyle) { xstr_app_cstr(name_ret, "Bold,"); ret += 1; }
  if (TSTYLE_ITALIC & tstyle) { xstr_app_cstr(name_ret, "Italic"); ret += 1; }
  if (TSTYLE_ITALIC & tstyle) { xstr_app_cstr(name_ret, "Underlined"); ret += 1; }
  if (TSTYLE_FIXED & tstyle) { xstr_app_cstr(name_ret, "Fixed"); ret += 1; }
  if (TSTYLE_SERIF & tstyle) { xstr_app_cstr(name_ret, "Serif"); ret += 1; }
  return ret;
}

/*!
 * Modify the tstyle \p a with the new tstyle \p b.
 * \return the modified tstyle
 */
static inline TSTYLE_t TSTYLE_set(TSTYLE_t a, TSTYLE_t b) { return a | b; }

/*!
 * Modify the tstyle \p a with the new tstyle \p b.
 * \return the modified tstyle
 */
static inline TSTYLE_t TSTYLE_clr(TSTYLE_t a, TSTYLE_t b) { return a & ~b; }

/*!
 * Modify the tstyle \p a with the new tstyle \p b.
 * \return the modified tstyle
 */
static inline TSTYLE_t TSTYLE_mod(TSTYLE_t a, TSTYLE_t b, int set_flag)
{
  if (set_flag) {
    return TSTYLE_set(a, b);
  } else {
    return TSTYLE_clr(a, b);
  }
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
