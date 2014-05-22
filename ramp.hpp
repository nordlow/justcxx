#pragma once
#include "type_traits_x.hpp"

/*! Generate a Ramp from \p start with \p step.
 */
template<class C> inline void ramp(C& a,
                                   typename C::value_type start = 0,
                                   typename C::value_type step  = 1)
{
    for (size_t i = 0; i < a.size(); i++) {
        a[i] = start + i * step;
    }
}
