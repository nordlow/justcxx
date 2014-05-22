/*!
 * \file bz2_utils.h
 * \brief Compression and Decompress of C-style char-arrays using \c bzlib.h and \c libbzip2.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \note These features are already included in \c libbz2.
 */

#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Encode (compress) an array in \p bufI containing \p lenI bytes using
 * bzip2 compression.
 *
 * \param[in] bufI Input data Buffer
 * \param[in] lenI Length of Input data Buffer
 *
 * \param[out] bufC Buffer where Compressed data is to placed, predefined
 *         (to zero if first time)
 * \param[out] lenC Length of Compressed Buffer, predefined (to zero if first time)
 *
 * \param[in] blockSize100k BlockSize in 100 kiloBytes.  It should be a
 * value between 1 and 9 inclusive (otherwise truncated to this
 * range), and the actual block size used is 100000 x this figure. 9
 * gives the best compression but takes most memory.
 */
int bzip2_encode(const char *bufI, size_t lenI, char **bufC, size_t *lenC,
		 int blockSize100k);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Decode (decompress) an array in \p bufI containing \p lenI bytes using
 * bzip2 compression.
 *
 * \param[in] bufC Compressed Data Buffer
 * \param[in] lenC Length of Compressed Data Buffer
 *
 * \param[out] bufD Decompressed Data Buffer
 * \param[out] lenD Length of Decompressed Data Buffer
 */
int bzip2_decode(const char *bufC, size_t lenC, char **bufD, size_t *lenD);

/* ---------------------------- Group Separator ---------------------------- */

int test_bzip2(void);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
