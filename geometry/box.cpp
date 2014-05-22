#include "box.hpp"

#if HAVE_CXX11_EXTERN_TEMPLATES
template box<float,2> shiftCtrlP(box<float,2>& a,
                                 const vec<float,2>& mpos,
                                 const vec<float,2>& mshift,
                                 const vec<float,2>& min_dim,
                                 const uint8_t * mask,
                                 const vec<float,2>& PAR);
#endif
