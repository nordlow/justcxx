/*! \file waves.hpp
 * \brief Generate harmonic waves.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <cmath>
#include <vector>

/*! Generate a sinus wave. */
template<class T>
inline spure std::vector<T> sinwave(T phase, T freq, T amp, size_t n)
{
    std::vector<T> a(n);
    for (size_t i = 0; i < n; i++) {
        a[i] = amp * std::sin(phase + freq * i * M_PI * 2 / n);
    }
    return a;
}

/*! Generate a cosinus wave. */
template<class T>
inline spure std::vector<T> coswave(T phase, T freq, T amp, size_t n)
{
    std::vector<T> a(n);
    for (size_t i = 0; i < n; i++) {
        a[i] = amp * std::cos(phase + freq * i * M_PI * 2 / n);
    }
    return a;
}
