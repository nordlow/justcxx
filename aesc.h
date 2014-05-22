/*!
 * \file aesc.h
 * \brief ANSI/VT100 Terminal Control Escape Sequences or ANSI Escape Codes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://www.termsys.demon.co.uk/vtansi.htm
 */

#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! ANSI Escape Codes: Display Attributes. */
typedef enum {
  AESC_Reset_All = 0,       /**< Reset All. */
  AESC_Bright = 1,          /**< Bright. */
  AESC_Dim = 2,             /**< Dim. */
  AESC_Underscore = 4,      /**< Underscore. */
  AESC_Blink = 5,           /**< Blink. */
  AESC_Reverse = 7,         /**< Reverse. */
  AESC_Hidden = 8,          /**< Hidden. */

  /** \name Foreground Colours  */
  /* @{ */
  AESC_FG_Black = 30,
  AESC_FG_Red = 31,
  AESC_FG_Green = 32,
  AESC_FG_Yellow = 33,
  AESC_FG_Blue = 34,
  AESC_FG_Magenta = 35,
  AESC_FG_Cyan = 36,
  AESC_FG_White = 37,
  /* @} */

  /** \name Background Colours */
  /* @{ */
  AESC_BG_Black = 40,
  AESC_BG_Red = 41,
  AESC_BG_Green = 42,
  AESC_BG_Yellow = 43,
  AESC_BG_Blue = 44,
  AESC_BG_Magenta = 45,
  AESC_BG_Cyan = 46,
  AESC_BG_White = 47,
  /* @} */
} __attribute__((packed)) AESC_DATTR_t;

/*! Initialize support ANSI Escape Control Codes by
 * checking current terminal (environment variable TERM) and remember this
 * state for check in AESC_isSupported().
 */
int AESC_init(void);

/*! Check current terminal support for ANSI Escape Control Codes.
 * \return 1 if support present, 0 otherwise.
 */
int AESC_isSupported(void);

/*! If terminal support it build an ANSI Escape Control Sequence from
 * the Display Attribute \p a and print it to \p stream.
 */
void AESC_DATTR_fprint_1(FILE * stream,
                         AESC_DATTR_t a);

/*! If terminal support it build an ANSI Escape Control Sequence from
 * the Display Attribute \p a and \p b and print it to \p stream.
 */
void AESC_DATTR_fprint_2(FILE * stream,
                         AESC_DATTR_t a,
                         AESC_DATTR_t b);

/*! If terminal support it build an ANSI Escape Control Sequence from
 * the Display Attribute \p a, \p b and \p c and print it to \p stream.
 */
void AESC_DATTR_fprint_3(FILE * stream,
                         AESC_DATTR_t a,
                         AESC_DATTR_t b,
                         AESC_DATTR_t c);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
