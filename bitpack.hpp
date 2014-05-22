/*! \file bitpack.hpp
 * \brief Simple Bit-Packing Codec.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "algorithm_x.hpp"
#include "encode.hpp"
#include "decode.hpp"
#include <vector>

/*! \name Bit-packing codec.
 * Determines the minimum number of bits needed to store the maximum
 * value of the vector this encoder compresses the information somewhat.
 */

/// \{

/*! Pack and Encode Indexes. */
template<class T>
void bitpack_encode(std::ostream & os, const std::vector<T> & a)
{
    // determine the maximum index value
    const T max = *std::max_element(begin(a), end(a));

    // and use it to determine the number of bits needed to encode an index
    const uint nbits = std::binlog(max);
    ::encode(os, nbits);

    const size_t sz = a.size();

    std::vector<bool> v(sz * nbits);

    // pack indexes
    auto vi = begin(v);
    for (size_t i = 0; i < sz; i++)
    {
	for (uint j = 0; j < nbits; j++)
	{
	    *vi = pnw::get_bit(a[i], j);
	    vi++;
	}
    }

    // encode bits
    ::encode(os, v);
}

/*! Decode and Unpack Indexes. */
template<class T>
void bitpack_decode(std::istream & is, std::vector<T> & a)
{
    // number of bits needed to store one index
    uint nbits;
    ::decode(is, nbits);

    std::vector<bool> v;

    // decode bits
    ::decode(is, v);

    size_t sz = v.size() / nbits;	// NOTE: possible truncation of bits
    a.resize(sz);

    // unpack indexes
    auto vi = begin(v);
    for (size_t i = 0; i < sz; i++)
    {
	for (uint j = 0; j < nbits; j++)
	{
	    pnw::put_bit(a[i], j, *vi);
	    vi++;
	}
    }
}

// \}
