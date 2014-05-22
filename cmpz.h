/*!
 * \file cmpz.h
 * \brief Complex Multi-Precision Integer.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#ifdef HAVE_CONFIG_H
#  include "config.h"
#  endif
#include "utils.h"
#ifdef HAVE_GMP_H
#include <gmp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Complex Multi-Precision Integer.
 */
typedef struct
{
  mpz_t r;			/**< Real Part. */
  mpz_t i;			/**< Imaginary Part. */
} __cmpz_struct;
typedef __cmpz_struct cmpz_t[1];

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Init & Clear.
 */

/* @{ */

void cmpz_init(cmpz_t rop);
void cmpz_clear(cmpz_t rop);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Init & Assign.
 */

/* @{ */

void cmpz_init_set(cmpz_t rop, const cmpz_t op);
void cmpz_init_set_ui(cmpz_t rop, ulong op_r, ulong op_i);
void cmpz_init_set_si(cmpz_t rop, signed long op_r, signed long op_i);
void cmpz_init_set_d(cmpz_t rop, double op_r, double op_i);
int cmpz_init_set_str(cmpz_t rop, char *str_r, char *str_i, int base);

/*! Returns c string in format (r,i). */
char *cmpz_get_str(const cmpz_t op, int base);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Arithmetic.
 */

/* @{ */

void cmpz_add(cmpz_t rop, const cmpz_t op1, const cmpz_t op2);
void cmpz_sub(cmpz_t rop, const cmpz_t op1, const cmpz_t op2);
void cmpz_mul(cmpz_t rop, const cmpz_t op1, const cmpz_t op2);
void cmpz_neg(cmpz_t rop, const cmpz_t op);
void cmpz_conj(cmpz_t rop, const cmpz_t op);
void cmpz_norm(mpz_t rop, const cmpz_t op);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Input and Output.
 */

/* @{ */

/*!
 * Output op to \p stream in specific base.
 * \return number of bytes written to \p stream.
 */
size_t cmpz_out_str(FILE * stream, int base, const cmpz_t op);

/* @} */

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
