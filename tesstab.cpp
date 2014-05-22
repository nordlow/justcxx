#include "tesstab.hpp"

vec2f g_tesstab[TESS_SZ];

__attribute__((constructor)) void tesstab_init(void) { cossin((vec2f*)&g_tesstab, TESS_SZ); }
