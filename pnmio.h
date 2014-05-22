#pragma once

#include "utils.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Write data to a PGM-file.
 */
int fwritePGM(const uchar *data,
	      uint w, uint h, FILE * stream);

/*!
 * Write data to a PPM-file.
 */
int fwritePPM(const uchar *data,
	      uint w, uint h, FILE * stream);

/*!
 * Read data from a PGM-file.
 */
int freadPGM(uint *w, uint *h, uchar **rgb,
	     FILE * stream);

/*!
 * Read data from a PPM-file.
 */
int freadPPM(uint *w, uint *h, uchar **rgb,
	     FILE * stream);

#ifdef __cplusplus
}
#endif
