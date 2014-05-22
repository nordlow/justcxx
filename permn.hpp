/*! \file permn.hpp
 * \brief Permutate \em fixed number of explicit arguments.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * C++ unfortunately has no \em Parallel Assignment so we need to code
 * this manually using a \em temporary variable \c _t.
 * \see http://en.wikipedia.org/wiki/Assignment_(computer_science)#Parallel_assignment
 */

#pragma once
#include <utility>

/*! \em Full 3-Permutation/Shift/Rotate to [\p b \p c \p a]. */
template<class T> inline void perm3_231(T& a, T& b, T& c) { T _t=a; a=b; b=c; c=_t; }
/*! \em Full 3-Permutation/Shift/Rotate to [\p c \p a \p b]. */
template<class T> inline void perm3_312(T& a, T& b, T& c) { T _t=a; a=c; c=b; b=_t; }

/*! \em 2-Cycle 4-Permutation/Shift/Rotate to [\p d \p c \p b \p a]. */
template<class T> inline void perm4_4321(T& a, T& b, T& c, T& d) { std::swap(a,d); std::swap(b,c); }
/*! \em 2-Cycle 4-Permutation/Shift/Rotate to [\p b \p a \p d \p c]. */
template<class T> inline void perm4_2143(T& a, T& b, T& c, T& d) { std::swap(a,b); std::swap(c,d); }
/*! \em Full 4-Permutation/Shift/Rotate to [\p c \p d \p a \p b]. */
template<class T> inline void perm4_3412(T& a, T& b, T& c, T& d) { std::swap(a,c); std::swap(b,d); }

/*! \em Full 4-Permutation/Shift/Rotate to [\p b \p c \p d \p a]. */
template<class T> inline void perm4_2341(T& a, T& b, T& c, T& d) { T _t=a; a=b; b=c; c=d; d=_t; }
/*! \em Full 4-Permutation/Shift/Rotate to [\p b \p d \p a \p c]. */
template<class T> inline void perm4_2413(T& a, T& b, T& c, T& d) { T _t=a; a=b; b=d; d=c; c=_t;  }
/*! \em Full 4-Permutation/Shift/Rotate to [\p c \p a \p d \p b]. */
template<class T> inline void perm4_3142(T& a, T& b, T& c, T& d) { T _t=a; a=c; c=d; d=b; b=_t;  }
/*! \em Full 4-Permutation/Shift/Rotate to [\p c \p d \p b \p a]. */
template<class T> inline void perm4_3421(T& a, T& b, T& c, T& d) { T _t=a; a=c; c=b; b=d; d=_t; }
/*! \em Full 4-Permutation/Shift/Rotate to [\p d \p a \p b \p c]. */
template<class T> inline void perm4_4123(T& a, T& b, T& c, T& d) { T _t=a; a=d; d=c; c=b; b=_t;  }
/*! \em Full 4-Permutation/Shift/Rotate to [\p d \p c \p a \p b]. */
template<class T> inline void perm4_4312(T& a, T& b, T& c, T& d) { T _t=a; a=d; d=b; b=c; c=_t;  }
