/*! \file mean.hpp
 * \brief Means (Averages).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <algorithm>
#include "cc_features.h"
#include "type_traits_x.hpp"

namespace pnw
{

#if HAVE_CXX11_VARIADIC_TEMPLATES

/// Sum of \p a.
template<Addable T> T sum(const T& a) { return a; } // template termination
/// Sum of \p a, \p and \p args.
template<class T, class ... R > //requires SameType <T , Args >...
T sum(const T& a, const T& b, const R &... args) { return sum(a + b, args...); }

/// Sum of \p a.
template<Addable T> T common_type_sum(const T& a) { return a; } // template termination
/// Sum of \p a, \p and \p args.
template<class T, class U, class ... R, class C = typename std::common_type<T, U, R...>::type >
C common_type_sum(const T& a, const U& b, const R &... c )
{
    return common_type_sum(static_cast<C>(a + b), static_cast<C>(c)...);
}

/*! Arithmetic Mean (Average) of \p a \p b and \p args. */
template<class T, class ... R > T mean(const T& a, const T& b, const R &... args) { return sum(a, b, args...) / (2+sizeof...(args)); }

/*! Arithmetic Mean (Average) of \p a \p b and \p args. */
template<class T, class U, class ... R, class C = typename std::common_type<T, U, R...>::type >
C common_type_mean (const T& a, const T& b, const R &... args) { return common_type_sum(a, b, args...) / (2+sizeof...(args)); }

#else

/*! Arithmetic Mean (Average) of \p a and \p b. */
template<Addable T> inline pure T mean(const T& a, const T& b) { return (a + b) / 2; }
/*! Arithmetic Mean (average) of \p a, \p b and \p c. */
template<Addable T> inline pure T mean(const T& a, const T& b, const T& c) { return (a + b + c) / 3; }
/*! Arithmetic Mean (Average) of \p a, \p b, \p c and \p d. */
template<Addable T> inline pure T mean(const T& a, const T& b, const T& c, const T& d) { return (a + b + c + d) / 4; }

#endif

// ============================================================================

template<class T> T mean_element(const std::pair<T,T>& a) { return pnw::mean<T>(a.first, a.second); }

/*! \em Arithmetic Mean (Average) of the elements between \p first and \p last. */
template<class InputIterator, class T>
T sum(InputIterator first, InputIterator last, T init = 0) { return std::accumulate(first, last, init); }
/*! \em Arithmetic Mean (Average) of the elements in \p a. */
template<class InputIterator> typename InputIterator::value_type mean_element(InputIterator first, InputIterator last) { return sum(first, last, 0) / (last-first); }
/*! \em Arithmetic Mean (Average) of the elements in \p a. */
//template<template<typename> class S, typename T> T mean_element(const S<T>& a) { return mean_element(begin(a), end(a)); }
template<class C> typename C::value_type mean_element(const C& a) { return mean_element(begin(a), end(a)); }

/// Geometric Mean of \p a and \p b.
template<class T> T g_mean(const T& a, const T& b) { return sqrt(dot(a, b)); }
/// Harmonic Mean of \p a and \p b.
template<class T> T h_mean(const T& a, const T& b) { return 1 / (1/a + 1/b); }

// ============================================================================

}
