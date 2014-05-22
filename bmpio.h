/*!
 * \file bmpio.h
 * \brief BMP-file input and output functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <stdio.h>

/* ========================================================================= */

#ifdef __cplusplus
extern "C" {
#endif

int freadBMP(size_t *w, size_t *h, uchar **rgb,
	     FILE * stream);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
