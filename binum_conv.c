#include <stdlib.h>
#include "binum_conv.h"
#include "stdio_x.h"

/**!
 * Array of Pointers to Functions that do \em Clipped \em Conversion.
 */
size_t (*g_binumarray_clipconv_funs[BINUM_undefined_][BINUM_undefined_])
(void *datD, size_t strideD,
 void *datS, size_t strideS,
 size_t n);

#ifdef HAVE_LIBOIL
#  include <liboil/liboil.h>
#endif

void binumarray_conv_init(void)
{
  /* g_binumarray_clipconv_funs = calloc(BINUM_undefined_ * BINUM_undefined_, sizeof(void*)); */

  /* default pointers to NULL */
  for (size_t typD = 0; typD < BINUM_undefined_; typD++)
  {
    for (size_t typS = 0; typS < BINUM_undefined_; typS++)
    {
      g_binumarray_clipconv_funs[typD][typS] = NULL; /* default to empty */
    }
  }

  // g_binumarray_clipconv_funs[BINUM_MPZ][BINUM_UINT32] = mpz_set_ui;
  // g_binumarray_clipconv_funs[BINUM_MPZ][BINUM_INT32] = mpz_set_si;

  /* if have liboil use it its conversion functions */
#ifdef HAVE_LIBOIL
  g_binumarray_clipconv_funs[BINUM_INT16][BINUM_F64] = (void*)oil_clipconv_s16_f64;
  g_binumarray_clipconv_funs[BINUM_INT16][BINUM_INT32] = (void*)oil_clipconv_s16_s32;
  g_binumarray_clipconv_funs[BINUM_INT16][BINUM_UINT16] = (void*)oil_clipconv_s16_u16;
  g_binumarray_clipconv_funs[BINUM_INT16][BINUM_UINT32] = (void*)oil_clipconv_s16_u32;
  g_binumarray_clipconv_funs[BINUM_INT32][BINUM_F32] = (void*)oil_clipconv_s32_f32;
  g_binumarray_clipconv_funs[BINUM_INT32][BINUM_F64] = (void*)oil_clipconv_s32_f64;
  g_binumarray_clipconv_funs[BINUM_INT32][BINUM_UINT32] = (void*)oil_clipconv_s32_u32;
  g_binumarray_clipconv_funs[BINUM_INT8][BINUM_F32]  = (void*)oil_clipconv_s8_f32;
  g_binumarray_clipconv_funs[BINUM_INT8][BINUM_F64]  = (void*)oil_clipconv_s8_f64;
  g_binumarray_clipconv_funs[BINUM_INT8][BINUM_INT16]  = (void*)oil_clipconv_s8_s16;
  g_binumarray_clipconv_funs[BINUM_INT8][BINUM_INT32]  = (void*)oil_clipconv_s8_s32;
  g_binumarray_clipconv_funs[BINUM_INT8][BINUM_UINT16]  = (void*)oil_clipconv_s8_u16;
  g_binumarray_clipconv_funs[BINUM_INT8][BINUM_UINT32]  = (void*)oil_clipconv_s8_u32;
  g_binumarray_clipconv_funs[BINUM_INT8][BINUM_UINT8]   = (void*)oil_clipconv_s8_u8;
  g_binumarray_clipconv_funs[BINUM_UINT16][BINUM_F32] = (void*)oil_clipconv_u16_f32;
  g_binumarray_clipconv_funs[BINUM_UINT16][BINUM_F64] = (void*)oil_clipconv_u16_f64;
  g_binumarray_clipconv_funs[BINUM_UINT16][BINUM_INT16] = (void*)oil_clipconv_u16_s16;
  g_binumarray_clipconv_funs[BINUM_UINT16][BINUM_INT32] = (void*)oil_clipconv_u16_s32;
  g_binumarray_clipconv_funs[BINUM_UINT16][BINUM_UINT32] = (void*)oil_clipconv_u16_u32;
  g_binumarray_clipconv_funs[BINUM_UINT32][BINUM_F32] = (void*)oil_clipconv_u32_f32;
  g_binumarray_clipconv_funs[BINUM_UINT32][BINUM_F64] = (void*)oil_clipconv_u32_f64;
  g_binumarray_clipconv_funs[BINUM_UINT32][BINUM_INT32] = (void*)oil_clipconv_u32_s32;
  g_binumarray_clipconv_funs[BINUM_UINT8][BINUM_F32]  = (void*)oil_clipconv_u8_f32;
  g_binumarray_clipconv_funs[BINUM_UINT8][BINUM_F64]  = (void*)oil_clipconv_u8_f64;
  g_binumarray_clipconv_funs[BINUM_UINT8][BINUM_INT16]  = (void*)oil_clipconv_u8_s16;
  g_binumarray_clipconv_funs[BINUM_UINT8][BINUM_INT32]  = (void*)oil_clipconv_u8_s32;
  g_binumarray_clipconv_funs[BINUM_UINT8][BINUM_INT8]   = (void*)oil_clipconv_u8_s8;
  g_binumarray_clipconv_funs[BINUM_UINT8][BINUM_UINT16]  = (void*)oil_clipconv_u8_u16;
  g_binumarray_clipconv_funs[BINUM_UINT8][BINUM_UINT32]  = (void*)oil_clipconv_u8_u32;
#endif
}

size_t binumarray_conv_perform(void *datD, BINUM_t typD, size_t strideD,
                               void *datS, BINUM_t typS, size_t strideS,
                               size_t n, size_t clip_flag)
{
  size_t ret = 0;
  if (clip_flag) {
    if (g_binumarray_clipconv_funs[typD][typD]) {
      (g_binumarray_clipconv_funs[typD][typS])(datD, strideD, datS, strideS, n);
    } else {
      PTODO("No support for conversion typD=%d typS=%d .\n", typD, typS);
    }
  } else {
    PTODO("Support unclipped conversion.\n");
  }
  return ret;
}
