/*!
 * \file fftshift.hpp
 * \brief Pre-FFT vector swap (shift).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <cmath>

template<class T> void fftshift(std::vector<T> & a, const std::vector<T> & b) {
    const size_t n = b.size();
    const size_t no2 = n/2;
    a.resize(n);
    for (size_t i = 0; i < no2; i++) {
        a[i + no2] = b[i];
        a[i] = b[i + no2];
    }
}
