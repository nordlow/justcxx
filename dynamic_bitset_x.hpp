/*! \file dynamic_bitset_x.hpp
 * \brief Extensions to boost::dynamic_bitset.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-13 14:58
 */

#pragma once
#include <boost/dynamic_bitset.hpp>
#include "bitwise.hpp"

namespace boost {

template<class Block>
inline dynamic_bitset<Block>& append(dynamic_bitset<Block>& bs, uint8_t a)
{
    using pnw::get_bit;
    bs.push_back(get_bit(a,7));
    bs.push_back(get_bit(a,6));
    bs.push_back(get_bit(a,5));
    bs.push_back(get_bit(a,4));
    bs.push_back(get_bit(a,3));
    bs.push_back(get_bit(a,2));
    bs.push_back(get_bit(a,1));
    bs.push_back(get_bit(a,0));
    return bs;
}

// append big-endian
template<class Block> inline dynamic_bitset<Block>& append_be(dynamic_bitset<Block>& bs, uint16_t a) {
    return bs << static_cast<uint8_t>(a >> 8)  // big
              << static_cast<uint8_t>(a >> 0); // little
}
template<class Block> inline dynamic_bitset<Block>& append_be(dynamic_bitset<Block>& bs, uint32_t a) {
    return bs << static_cast<uint16_t>(a >> 16) // big
              << static_cast<uint16_t>(a >> 0); // little
}
template<class Block> inline dynamic_bitset<Block>& append_be(dynamic_bitset<Block>& bs, uint64_t a) {
    return bs << static_cast<uint32_t>(a >> 32) // big
              << static_cast<uint32_t>(a >> 0); // little
}

// append little-endian
template<class Block> inline dynamic_bitset<Block>& append_le(dynamic_bitset<Block>& bs, uint16_t a) {
    return bs << static_cast<uint8_t>(a >> 0)   // little
              << static_cast<uint8_t>(a >> 8);  // big
}
template<class Block> inline dynamic_bitset<Block>& append_le(dynamic_bitset<Block>& bs, uint32_t a) {
    return bs << static_cast<uint16_t>(a >> 0)   // little
              << static_cast<uint16_t>(a >> 16); // big
}
template<class Block> inline dynamic_bitset<Block>& append_le(dynamic_bitset<Block>& bs, uint64_t a) {
    return bs << static_cast<uint32_t>(a >> 0)   // little
              << static_cast<uint32_t>(a >> 32); // big
}

template<class Block> inline dynamic_bitset<Block>& operator<<(dynamic_bitset<Block>& bs, bool     a) { bs.push_back(a); return bs; }
template<class Block> inline dynamic_bitset<Block>& operator<<(dynamic_bitset<Block>& bs, uint8_t  a) { return append(bs, a); }
template<class Block> inline dynamic_bitset<Block>& operator<<(dynamic_bitset<Block>& bs, uint16_t a) { return append_be(bs, a); }
template<class Block> inline dynamic_bitset<Block>& operator<<(dynamic_bitset<Block>& bs, uint32_t a) { return append_be(bs, a); }
template<class Block> inline dynamic_bitset<Block>& operator<<(dynamic_bitset<Block>& bs, uint64_t a) { return append_be(bs, a); }

}
