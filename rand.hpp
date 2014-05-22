/*! \file rand.hpp
 * \brief Random Generators.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \note Make sure as few container elements as possible are zero-initialized before randomization.
 * \todo Use Boost.Random.
 * \todo Use block randoming using \c _Bit_type in \c rand() and \c rand_n() especially for
 * rand(std::vector<bool>& a)
 * \see http://isocpp.org/files/papers/n3551.pdf
 * \todo Support libc++ header guard macro names.
 */

#pragma once
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <utility>
#include <tuple>
#include "cc_features.h"
#include "rand.h"
#include "algorithm_x.hpp"
#include "type_traits_x.hpp"

/*! \em Seed State of \c rand() with current time. */
inline void time_srand() { srand(time(NULL)); }

/*! Forward Declarations. */
template<class C> inline wuur C rrand_n(size_t n,
                                        const typename C::value_type& l,
                                        const typename C::value_type& h);

/*! \name Return a Random Value from Inclusive Range [\p l, \p h]. */
/// \{
#ifdef _GLIBCXX_TYPE_TRAITSf
template<class T>
inline T rrand(T& x, const T& l, const T& h) {
    if (std::is_integral<T>()) {
        const auto span = h-l + 1;
        x = l + std::rand() % (span ? span : std::numeric_limits<T>::max());
    } else if (std::is_floating_point<T>()) {
        x = l + (h-l) / RAND_MAX * std::rand();
    } else {
        x = l + (h-l) / RAND_MAX * std::rand();
    }
    return x;
}
#else
template<class T> inline T rrand(T& x, const T& l, const T& h) {
    const auto span = h-l + 1;
    return x = l + std::rand() % (span ? span : std::numeric_limits<T>::max());
}
template<class T> inline T rrandf(T& x, const T& l, const T& h) { return x = l + (h-l) / RAND_MAX * std::rand(); }
inline               float rrand (float& x,  float l, float h) { return x = rrandf(x, l, h); }
inline              double rrand (double& x, double l, double h) { return x = rrandf(x, l, h); }
inline         long double rrand (long double& x, long double l, long double h) { return x = rrandf(x, l, h); }
/// \}
#endif

/*! \name Random. */
/// \{
// Signed Integers Specialization
inline char&    rand(char& a)    { return a = int8_rand(); }
inline wchar_t& rand(wchar_t& a) { return a = int32_rand(); }
inline int8_t&  rand(int8_t& a)  { return a = int8_rand(); }
inline int16_t& rand(int16_t& a) { return a = int16_rand(); }
inline int32_t& rand(int32_t& a) { return a = int32_rand(); }
inline int64_t& rand(int64_t& a) { return a = int64_rand(); }
// Unsigned Integers Specialization
inline uint8_t&  rand(uint8_t& a)  { return a = int8_rand(); }
inline uint16_t& rand(uint16_t& a) { return a = int16_rand(); }
inline uint32_t& rand(uint32_t& a) { return a = int32_rand(); }
inline uint64_t& rand(uint64_t& a) { return a = int64_rand(); }
// Misc Specialization
inline bool& rand(bool& a) { return a = bool_rand(); }
// Floating Point Specialization
#ifdef _GLIBCXX_TYPE_TRAITSf // TODO: Enable this
template<class T> T& rand<typename std::enable_if<std::is_floating_point<T>::value, T>::type>(T& a) {
    return rrand<T>(a,
                           static_cast<T>(0.0),
                           static_cast<T>(1.0));
}
#else
inline float& rand(float& a) { rrand(a, 0.0f, 1.0f); return a; }
inline double& rand(double& a) { rrand(a, 0.0, 1.0); return a; }
inline long double& rand(long double& a) { rrand(a, 0.0L, 1.0L); return a; }
#endif
/*! Show Printable ASCII Characters \em above double-quote (0x22). */
#if defined (_GLIBCXX_STRING) || defined (_STL_STRING_H) ///< If <string> was included
inline std::string& rand(std::string& a) { return a = rrand_n<std::string>(2, 0x23, 0x7e); }
#endif
/// \}

// ============================================================================

/*! Randomize Pair \p a. */
template<class T, class U> inline std::pair<T,U>& rand(std::pair<T,U>& a) {
    rand(std::get<0>(a));
    rand(std::get<1>(a));
    return a;
}

#if HAVE_CXX11_VARIADIC_TEMPLATESf

// template<class T, class U...> inline std::tuple<T, U...>& rand(std::tuple<T, U...>& a) {
// }

// TODO: Add it!
#else
/*! Randomize Tuple \p a. */
template<class T> inline std::tuple<T>& rand(std::tuple<T>& a) {
    rand(std::get<0>(a));
    return a;
}
/*! Randomize Tuple \p a. */
template<class T, class U> inline std::tuple<T,U>& rand(std::tuple<T,U>& a) {
    rand(std::get<0>(a));
    rand(std::get<1>(a));
    return a;
}
/*! Randomize Tuple \p a. */
template<class T, class U, class V> inline std::tuple<T,U,V>& rand(std::tuple<T,U,V>& a) {
    rand(std::get<0>(a));
    rand(std::get<1>(a));
    rand(std::get<2>(a));
    return a;
}
/*! Randomize Tuple \p a. */
template<class T, class U, class V, class W> inline std::tuple<T,U,V,W>& rand(std::tuple<T,U,V,W>& a) {
    rand(std::get<0>(a));
    rand(std::get<1>(a));
    rand(std::get<2>(a));
    rand(std::get<3>(a));
    return a;
}
#endif

// ============================================================================

/*! Randomized \p n elements at \p first. */
extern int32_t* rand_n(int32_t* first, size_t n);
/*! Randomized \p n elements at \p first. */
extern int64_t* rand_n(int64_t* first, size_t n);

/*! Randomized \p n elements at \p first. */
inline uint32_t* rand_n(uint32_t* first, size_t n) { return reinterpret_cast<uint32_t*>(rand_n(reinterpret_cast<int32_t*>(first), n)); }
/*! Randomized \p n elements at \p first. */
inline uint64_t* rand_n(uint64_t* first, size_t n) { return reinterpret_cast<uint64_t*>(rand_n(reinterpret_cast<int64_t*>(first), n)); }

// ============================================================================

/*! Randomize all elements in \p a and return it. */
template<class C> inline typename std::enable_if<is_container<C>::value, C>::type& rrand(C& a,
                                                                                         const typename C::value_type& l,
                                                                                         const typename C::value_type& h) {
    for (auto& e : a) { rrand(e, l, h); } return a;
}
/*! Randomize all elements in \p a and return it. */
template<class C> inline typename std::enable_if<is_container<C>::value, C>::type& rand(C& a) {
    for (auto& e : a) { rand(e); }
    return a;
}
#if defined (_GLIBCXX_VECTOR) || defined (_STL_VECTOR_H) ///< If <vector> was included
inline std::vector<bool>& rand(std::vector<bool>& a) {
    for (size_t i = 0; i < a.size(); i++) { bool b; rand(b); a[i] = b; } // TODO: Optimize
    return a;
}
#endif
#if defined (_GLIBCXX_BITSET) || defined (_STL_BITSET_H) ///< If <bitset> was included
template<size_t N> inline std::bitset<N>& rand(std::bitset<N>& a) {
    for (size_t i = 0; i < a.size(); i++) { bool b; rand(b); a[i] = b; } // TODO: Optimize
    return a;
}
#endif
#if defined (_GLIBCXX_TR2_DYNAMIC_BITSET)
template<class T> inline std::tr2::dynamic_bitset<T>& rand(std::tr2::dynamic_bitset<T>& a) {
    for (size_t i = 0; i < a.size(); i++) { bool b; rand(b); a[i] = b; }
    return a;
}

// TODO: Activate these when we know how to access the internal blocks.
// inline std::tr2::dynamic_bitset<uint32_t>& rand(std::tr2::dynamic_bitset<uint32_t>& a) { rand_n(reinterpret_cast<int32_t*>(a._M_Nb), a.num_blocks()); return a; }
// inline std::tr2::dynamic_bitset<uint64_t>& rand(std::tr2::dynamic_bitset<uint64_t>& a) { rand_n(reinterpret_cast<int64_t*>(a._M_Nb), a.num_blocks()); return a; }

#endif
#if defined (BOOST_DYNAMIC_BITSET_HPP) ///< If <boost/dynamic_bitset.hpp> was included
template<class T> inline boost::dynamic_bitset<T>& rand(boost::dynamic_bitset<T>& a) {
    for (size_t i = 0; i < a.size(); i++) { bool b; rand(b); a[i] = b; } // TODO: Optimize
    return a;
}
#endif

// ============================================================================

template<class T> inline wuur T rand() { T a; rand(a); return a; }

/*! Randomized \p n elements at \p first. */
template<class OutputIterator> inline void rand_n(OutputIterator first, size_t n) {
    std::generate_n(first, n, rand<typename OutputIterator::value_type>());
}

// ============================================================================

/*! Randomized \p n elements at \p first. */
template<class T> inline void rrand_n(T* first, size_t n, T l, T h) {
    std::generate_n(first, n, [l,h]() { T a; return rrand(a, l,h); } ); // C++11
}

/*! Randomized \p n elements at \p first in range [\p l, \p h]. */
template<class OutputIterator> inline void rrand_n(OutputIterator first, size_t n,
                                                   const typename OutputIterator::value_type& l,
                                                   const typename OutputIterator::value_type& h) {
    std::generate_n(first, n, [l,h]() { typename OutputIterator::value_type a; return rrand(a, l,h); } ); // C++11
}

/*! Return a Randomized Instance of \c C of length \p n. */
template<class C> inline wuur typename std::enable_if<is_container<C>::value, C>::type rand_n(size_t n) {
    C c; c.reserve(n);
    std::generate_n(std::back_inserter(c), n, []() { typename C::value_type a; rand(a); return a; } );
    return c;
}

/*! Return a Randomized Instance of \c C of length \p n in range [\p l, \p h]. */
template<class C> inline wuur C rrand_n(size_t n,
                                        const typename C::value_type& l,
                                        const typename C::value_type& h) {
    C c; c.reserve(n);
    std::generate_n(std::back_inserter(c), n, [l,h]() { typename C::value_type a; return rrand(a, l,h); });
    return c;
}
