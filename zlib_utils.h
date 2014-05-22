/*!
 * \file zlib_utils.h
 * \brief Utils for zlib compression.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-10-21 16:46
 */

#pragma once
#include <zlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \em Compress from file \p source to file \p dest until \c EOF on source.
 *
 * \return \c Z_OK on success, \c Z_MEM_ERROR if memory could not be
 * allocated for processing, \c Z_STREAM_ERROR if an invalid
 * compression level is supplied, \c Z_VERSION_ERROR if the version of
 * zlib.h and the version of the library linked do not match, or @c
 * Z_ERRNO if there is an error reading or writing the files.
 */
int zlib_compress(FILE *source, FILE *dest, int level);

/* Decompress from file \p source to file \p dest until stream ends or \c EOF.
 *
 * \return \c Z_OK on success, \c Z_MEM_ERROR if
 * memory could not be allocated for processing, \c Z_DATA_ERROR if
 * the deflate data is invalid or incomplete, \c Z_VERSION_ERROR if
 * the version of zlib.h and the version of the library linked do not
 * match, or \c Z_ERRNO if there is an error reading or writing the
 * files.
 */
int zlib_decompress(FILE *source, FILE *dest);

/*! Compress or decompress from stdin to stdout. */
int zlib_test(int argc, char **argv);

#ifdef __cplusplus
}
#endif
