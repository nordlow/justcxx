/*! \file numeric_x.hpp
 * \brief Extensions to STL <numeric>.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <algorithm>
#include <type_traits>
#include <numeric>
#include <utility>
#include "enforce.hpp"

namespace std
{

/// Accumulate \c IC.
template<class C, class T> inline pure T& accumulate(const C& input, T value) {
    return std::accumulate(begin(input), end(input), value);
}
/// Accumulate \c IC.
template<class C, class T, class BinaryOperation> inline pure T& accumulate(const C& input, T value, BinaryOperation binary_op) {
    return std::accumulate(begin(input), end(input), value, binary_op);
}

/// Check if any element of \p a is not \em Not-A-Number (NaN).
template<class C> inline pure typename std::enable_if<is_container<C>::value, bool>::type isnan(const typename C::iterator& a) {
    return std::any_of(begin(a), end(a), std::isnan<typename C::value_type>); }
/// Check if either element of \p a is not \em Not-A-Number (NaN).
template<class T, class U> inline pure bool isnan(const std::pair<T,U>& a) { return std::isnan(a.first) or std::isnan(a.second); }

/// Check if all elements of \p a are \em Finite.
template<class C> inline pure typename std::enable_if<is_container<C>::value, bool>::type isfinite(const typename C::iterator& a) {
    return std::all_of(begin(a), end(a), std::isfinite<typename C::value_type>); }
/// Check if both elements of \p a are \em Finite.
template<class T, class U> inline pure bool isfinite(const std::pair<T,U>& a) { return std::isfinite(a.first) and std::isfinite(a.second); }

/// \em Standard Dot Product of \p a and \p b. For std::array<T,N> and boost::array<T,N>.
template<template<class, std::size_t> class C,
         class T,
         std::size_t N> inline pure T dot(const C<T,N>& a,
                                          const C<T,N>& b) {
    return std::inner_product(begin(a), end(a), begin(b), 0);
}
/// Dot Product of \p a and \p b. For std::vector.
template<class C> inline pure typename C::value_type dot(const C& a,
                                                         const C& b) {
    enforce_eq(a.size(), b.size());
    return std::inner_product(begin(a), end(a), begin(b), static_cast<typename C::value_type>(0));
}
/// \em Normalized Dot Product of \p a and \p b. For std::array<T,N> and boost::array<T,N>.
template<class C> inline pure typename C::value_type norm_dot(const C& a,
                                                              const C& b) {
    return std::inner_product(begin(a), end(a), begin(b), static_cast<typename C::value_type>(0)) / (norm(a) * norm(b));
}
/*! Angle \theta between \p a and \p b.
 * \see https://en.wikipedia.org/wiki/Dot_product#Geometric_interpretation */
template<class C> inline pure typename C::value_type angle(const C& a,
                                                           const C& b) {
    return std::acos(norm_dot(a, b));
}

}
