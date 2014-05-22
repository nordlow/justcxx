/*
 * The TGA format was developed by Truevision as a way of storing color images,
 * specifically 24 and 32-bit color images. The format is sometimes known
 * incorrectly as "Targa" which refers to the graphics hardware for which the
 * format was originally designed.
 *
 * The TGA format is lossless (no image data is discarded during saving) and
 * supports high-color images.
 *
 * The TGA format can be RLE (Run Length Encoding) compressed which results in
 * significantly smaller file sizes, but will give slower performance when
 * handling the image.
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif


void fwriteTGA_UC_RGB24(FILE * file, int width, int height, char *img,
			int sz);

#ifdef __cplusplus
}
#endif
