/*!
 * \file cmatchers.h
 * \brief Matches char patterns.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "ASCII.h"
#include "ISO8859.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Check if \p a is a Windows Style Newline */
static inline bool char_is_CRLF(char a)
{
  return (a == ASCII_CR &&
	  a == ASCII_LF);
}

/*! Check if \p a is a Mac Style Newline */
static inline bool char_is_CR(char a)
{
  return (a == ASCII_CR);
}

/*! Check if \p a is a Unix Style Newline */
static inline bool char_is_LF(char a)
{
  return (a == ASCII_LF);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Check if \p a is a Punctuation Symbol */
static inline bool char_is_PUNCT(char a)
{
  return (a == ASCII_DOT ||
	  a == ASCII_COMMA ||
	  a == ASCII_SEMICOLON ||
	  a == ASCII_COLON);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Match Letter English Lowercase */
static inline bool char_is_LETTER_EN_LOWER(char a)
{
  return (a >= ASCII_a && a <= ASCII_z);
}

/*! Match Letter English Uppercase */
static inline bool char_is_LETTER_EN_UPPER(char a)
{
  return (a >= ASCII_A && a <= ASCII_Z);
}

/*! Match Letter English Uppercase or Lowercase */
static inline bool char_is_LETTER_EN(char a)
{
  return (char_is_LETTER_EN_LOWER(a) ||
          char_is_LETTER_EN_UPPER(a));
}

/*! Match Letter Swedish-Only */
static inline bool char_is_LETTER_ISO8859_SVonly(char a)
{
  return (a == ISO8859_ARING_L ||
          a == ISO8859_AUMLAUT_L ||
          a == ISO8859_OUMLAUT_L ||
          a == ISO8859_ARING_U ||
          a == ISO8859_AUMLAUT_U ||
          a == ISO8859_OUMLAUT_U);
}

/*! Match Letter Swedish */
static inline bool char_is_LETTER_ISO8859_SV(char a)
{
  return (char_is_LETTER_EN(a) ||
          char_is_LETTER_ISO8859_SVonly(a));
}

/* ---------------------------- Group Separator ---------------------------- */

static inline bool char_is_LETTER_a_to_f(char a)
{
  return (a >= ASCII_a && a <= ASCII_f);
}

static inline bool char_is_LETTER_A_to_F(char a)
{
  return (a >= ASCII_A && a <= ASCII_F);
}

/*! Match Letter English Lowercase */
static inline bool char_is_LETTER_HEX(char a)
{
  return (((a >= ASCII_a) && (a <= ASCII_f)) ||
          ((a >= ASCII_A) && (a <= ASCII_F)));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Match English Vowel - aeiouy */
static inline bool char_is_VOWEL_EN(char a)
{
  return (a == ASCII_a ||
          a == ASCII_e ||
          a == ASCII_i ||
          a == ASCII_o ||
          a == ASCII_u ||
          a == ASCII_y);
}

/*! Match Vowel Swedish-Only - ĺäö */
static inline bool char_is_VOWEL_SVonly(char a)
{
  return char_is_LETTER_ISO8859_SVonly(a);
}

/*! Match Vowel Swedish - aeiouy, ĺäö */
static inline bool char_is_VOWEL_SV(char a)
{
  return (char_is_VOWEL_EN(a) ||
          !char_is_VOWEL_SVonly(a));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Match Consonant English - bcdfghjklmnpqrstvwxz */
static inline bool char_is_CONSONANT_EN(char a)
{
  return (char_is_LETTER_EN(a) &&
          !char_is_VOWEL_EN(a));
}

/*! Match Consonant Swedish - bcdfghjklmnpqrstvwxz */
static inline bool char_is_CONSONANT_SV(char a)
{
  return char_is_CONSONANT_EN(a);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Match Digit Binary. */
static inline bool char_is_DIGIT_BIN(char a)
{
  return (a >= '0' && a <= '1');
}

/*! Match Digit Octal */
static inline bool char_is_DIGIT_OCT(char a)
{
  return (a >= '0' && a <= '7');
}

/*! Match Digit Decimal */
static inline bool char_is_DIGIT_DEC(char a)
{
  return (a >= '0' && a <= '9');
}

/*! Match Digit HexaDecimal */
static inline bool char_is_DIGIT_HEX(char a)
{
  return ((a >= '0' && a <= '9') ||
          (a >= ASCII_a && a <= ASCII_z) ||
          (a >= ASCII_A && a <= ASCII_Z));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Match Letter C Identifier Begin Letter */
static inline bool char_is_LETTER_CIDENT_BEGIN(char a)
{
  return (char_is_LETTER_EN_LOWER(a) ||
          char_is_LETTER_EN_UPPER(a) ||
          a == ASCII_UNDERSCORE);
}

/*! Match Letter C Identifier Rest Letter */
static inline bool char_is_LETTER_CIDENT_REST(char a)
{
  return (char_is_LETTER_EN_LOWER(a) ||
          char_is_LETTER_EN_UPPER(a) ||
          char_is_DIGIT_DEC(a) ||
          a == ASCII_UNDERSCORE);
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
