/*!
 * \file unite.hpp
 * \brief Calculation Set Unions typically Box Types of arbitrary dimension.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "cc_features.h"

#if HAVE_CXX11_VARIADIC_TEMPLATES
template<LTC S, LTC T, LTC ... R > inline pure T unite(const S& a, const T& b, const R &... c) { return unite(a, unite(b, c...)); }
#else
template<class T> inline pure T unite(const T& a, const T& b, const T& c) { return unite(a, unite(b, c)); }
template<class T> inline pure T unite(const T& a, const T& b, const T& c, const T& d) { return unite(unite(a, b),
                                                                                                                       unite(c, d)) }
template<class T> inline pure T unite(const T& a, const T& b, const T& c, const T& d, const T& e) { return unite(a,
                                                                                                                                   unite(b, c, d, e)); }
#endif
