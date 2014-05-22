/*!
 * \file jpgio.h
 * \brief JPEG-file Input and Output.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Open the file named filename and try reading a JPEG-image from it.
 *
 * \param[in] stream JPEG-image file stream
 *
 * \param[out] width Width of the loaded image
 * \param[out] height Height of the loaded image
 *
 * \param[out] rgb *rgb is set to the allocated and written image-data
 *
 * \return
 * - 0 if operation succeeded.
 * - -1 otherwise.
 */
int freadJPG_RGB(uint *width, uint *height, uchar **rgb,
		 FILE * stream);

/*!
 * Write a JPEG image width the dimensions width x height and with the image
 * data inside rgb to the file outfile.
 *
 * \return
 * - 0 if operation succeeded.
 * - -1 otherwise.
 */
int fwriteJPG_RGB(uint width, uint height,
		  uchar *rgb, int quality,
		  FILE * stream);

#ifdef __cplusplus
}
#endif
