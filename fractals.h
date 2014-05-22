/*!
 * \file fractals.h
 * \brief Fractal generators.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Generates fractals of various kinds such as a Mandelbrot.
 */

#pragma once

#include "utils.h"
#include "complexf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Return an intensity value at position \p z = x+y*i in the Mandelbrot fractal.
 *
 * Intensity value is truncated to the parameter cmax.
 *
 * \param x is the real part.
 * \param y is the complex part.
 * \param zmax is the complex square norm approximating "infinity".
 * \param cmax is the maximum intensity value.
 */
#ifdef HAVE_CPU_3DNOW
static inline int
complexf_mandelbrot_at(complexf z, float zmax, int cmax)
{
  int c = 0;
  complexf z0 = z;
  float zn;

  do {
    __asm__ volatile ("movq   (%1), %%mm0 \n\t"	// z => mm0
		      "pfmul %%mm0, %%mm0 \n\t"	// xs, ys => mm1
		      "punpckldq %%mm0, %%mm1 \n\t"	// xs => mm1
		      :		// out args to C
		      :"r" (&z0), "r"(&z), "r"(&zn));	// in args from C
  }
  while (c++ <= cmax && zn <= zmax);

  return c;
}
#else
#ifdef PNW_USE_GNUC_COMPLEX
static inline int
complexf_mandelbrot_at(complexf z, float zmax, int cmax)
{
  int c = 0;
  complexf z0 = z;
  float zn;			/* znorm */

  do {
    z = z * z + z0;
    zn = std::sqr(__real__ z) + std::sqr(__imag__ z);
  }
  while (c++ <= cmax && zn <= zmax);

  return c;
}
#else
static inline int
complexf_mandelbrot_at(complexf z, float zmax, int cmax)
{
  int c = 0;
  float x = z.re, y = z.im;
  float x0 = x, y0 = y;
  float xs, ys, zn;		/* x squared, y squared, znorm */

  do {
    xs = x * x;
    ys = y * y;

    y = 2 * x * y + y0;
    x = xs - ys + x0;

    zn = xs + ys;
  }
  while (c++ <= cmax && zn <= zmax);

  return c;
}
#endif
#endif

#ifdef __cplusplus
}
#endif
