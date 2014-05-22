#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const uchar ch_fliptab[256];

void uchararray_lookup(uchar *a, const uchar *b, int n,
		 const uchar tab[256]);

#ifdef __cplusplus
}
#endif
