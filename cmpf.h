/*!
 * \file cmpf.h
 * \brief Complex Multi-Precision Floating Point Number.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#ifdef HAVE_GMP_H
#include <gmp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Complex Multi-Precision Floating Point Number.
 */
typedef struct
{
  mpf_t r;			/**< Real Part. */
  mpf_t i;			/**< Imaginary Part. */
} __cmpf_struct;
typedef __cmpf_struct cmpf_t[1];

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Init and Assign.
 */

/* @{ */

void cmpf_init(cmpf_t rop);
void cmpf_init_set(cmpf_t rop, const cmpf_t op);
void cmpf_init_set_ui(cmpf_t rop, ulong r, ulong i);
void cmpf_init_set_si(cmpf_t rop, signed long r, signed long i);
void cmpf_init_set_d(cmpf_t rop, double r, double i);
void cmpf_clear(cmpf_t rop);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Arithmetic.
 */

/* @{ */

void cmpf_add(cmpf_t rop, const cmpf_t op1, const cmpf_t op2);
void cmpf_sub(cmpf_t rop, const cmpf_t op1, const cmpf_t op2);
void cmpf_mul(cmpf_t rop, const cmpf_t op1, const cmpf_t op2);
void cmpf_div(cmpf_t rop, const cmpf_t op1, const cmpf_t op2);
void cmpf_neg(cmpf_t rop, const cmpf_t op);
void cmpf_conj(cmpf_t rop, const cmpf_t op);
void cmpf_norm(mpf_t rop, const cmpf_t op);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Input and Output.
 */

/*!
 * Output op to \p stream in specific base.
 * \return number of bytes written to \p stream.
 */
size_t cmpf_out_str(FILE * stream, int base, size_t ndigits, const cmpf_t op);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
