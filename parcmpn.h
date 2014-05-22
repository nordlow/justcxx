/*!
 * \file parcmpn.h
 * \brief N-way Parallel Compare of pairs implemented in x86 MMX-instructions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Equal to doing N-way Parallel SORT(x[i], y[i]) when \c i runs from
 * 0 to \c n.
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void
int8_parcmp8(int8_t * x, int8_t * y)
{
#ifdef HAVE_CPU_MMX
  __asm__("movq     (%2), %%mm7 \n\t" /* Load inverter bits. */
          "movq     (%0), %%mm0 \n\t" /* Load x, */
          "movq     (%1), %%mm1 \n\t" /* Load y */
          "movq    %%mm1, %%mm2 \n\t" /* y again for mask bits */
          "pcmpgtb %%mm0, %%mm2 \n\t" /* Mask bits */
          "movq    %%mm0, %%mm3 \n\t"
          "pand    %%mm2, %%mm3 \n\t" /* All x_i where x_i < y_i */
          "movq    %%mm1, %%mm4 \n\t"
          "pxor    %%mm7, %%mm2 \n\t" /* Toggle mask bits */
          "pand    %%mm2, %%mm4 \n\t" /* All y_i where y_i <= x_i */
          "por     %%mm3, %%mm4 \n\t" /* min_i */
          "movq    %%mm4,  (%0) \n\t" /* is written */
          "movq    %%mm0, %%mm3 \n\t"
          "pand    %%mm2, %%mm3 \n\t" /* All x_i where x_i >= y_i */
          "movq    %%mm1, %%mm4 \n\t"
          "pxor    %%mm7, %%mm2 \n\t" /* Toggle mask bits */
          "pand    %%mm2, %%mm4 \n\t" /* All y_i where y_i > x_i */
          "por     %%mm3, %%mm4 \n\t" /* max_i */
          "movq    %%mm4,  (%1) \n\t" /* is written */
          ::"r"(x), "r"(y), "r"(&g_uint64_ones));
#endif
}

static inline void
int16_parcmp4(int16_t * x, int16_t * y)
{
#ifdef HAVE_CPU_MMX
  __asm__("movq     (%2), %%mm7 \n\t" /* Load inverter bits. */
          "movq     (%0), %%mm0 \n\t" /* Load x, */
          "movq     (%1), %%mm1 \n\t" /* Load y */
          "movq    %%mm1, %%mm2 \n\t" /* y again for mask bits */
          "pcmpgtw %%mm0, %%mm2 \n\t" /* Mask bits */
          "movq    %%mm0, %%mm3 \n\t"
          "pand    %%mm2, %%mm3 \n\t" /* All x_i where x_i < y_i */
          "movq    %%mm1, %%mm4 \n\t"
          "pxor    %%mm7, %%mm2 \n\t" /* Toggle mask bits */
          "pand    %%mm2, %%mm4 \n\t" /* All y_i where y_i <= x_i */
          "por     %%mm3, %%mm4 \n\t" /* min_i */
          "movq    %%mm4,  (%0) \n\t" /* is written */
          "movq    %%mm0, %%mm3 \n\t"
          "pand    %%mm2, %%mm3 \n\t" /* All x_i where x_i >= y_i */
          "movq    %%mm1, %%mm4 \n\t"
          "pxor    %%mm7, %%mm2 \n\t" /* Toggle mask bits */
          "pand    %%mm2, %%mm4 \n\t" /* All y_i where y_i > x_i */
          "por     %%mm3, %%mm4 \n\t" /* max_i */
          "movq    %%mm4,  (%1) \n\t" /* is written */
          ::"r"(x), "r"(y), "r"(&g_uint64_ones));
#endif
}

static inline void
int32_parcmp2(int32_t * x, int32_t * y)
{
#ifdef HAVE_CPU_MMX
  __asm__("movq     (%2), %%mm7 \n\t" /* Load inverter bits. */
          "movq     (%0), %%mm0 \n\t" /* Load x, */
          "movq     (%1), %%mm1 \n\t" /* Load y */
          "movq    %%mm1, %%mm2 \n\t" /* y again for mask bits */
          "pcmpgtd %%mm0, %%mm2 \n\t" /* Mask bits */
          "movq    %%mm0, %%mm3 \n\t"
          "pand    %%mm2, %%mm3 \n\t" /* All x_i where x_i < y_i */
          "movq    %%mm1, %%mm4 \n\t"
          "pxor    %%mm7, %%mm2 \n\t" /* Toggle mask bits */
          "pand    %%mm2, %%mm4 \n\t" /* All y_i where y_i <= x_i */
          "por     %%mm3, %%mm4 \n\t" /* min_i */
          "movq    %%mm4,  (%0) \n\t" /* is written */
          "movq    %%mm0, %%mm3 \n\t"
          "pand    %%mm2, %%mm3 \n\t" /* All x_i where x_i >= y_i */
          "movq    %%mm1, %%mm4 \n\t"
          "pxor    %%mm7, %%mm2 \n\t" /* Toggle mask bits */
          "pand    %%mm2, %%mm4 \n\t" /* All y_i where y_i > x_i */
          "por     %%mm3, %%mm4 \n\t" /* max_i */
          "movq    %%mm4,  (%1) \n\t" /* is written */
          ::"r"(x), "r"(y), "r"(&g_uint64_ones));
#endif
}

#ifdef __cplusplus
}
#endif
