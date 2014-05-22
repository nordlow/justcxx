/*! \file decode.hpp
 * \brief Decoding (Unserialization) of basic C/C++ types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <iostream>
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
#include "decode.h"

/// Decode \p a.
template<class T>               inline void decode(std::ostream& os, const T& a) { return a; } // template termination
/// Decode \p a, \p and \p args.
template<class T, class ... R > inline void decode(std::ostream& os, const T& a, const R&... args) {
    decode(os, a);
    decode(os, args...);
}

/// Decode \p a.
template<class T, class U> inline void decode(std::ostream& os, const std::pair<T,U>& a) {
    decode(os, a.first);
    decode(os, a.second);
}

/// Decode \p a.
template<class T, class U> inline void decode(std::ostream& os, const std::tuple<T,U>& a) {
    decode(os, std::get<0>(a));
    decode(os, std::get<1>(a));
}
/// Decode \p a.
template<class T, class U, class V> inline void decode(std::ostream& os, const std::tuple<T,U,V>& a) {
    decode(os, std::get<0>(a));
    decode(os, std::get<1>(a));
    decode(os, std::get<2>(a));
}
/// Decode \p a.
template<class T, class U, class V, class W> inline void decode(std::ostream& os, const std::tuple<T,U,V,W>& a) {
    decode(os, std::get<0>(a));
    decode(os, std::get<1>(a));
    decode(os, std::get<2>(a));
    decode(os, std::get<3>(a));
}

/*! Decode \p a. */
inline void decode(std::istream& is, uchar& a) { is.read(reinterpret_cast<char*>(&a), 1); }
/*! Decode \p a. */
inline void decode(std::istream& is, char& a) { is.read(reinterpret_cast<char*>(&a), 1); }

/*! Decode \p a. */
inline int16_t decode_16(std::istream& is) { int16_t a; is.read(reinterpret_cast<char*>(&a), sizeof(a)); return ntohs(a); }
/*! Decode \p a. */
inline void decode(std::istream& is, ushort& a) { a = decode_16(is); }
/*! Decode \p a. */
inline void decode(std::istream& is, short& a) { a = decode_16(is); }

/*! Decode \p a. */
inline int32_t decode_32(std::istream& is) { int32_t a; is.read(reinterpret_cast<char*>(&a), sizeof(a)); return ntohl(a); }
/*! Decode \p a. */
inline void decode(std::istream& is, uint& a) { a = decode_32(is); }
/*! Decode \p a. */
inline void decode(std::istream& is, int& a) { a = decode_32(is); }

/*! Decode \p a. */
inline int64_t decode_64(std::istream& is) {
    auto h = decode_32(is);     // big end first (big-endian)
    auto l = decode_32(is);
    int64_t a;
    memcpy(reinterpret_cast<char*>(&a) + 0,         &l, sizeof(l));
    memcpy(reinterpret_cast<char*>(&a) + sizeof(l), &h, sizeof(h));
    return a;
}

/*! Decode \p a. */
inline long decode_long(std::istream& is) { return (sizeof(long) == 8) ? decode_64(is) : decode_32(is); }
/*! Decode \p a. */
inline void decode(std::istream& is, long& a) { a = decode_long(is); }
/*! Decode \p a. */
inline void decode(std::istream& is, ulong& a) { a = decode_long(is); }

/*! Decode a \c size_t into \p a as either @b 4 or @b 8 eight bytes depending on sizeof(size_t). */
inline void decode_size(std::istream& is, size_t& a)
{
    const size_t sz = sizeof(a);
    is.read(reinterpret_cast<char*>(&a), sz);
    if (sz == 4) { a = ntohl(a); }
    else if (sz == 8) { a = ntoh64(a); }
    else { // PWARN("sz is not supported\n");
    }
}

/*! Decode a \c a (using 8 bits). */
inline void decode(std::istream& is, bool& a) { uchar b; decode(is, b); a = b; }

/*! Decode \p a. */
inline void decode(std::istream& is, float& a)  { auto t = decode_32(is); memcpy(&a, &t, sizeof(t)); }
/*! Decode \p a. */
inline void decode(std::istream& is, double& a) { auto t = decode_64(is); memcpy(&a, &t, sizeof(t)); }

// ============================================================================

/*! Decode Size of \p a. */
template<class T> inline void decode_size(std::istream& is, std::vector<T>& a) {
    size_t size; decode(is, size); a.resize(size);
}

/*! Decode Elements of \p a. */
template<class T> inline void decode_elements(std::istream& is, std::vector<T>& a) {
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
    for (auto& e : a) { decode(is, e); }
#elif HAVE_CXX11_LAMBDA_EXPRESSIONS
    std::for_each(begin(a), end(a), [&os](typename C::value_type& e) { de4code(os, e); });
#endif
}

/*! Decode \p a. */
template<class T> inline void decode(std::istream& is, std::vector<T>& a)
{
    decode_size(is, a);
    decode_elements(is, a);
}

/*! Decode \p a. */
inline void decode(std::istream& is, std::vector<bool>& a)
{
    decode_size(is, a);

    auto b = begin(a);

    // m bytes
    const size_t bsz = 1;	// block size (in bytes)

    const size_t m = a.size() / (8 * bsz); // number of whole bytes
    for (size_t i = 0; i < m; i++) {
        char ch;
        decode(is, ch);

        for (size_t j = 0; j < (8 * bsz); j++)
        {
            *b = pnw::get_bit(ch, j); // get j:th bit in ch (which is byte i)
            b++;
        }
    }

    // n (1 to 7) rests bits padded with zero bits (one byte)
    const size_t n = a.size() % (8 * bsz); // number of bits left
    if (n > 0) {
        char ch;
        decode(is, ch);

        for (size_t j = 0; j < n; j++)
        {
            *b = pnw::get_bit(ch, j); // get j:th bit in ch (which is byte i)
            b++;
        }
    }
}

/*! Decode \p a. */
inline void decode_cstr(std::istream& is, char *& a)
{
    size_t n;
    decode(is, n);
    a = new char[n + 1];
    for (size_t i = 0; i < n; i++) { decode(is, a[i]); }
    a[n] = '\0';
}
/*! Decode \p a with size checking.
 * \return 0 if everything went ok, otherwise -1.
 */
inline int safe_decode_cstr(std::istream& is, char *& a,
                            size_t nmax)
{
    size_t n; decode(is, n);
    if (n > nmax) { return -1; }
    a = new char[n + 1];
    for (size_t i = 0; i < n; i++) { decode(is, a[i]); }
    a[n] = '\0';
    return 0;
}

/*!* Decode \p a. */
inline void decode(std::istream& is, std::string& a)
{
    size_t n;
    decode(is, n);
    a.resize(n);
    for (size_t i = 0; i < n; i++) {
        decode(is, a[i]);
    }
}

/*! Useful in Decoding Constructors.
 * For an example see Obj(std::istream& is).
 */
template<class T> inline T returned_decode(std::istream& is) { T a; decode(is, a); return a; }

/*! \em Nullsafely Decode the pointer \p a, that is decode it only if the preceeding nullflag is set.
 * Value of argument \p a at call is not handled.
 */
template<class T> inline void decode_nullsafe(std::istream& is, T*& a) {
    bool b; decode(is, b);      // nullflag
    if (b) {                    // if value
        a = new T;		// NOTE: No non-null check of a here!
        decode(is, *a);
    } else {
        a = nullptr;
    }
}
template<class T> inline void decode(std::ostream& os, const T*& a) { nullsafe_decode(os, a); }
template<class T> inline void decode(std::ostream& os, std::unique_ptr<T> a) { T* p = nullptr; nullsafe_decode(os, p); a.reset(a); }

/*! Decode STL Container Elements \p a. */
template<class C> void decode_elements(std::istream& is, C& a) {
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
    for (auto& e : a) { decode(is, e); }
#elif HAVE_CXX11_LAMBDA_EXPRESSIONS
    std::for_each(begin(a), end(a), [&os](typename C::value_type& e) { decode(os, e); });
#endif
}

/*! Decode STL Container \p a. */
template<class C> inline void decode(std::istream& is, C& a) {
    std::size_t size; decode(is, size);
    a.resize(size); decode_elements(is, a);
}

template<class T, std::size_t N> inline void decode(std::istream& is, std::array<T,N>& a) {
    decode_elements(is, a);
}
