/*! \file are_ordered.hpp
 * \brief Check if all arguments are sorted.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see boost::is_sorted
 * \see std::is_sorted
 */

#pragma once
#include "cc_features.h"

namespace pnw {

/*! \name Check for Sortedness.
 * Return true if all arguments are sorted in \em ascending order, false otherwise.
 */
/// \{
#if HAVE_CXX11_VARIADIC_TEMPLATES
template<class T>               inline bool are_ordered(const T& a, const T& b)                    { return a < b; } // template termination
template<class T, class ... R > inline bool are_ordered(const T& a, const T& b, const R &... args) { return are_ordered(a, b) and are_ordered(b, args...); } // requires SameType<T, Args>...
#else
template<class T> inline pure bool are_ordered(const T & a, const T & b) { return a <= b; }
template<class T> inline pure bool are_ordered(const T & a, const T & b, const T & c) { return a <= b and b <= c; }
template<class T> inline pure bool are_ordered(const T & a, const T & b, const T & c, const T & d) { return a <= b and b <= c and c <= d; }
template<class T> inline pure bool are_ordered(const T & a, const T & b, const T & c, const T & d, const T & e) { return a <= b and b <= c and c <= d and d <= e; }
template<class T> inline pure bool are_ordered(const T & a, const T & b, const T & c, const T & d, const T & e, const T & f) { return a <= b and b <= c and c <= d and d <= e and e <= f; }
template<class T> inline pure bool are_ordered(const T & a, const T & b, const T & c, const T & d, const T & e, const T & f, const T & g) { return a <= b and b <= c and c <= d and d <= e and e <= f and f <= g; }
// \}
#endif

}
