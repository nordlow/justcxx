/*! \file rot2tab.hpp
 * \brief Lookup table of Two-dimensional rotation matrices.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <cmath>
#include <array>
#include "algorithm_x.hpp"
#include "geometry/box.hpp"

/*!
 * An STL container (as wrapper) for a set of rotation matrices.
 */
template<class T, std::size_t N>
class rot2tab : public std::array<mat2<T>, N>
{
public:
    rot2tab() { for (size_t i = 0; i < N; i++) { (*this)[i].rotation(M_2PI * i / N); } }

    /***
     * Lookup the angle i.
     *
     * \param[in] i can take negative or positive angles which are wrapped
     * to [0..N] before lookup is performed.
     */
    const mat2<T>& circular_lookup(int i) { return (*this)[pnw::modulo(i, static_cast<int>(N))]; }
};
