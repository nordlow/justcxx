/*! \file encode.hpp
 * \brief Encoding (Serialization) of Basic C/C++ Types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Improve encoding std::tuple by reading: http://stackoverflow.com/questions/8569567/get-part-of-stdtuple
 * \todo Integrate with Boost::Serialization.
 */

#pragma once
#include <ostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <array>
#include <vector>
#include <string>
#include <cstring>
#include <sys/types.h> /* Defines in_addr_t which is needed in <netinet/in.h>  */
#include <netinet/in.h>
#include "cc_features.h"
#include "bitwise.hpp"
#include "encode.hpp"

/// Encode \p a.
template<class T, class U> inline void encode(std::ostream& os, const std::pair<T,U>& a) {
    encode(os, a.first);
    encode(os, a.second);
}

#if HAVE_CXX11_VARIADIC_TEMPLATES
/// Encode \p a, \p and \p args.
template<class T, class... R> inline void encode(std::ostream& os, const T& a, const T& b, const R&... args) {
    encode(os, a);
    encode(os, b);
    encode(os, args...);
}
template<class T, std::size_t N>
struct TupleEncoder {
    static void encode(std::ostream& os, const T& a) { TupleEncoder<T,N-1>::encode(os, a); encode(std::get<N-1>(a)); }
};
template<class T>
struct TupleEncoder<T, 1> {
    static void encode(std::ostream& os, const T& a) { encode(std::get<0>(a)); }
};
template<class... Ts> inline void encode(std::ostream& os, const std::tuple<Ts...>& a) {
    typedef const std::tuple<Ts...>& tuple_t;
    TupleEncoder<tuple_t,sizeof...(Ts)>::encode(os,a);
}
#else
/// Encode \p a.
template<class T, class U> inline void encode(std::ostream& os, const std::tuple<T,U>& a) {
    encode(os, std::get<0>(a));
    encode(os, std::get<1>(a));
}
/// Encode \p a.
template<class T, class U, class V> inline void encode(std::ostream& os, const std::tuple<T,U,V>& a) {
    encode(os, std::get<0>(a));
    encode(os, std::get<1>(a));
    encode(os, std::get<2>(a));
}
/// Encode \p a.
template<class T, class U, class V, class W> inline void encode(std::ostream& os, const std::tuple<T,U,V,W>& a) {
    encode(os, std::get<0>(a));
    encode(os, std::get<1>(a));
    encode(os, std::get<2>(a));
    encode(os, std::get<3>(a));
}
#endif

/*! Encode \p a. */
inline void encode(std::ostream& os, uint8_t a) { os.write(reinterpret_cast<char*>(&a), 1); }
/*! Encode \p a. */
inline void encode(std::ostream& os, char a) { os.write(reinterpret_cast<char*>(&a), 1); }

/*! Encode \p a. */
inline void encode_16(std::ostream& os, int16_t a) { a = htons(a); os.write(reinterpret_cast<char*>(&a), sizeof(a)); }
/*! Encode \p a. */
inline void encode(std::ostream& os, ushort a) { encode_16(os, a); }
/*! Encode \p a. */
inline void encode(std::ostream& os, short a) { encode_16(os, a); }

/*! Encode \p a. */
inline void encode_32(std::ostream& os, int32_t a) { a = htonl(a); os.write(reinterpret_cast<char*>(&a), sizeof(a)); }
/*! Encode \p a. */
inline void encode(std::ostream& os, uint a) { encode_32(os, a); }
/*! Encode \p a. */
inline void encode(std::ostream& os, int a) { encode_32(os, a); }

/*! Encode \p a. */
inline void encode_64(std::ostream& os, int64_t a) {
    uint32_t l, h;                 // low, high
    memcpy(&l, reinterpret_cast<char*>(&a) + 0,         sizeof(l));
    memcpy(&h, reinterpret_cast<char*>(&a) + sizeof(l), sizeof(h));
    encode_32(os, h);          // big end first (big-endian)
    encode_32(os, l);
}

/*! Encode \p a. */
inline void encode_long(std::ostream& os, long a) { (sizeof(long) == 8) ? encode_64(os, a) : encode_32(os, a); }
/*! Encode \p a. */
inline void encode(std::ostream& os, ulong a) { encode_long(os, a); }
/*! Encode \p a. */
inline void encode(std::ostream& os, long a) { encode_long(os, a); }

/*! Encode \p a (using 8 bits). */
inline void encode(std::ostream& os, bool a) { encode(os, static_cast<uint8_t>(a)); }

/*! Encode \p a. */
inline void encode(std::ostream& os, float a) {
    uint32_t t;
    memcpy(&t, &a, sizeof(t));
    encode_32(os, t);
}
/*! Encode \p a. */
inline void encode(std::ostream& os, double a) {
    uint32_t l, h;                 // low, high
    memcpy(&l, reinterpret_cast<char*>(&a) + 0,         sizeof(l));
    memcpy(&h, reinterpret_cast<char*>(&a) + sizeof(l), sizeof(h));
    encode_32(os, h);          // big end first (big-endian)
    encode_32(os, l);
}

/*! Encode \p a. */
inline void encode(std::ostream& os, const std::vector<bool>& a) {
    // size
    const size_t size = a.size(); encode(os, size);
    auto b = begin(a);
    // m bytes
    const size_t bsz = 1;	// block size (in bytes)
    const size_t m = size / (8 * bsz); // number of whole bytes
    for (size_t i = 0; i < m; i++)
    {
        char ch;		// no zero padding needed
        for (size_t j = 0; j < (8 * bsz); j++) {
            pnw::ip_put_bit(ch, j, *b); // set j:th bit in ch
            b++;
        }
        encode(os, ch);
    }
    // n (1 to 7) rests bits padded with zero bits (one byte)
    const size_t n = size % (8 * bsz); // number of bits left
    if (n > 0) {
        char ch = 0;             // pad with zeros
        for (size_t j = 0; j < n; j++) {
            pnw::ip_put_bit(ch, j, *b); // set j:th bit in ch
            b++;
        }
        encode(os, ch);
    }
}

/*! Encode \p a. */
inline void encode_cstr(std::ostream& os, const char* a) {
    auto n = strlen(a);
    encode(os, n);
#if HAVE_CXX11_LAMBDA_EXPRESSIONS
    std::for_each(a, a+n, [&os](char e) { encode(os, e); });
#else
    for (size_t i = 0; i != n; i++) { encode(os, e[i]); }
#endif
}
/*! Encode \p a. */
inline void encode(std::ostream& os, const std::string& a) {
    encode(os, a.size());
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
    for (const auto& e : a) { encode(os, e); }
#elif HAVE_CXX11_LAMBDA_EXPRESSIONS
    std::for_each(begin(a), end(a), [&os](char e) { encode(os, e); });
#endif
}
/*! \em Nullsafely Encode the pointer \p a, that is encode its dereferenced value only if it is non-null.  */
template<class T> inline void nullsafe_encode(std::ostream& os, const T* a) {
    encode(os, static_cast<bool>(a)); // nullflag
    if (a) { encode(os, *a); }  // optional value
}
/*! Encode Pointer to \p a. */
template<class T> inline void encode(std::ostream& os, const T* a) { nullsafe_encode(os, a); }
template<class T> inline void encode(std::ostream& os, const std::unique_ptr<T> a) { nullsafe_encode(os, a.get()); }

/*! Encode STL Container \em Elements \p a. */
template<class C> inline void encode_elements(std::ostream& os, const C& a) {
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
    for (const auto& e : a) { encode(os, e); }
#elif HAVE_CXX11_LAMBDA_EXPRESSIONS
    std::for_each(begin(a), end(a), [&os](const typename C::value_type& e) { encode(os, e); });
#endif
}

/*! Encode STL Container \p a. */
template<class C> inline void encode(std::ostream& os, const C& a) {
    encode(os, a.size());
    encode_elements(os, a);
}

template<class T, std::size_t N> inline void encode(std::ostream& os, const std::array<T,N>& a) {
    encode_elements(os, a);     // size is known at compile time
}
