/*!
 * \file pngio.h
 * \brief PNG-file input and output functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

int freadPNG_RGB(uint *w, uint *h, uchar **rgb,
		 FILE * stream);

#ifdef __cplusplus
}
#endif
