/*!
 * \file cbufn.h
 * \brief Strings (Char Buffers) of Fixed and Limited Lengths.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "pnw_types.h"
#include "chv.h"
#include <stdlib.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * String with a length of 1 to 2.
 */
typedef struct {
  uint8_t l;			/**< Length of string + 1. */
  uint16_t s;			/**< String of length given by l. */
} cbuf2_t;

/*!
 * String with a maximum length of 1 to 4.
 */
typedef struct {
  uint8_t l;			/**< Length of string + 1. */
  uint32_t s;			/**< String of length given by l. */
} cbuf4_t;

/*!
 * String with a length of 1 to 8.
 */
typedef struct {
  uint8_t l;			/**< Length of string + 1. */
  uint64_t s;			/**< String of length given by l. */
} cbuf8_t;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * String with a length of 1 to 256.
 */
typedef struct {
  uint8_t l;			/**< Length of string + 1 . */
  char * s;			/**< Null-terminated string. */
} cbuf256_t;

/*!
 * String with a length of 1 to 65536.
 */
typedef struct {
  uint16_t l;			/**< Length of string + 1. */
  char * s;			/**< Null-terminated string. */
} cbuf65536_t;

/*!
 * String with a length of 1 to 4294967296 (2^32).
 */
typedef struct {
  uint32_t l;			/**< Length of string + 1. */
  char * s;			/**< Null-terminated string. */
} cbuf4294967296_t;

/* ---------------------------- Group Separator ---------------------------- */

/*! Encode an cbuf2 \p x into \p chv. */
static inline int chv_enc_cbuf2_be(chv_t * chv, const cbuf2_t * x)
{
  return (chv_enc_u8(chv, &x->l) +
	  chv_enc_u16be(chv, &x->s));
}

/*! Encode an cbuf4 \p x into \p chv. */
static inline int chv_enc_cbuf4_be(chv_t * chv, const cbuf4_t * x)
{
  return (chv_enc_u8(chv, &x->l) +
	  chv_enc_u32be(chv, &x->s));
}

/*! Encode an cbuf8 \p x into \p chv. */
static inline int chv_enc_cbuf8_be(chv_t * chv, const cbuf8_t * x)
{
  return (chv_enc_u8(chv, &x->l) +
	  chv_enc_u64be(chv, &x->s));
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
