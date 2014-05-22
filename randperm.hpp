/*! \file randperm.hpp
 * \brief Random Permutation.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-07-30 03:12
 */

#pragma once
#include "type_traits_x.hpp"
#include "ramp.hpp"
#include "shuffle.hpp"

/*! Generate a Random Permutation of length \p n (indexing from \p a_min to \p a_max).
 * \param a_min defaults to 0.
 * \param a_max defaults to n-1.
 */
template<class C> void randperm(C& a,
                                typename C::value_type a_min = std::numeric_limits<typename C::value_type>::min(),
                                typename C::value_type a_max = 1) {
    ramp(a, a_min, a_max);
    shuffle(a);
    enforce(pnw::is_continuous_permutation(a));
}
