#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Bresenham line generator.
 */
void bresenham(int x1, int y1, int x2, int y2, int (*plot) (int, int));

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
