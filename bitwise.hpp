/*! \file bitwise.hpp
 * \brief Bitwise Operations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see http://graphics.stanford.edu/~seander/bithacks.html
 * \see http://corner.squareup.com/2013/07/reversing-bits-on-arm.html
 * \see http://stackoverflow.com/questions/746171/best-algorithm-for-bit-reversal-from-msb-lsb-to-lsb-msb-in-c
 * \see http://www.ibm.com/developerworks/library/l-port64.html
 * \see http://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Other-Builtins.html
 * \todo Implement http://bmagic.sourceforge.net/articles.html
 */

#pragma once
#include "cc_features.h"
#include <limits>

namespace pnw
{

/*! Get type's size in bits. */
template<class T> inline spure size_t bitsizeof() { return 8 * sizeof(T); }
/*! Get type's size in bits. */
template<class T> inline spure size_t bitsizeof(T a) { return 8 * sizeof(a); }

/*! \name Bit Operations
 * Operations for accessing individual bits in types supporting normal bit
 * operations (normally integer types).
 */
/// \{

/*! Get \p i:th bit of \p a as a \c bool. */
template<class T> inline spure bool get_bit(T a, uint i) { return (a & (static_cast<T>(1) << i)) != 0; }

/*! Get \p a with the \p i:th bit \em set. */
template<class T> inline spure T set_bit(T a, uint i) { return (a | (static_cast<T>(1) << i)); }
/*! Get \p a with the \p i:th bit \em cleared. */
template<class T> inline spure T clr_bit(T a, uint i) { return (a & ~(static_cast<T>(1) << i)); }

/*! Set \p i:th and \p j:th bit of \p a to 1 if \p a is non-zero or to 0 if \p a is zero. */
template<class T> inline spure T set_bits(T a, uint i, uint j) { return (a | (1 << i) | (1 << j)); }
/*! Clear the \p i:th and \p j:th bit of \p a to 1 if \p a is non-zero or to 0 if \p a is zero. */
template<class T> inline spure T clr_bits(T a, uint i, uint j) { return (a & ~(1 << i) & ~(1 << j)); }

/*! Set \p i:th and \p j:th bit of \p a to 1 if \p a is non-zero or to 0 if \p a is zero. */
template<class T> inline spure T set_bits(T a, uint i, uint j, uint k, uint l) { return (a | (1 << i) | (1 << j) | (1 << k) | (1 << l)); }
/*! Clear the \p i:th and \p j:th bit of \p a to 1 if \p a is non-zero or to 0 if \p a is zero. */
template<class T> inline spure T clr_bits(T a, uint i, uint j, uint k, uint l) { return (a & ~(1 << i) & ~(1 << j) & ~(1 << k) & ~(1 << l)); }

/*! Set i:th bit of \p a to the value of \c b. */
template<class T> inline spure T put_bit(T a, uint i, bool b) { return b ? set_bit(a, i) : clr_bit(a, i); }

/*! \em Clear (assign it to 0) the i:th bit of \p a. */
template<class T> inline void ip_clr_bit(T & a, uint i) { a &= ~(static_cast<T>(1) << i); }
/*! \em Set (assign it to 1) the i:th bit of \p a. */
template<class T> inline void ip_set_bit(T & a, uint i) { a |= static_cast<T>(1) << i; }
/*! \em Set (assign it to 1) the i:th bit of \p a if b is 1. */
template<class T> inline void ip_set_bit(T & a, uint i, bool b) { if (b) { set_bit(a, i); } }
/*! \em Set the i:th bit of \p a to the value of \c b. */
template<class T> inline void ip_put_bit(T & a, uint i, bool b) { b ? ip_set_bit(a, i) : ip_clr_bit(a, i); }

/*! Bitwise Rotate x n steps to the left. */
template<class T> inline spure T bit_rotl(T a, uint n) { return (a) << (n) bitor (a) >> (8 * sizeof(T) - (n)); }
/*! Bitwise Rotate a n steps to the right. */
template<class T> inline spure T bit_rotr(T a, uint n) { return (a) << (n) bitor (a) >> (8 * sizeof(T) - (n)); }

/*! This alternative is NOT faster on my machine also for uint32_t. Does anyone
 * understand why?
 */
#if defined (__GNUC__) and __GNUC__ >= 2 and defined (__i386__) and (0)
inline spure uint32_t bit_rotl(uint32_t a, uint n)
{
    uint a;
    __asm__ volatile ("roll %%cl, %0"
                      : "=r" (a)
                      : "0" (a), "c" (n));
    return a;
}
inline spure uint32_t bit_rotr(uint32_t a, uint n)
{
    uint a;
    __asm__ volatile ("rorl %%cl, %0"
                      : "=r" (a)
                      : "0" (a), "c" (n));
    return a;
}
#endif

/*! Check if \p a is an exact power of two.
 * \see http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
 */
template<class T> inline spure bool is_binpow(T a) { return a && !(a & (a-1)); }
template<class T> inline spure bool is_binpow_alt(T a) { return a & -a; }

/*! Get smallest value c that is an exact power of two p (c = 2^p) and
 *  where c >= a.
 *
 * Often useful when allocating memory for dynamic arrays that
 * continually grow and/or shrink in size.
 */
template<class T> inline spure T binceil(T a) {
    T c = 1;                    // ceiling
    while (c < a) { c <<= 1; }
    return c;
}

/*! Get Smallest Integer \p, where 2^p >= a and a > 0.
 *
 * If \p a is zero result is undefined.
 *
 * Often useful when allocating memory for dynamic arrays that
 * continually grow and/or shrink in size.
 *
 * \see http://stackoverflow.com/questions/2589096/find-most-significant-bit-left-most-that-is-set-in-a-bit-array
 * \see http://stackoverflow.com/questions/671815/what-is-the-fastest-most-efficient-way-to-find-the-highest-set-bit-msb-in-an-i
 *
 * \alias \c bsr (Bit scan reverse)
 * \alias binlog, msb
 * \todo Can we optimize this like with \c binceil()?
 */
template<class T>
inline spure int binlog_generic(T a)
{
    const int bsz = sizeof(a) * 8; // bitsize
    if (get_bit(a, bsz-1)) { /* most signficant bit is set */
        if (is_binpow(a)) { return bsz-1; } else { return bsz; }
    }
    int p = 0;                  /* power */
    T c = 1;                  /* ceiling */
    while (c < a) { p++; c <<= 1; }
    return p;
}
template<class T> inline spure int binlog(T a) { return binlog_generic(a); }
#if defined(__GNUC__)
inline spure int binlog(uint a) { return 8*sizeof(a) - __builtin_clz(a); }
inline spure int binlog(unsigned long a) { return 8*sizeof(a) - __builtin_clzl(a); }
inline spure int binlog(unsigned long long a) { return 8*sizeof(a) - __builtin_clzll(a); }
#endif

#if defined(__VISUAL_C__)
#include <intrin.h>
// \see http://msdn.microsoft.com/en-us/library/fbxyd7zd%28v=VS.80%29.aspx
unsigned char _BitScanReverse(
    unsigned long * Index,
    unsigned long Mask
                              );
unsigned char _BitScanReverse64(
    unsigned long * Index,
    unsigned __int64 Mask
                                );
inline spure int binlog(uint a) { return ...; }
inline spure int binlog(unsigned long long a) { return ...; }
#endif

// ----------------------------------------------------------------------------

/*! Zero-Offset Position of Most Significant Bit (MSB) Set. */
template<class T> inline spure int msb(T a) { return binlog(a)-1; }

/*! Zero-Offset Position of Least Significant Bit (LSB) Set. */
template<class T>
inline spure T lsb(T a) { return a & -a; }
#if defined(__GNUC__)
inline spure int lsb(uint a) { return __builtin_ctz(a); }
inline spure int lsb(unsigned long a) { return __builtin_ctzl(a); }
inline spure int lsb(unsigned long long a) { return __builtin_ctzll(a); }
#endif

typedef unsigned char uchar;

template<class T> inline spure uchar get_byte(T a, uint i) { return (a >> (8 * i)) bitand 0xff; }
template<class T> inline void ip_clear_byte(T & a, uint i) { a &= ~(static_cast<T>(0xff) << (8 * i)); }
template<class T> inline void ip_set_byte(T & a, uint i, uchar b) { clear_byte(a, i); a |= static_cast<T>(b) << (8 * i); /*set bits*/ }
template<class T> inline void ip_copy_byte(T & a, int i, T b, int j) { set_byte(a, i, get_byte(b, j)); }

/*! \name BitMask
 * Generate a bit mask where the bits indexed by [0 n] are 1, others 0.
 */
/// \{
template<class T> inline spure T bitmask(uint n) {
    if (n >= sizeof(T) * 8 - 1) {
        return std::numeric_limits<T>::max();
    } else {
        return (static_cast<T>(1) << (n + 1)) - 1;
    }
}
inline spure uint bitmask(uint n) {
    if (n >= sizeof(uint) * 8 - 1) {
        return std::numeric_limits<uint>::max();
    } else {
        return (1 << (n + 1)) - 1;
    }
}
/// \}

/*! Mask out the n lower bits of \p a.
 * \todo THE correct version of bitmask.
 */
template<class T> inline spure T mask_bits(T a, uint n) {
    uint s = sizeof(uint) * 8 - n; // shift
    return (a << s) >> s;
}

/*! Get a Random Bit. */
inline bool get_random_bit() { return pnw::get_bit(std::rand(), 15); }

#if defined (_GLIBCXX_VECTOR) || defined (_STL_VECTOR_H) ///< If <vector> was included
/*! Randomly Change (Set or Clear) \p n number of bits in \p a
 * If \p b is
 * - 1: Change a maximum of \p n zeros in \p a to one.
 * - 0: Change a maximum of \p n ones in \p a to zero.
 */
inline void random_assign_bits(std::vector<bool>& a,
                               size_t n, bool b)
{
    std::vector<size_t> ix;
    for (size_t i = 0; i < a.size(); i++) {
        if (b) {
            if (not a[i]) {
                ix.push_back(i); // store index that contains a zero
            }
        } else {
            if (a[i]) {
                ix.push_back(i); // store index that contains a one
            }
        }
    }

    // the random part
    std::random_shuffle(begin(ix), end(ix));
    n = std::min(static_cast<size_t>(n), ix.size());
    for (size_t i = 0; i < n; i++) {
        a[ix[i]] = b; // at index set either zero or one depending on b
    }
}
#endif

}
