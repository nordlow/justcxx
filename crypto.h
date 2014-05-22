#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

void tea_encode(ulong *v, const ulong *keys);
void tea_decode(ulong *v, const ulong *keys);

#ifdef __cplusplus
}
#endif
