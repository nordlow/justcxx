#include "rottab.hpp"
#include "geometry/mat.hpp"

mat2f g_rottab[ROTTAB_SZ];

__attribute__ ((constructor)) void rottab_init(void)
{
    for (uint i = 0; i < ROTTAB_SZ; i++) {
        g_rottab[i] = rotation(static_cast<float>(M_2PI) * i / ROTTAB_SZ);
    }
}
