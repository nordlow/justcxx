/*!
 * \file binum_conv.h
 * \brief Conversion of arrays of \c BINUM_t.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-12-17 09:03
 *
 * Uses a lookup-table of size \c BINUM_undefined_ ^ 2;
 */

#pragma once

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "BINUM_enum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**!
 * Initialize Static Array of Conversion Functions.
 */
void binumarray_conv_init(void);

/**!
 * Perform Conversion.
 * \return 1 upon success, 0 if no conversion function could be found.
 */
size_t binumarray_conv_perform(void *datD, BINUM_t typD, size_t strideD,
                               void *dstS, BINUM_t typS, size_t strideS,
                               size_t n, size_t clip_flag);

#ifdef __cplusplus
}
#endif
