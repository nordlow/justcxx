/*! \file bix.hpp
 * \brief Bit Index and Range.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-09 14:09
 */

#pragma once
#include "range.hpp"
#include "saturated.hpp"

/*! Bit Index Type. */
typedef uint64_t bix_type;

/*! Bit \em Index/Size/Length with Saturation Arithmetic. */
typedef saturated<bix_type> bix;
template<> inline pure const char* unitof<bix>() { return "bit"; }

inline bix operator"" _bix(unsigned long long val) { return bix(val); }

/*! Down-Convert Bit Offset \p a to Nearest Floored Byte offset. */
inline spure bix_type floored_to_byte(bix a) { return a.get() / 8; }
inline spure bix_type to_rest(bix a) { return a.get() % 8; }

/*! Convert Bit Offset \p a to Byte offset. */
inline spure bix_type to_byte(bix a) { enforce_zero(a.get() % 8); return a.get() / 8; }

/*! Convert Bit Offset \p a to Padded Byte offset. */
inline spure bix_type to_padded_byte(bix a) { auto n = a.get(); return n/8 + (n%8 ? 1 : 0) ; }

/*! Convert Byte Offset \p a to Bit offset. */
inline spure bix to_bit(bix_type a) { return bix(a * 8); }

/*! Check if bit size \p a is one bit. */
inline spure bool is_bit(bix a) { return a.get() == 1; }
/*! Check if bit size \p a is an even byte size. */
inline spure bool is_byte(bix a) { return a.get() == 8; }

/*! Bit Index \em Range. */
typedef range<bix> bir;

/*! Check if size of range \p a is one bit. */
inline bool is_bit(const bir& a) { return (a.bitsize() == bix::one()); }
/*! Check if size of range \p a is one byte. */
inline bool is_byte(const bir& a) { return (a.bitsize() == bix(8)); }

/*! Convert Bit Range \p a to Byte Range. */
inline spure range<bix_type> to_byte(bir a) { return range<bix_type>(to_byte(a.low()),
                                                                     to_byte(a.high())); }

/*! Get \em Byte Element in \p a at index \p i.  */
inline uchar  ix(const uchar* a, bix i) { return a[to_byte(i)]; }
inline uchar& ix(uchar*       a, bix i) { return a[to_byte(i)]; }

inline char  ix(const char* a, bix i) { return a[to_byte(i)]; }
inline char& ix(char*       a, bix i) { return a[to_byte(i)]; }

/*! Get \em Bit Element in \p a at index \p i.  */
inline bool ixb(const uchar* a, bix i) { return a[floored_to_byte(i)] >> to_rest(i); }
