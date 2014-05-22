#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Pointer arithmetic together with a "char *oend, *iend" should make it
 * more readable.
 */
int rl_encode(uchar *out, uchar *in, int nin);

int rl_decode(uchar *out, int nout, uchar *in, int nin);

/*!
 * Pointer arithmetic together with a "char *oend, *iend" should make it
 * more readable.
 */
int rl_encode1(uchar *out, uchar *in, int nin);

int rl_decode1(uchar *out, int nout, uchar *in, int nin);

void rl_test(void);

#ifdef __cplusplus
}
#endif
