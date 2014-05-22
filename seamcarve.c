#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "utils.h"
#include "seamcarve.h"
#include "meman.h"
#include "extremes.h"

void
uint8_seamcarve(uint8_t ** inAA, size_t wI, size_t hI,
                uint8_t ** outAA, size_t wO, size_t hO)
{
#ifndef NDEBUG
  if (wI == 0 || hI == 0) { PWARN("No input.\n"); }
  if (wO == 0 || hO == 0) { PWARN("No output.\n"); }
#endif

  /* energy field matrix */
  int16_t ** enAA = int16AA_calloc(hI, wI);

  /* calculate energy field of \p wI x \p hI image */
  for (size_t y = 1; y < hI-2; y++) {
    for (size_t x = 1; x < wI-2; x++) {
      /* 1-norm measure of energy using 8-nearest pixels in pixel grid */
      enAA[y][x] =
	ABS(inAA[y-1][x] - inAA[y][x-1]) + /* upper left */
	ABS(inAA[y-1][x] - inAA[y][x  ]) + /* upper center */
	ABS(inAA[y-1][x] - inAA[y][x+1]) + /* upper right */

	ABS(inAA[y  ][x] - inAA[y][x-1]) + /* middle left */
	ABS(inAA[y  ][x] - inAA[y][x  ]) + /* middle center */
	ABS(inAA[y  ][x] - inAA[y][x+1]) + /* middle right */

	ABS(inAA[y+1][x] - inAA[y][x-1]) + /* lower left */
	ABS(inAA[y+1][x] - inAA[y][x  ]) + /* lower center */
	ABS(inAA[y+1][x] - inAA[y][x+1]);  /* lower right */
    }
  }

  if (wO < wI) {		/* if shrink along X */
    /* size_t idxP[hI]; */		/* indexes for path  */
  }

  if (hO < hI) {		/* if shrink along Y */
    /* size_t idxP[wI]; */
  }

  /* find minimal-energy-path along the dimension to minimize */

  /* reduce */

  int16matrix_free(enAA);

}
