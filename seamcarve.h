/*!
 * \file seamcarve.h
 * \brief Seam Carving - Content Aware Image Resize
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-08-23 18:06
 *
 * \url: http://www.faculty.idc.ac.il/arik/
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Seam Carve Resize the C array of row-pointers \p dat of dimensions
 * \p wI x \p hI into the new dimension \p wO x \p hO.
 */
void
uint8_seamcarve(uint8_t ** inAA, size_t wI, size_t hI,
                uint8_t ** outAA, size_t wO, size_t hO);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
