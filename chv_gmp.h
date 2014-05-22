/*!
 * \file chv_gmp.h
 * \brief 
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2010-03-18 15:25
 */

#pragma once

#include "chv.h"
#include <gmp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Encode an \c mpz_t from \p a into \p chv. */
int chv_enc_mpz(chv_t * chv, const mpz_t a, int little_endian_flag);
/*! Decode an \c mpz_t from \p chv into \p a. */
int chv_dec_mpz(chv_t * chv, mpz_t a, int little_endian_flag);

/*! Encode an \c mpq_t from \p a into \p chv. */
int chv_enc_mpq(chv_t * chv, const mpq_t a, int little_endian_flag);
/*! Decode an \c mpq_t from \p chv into \p a. */
int chv_dec_mpq(chv_t * chv, mpq_t a, int little_endian_flag);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif

