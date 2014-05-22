/*! \file show_binary.hpp
 * \brief Show Binary.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-08-24 22:40
 */

#pragma once
#include <ostream>
#include "bitwise.hpp"

/*! Print \em all the bits contained in \p a left to right. */
template<class T> inline std::ostream& show_binary(std::ostream& os, T a, char separator = '_') {
    for (size_t i = 0; i < 8*sizeof(T); i++) {
        if (i and i % 8 == 0 and i+1 < 8*sizeof(T)) { os << separator; } // print separator
        os << (pnw::get_bit(a, i) ? '1' : '0');
    }
    return os;
}
template<class T> inline std::ostream& show_binary(T a) { return show_binary(std::cout, a); }
template<> inline std::ostream& show_binary(float a) { uint32_t b; memcpy(&b, &a , sizeof(a)); return show_binary(std::cout, b); } ///< TODO: Make warning when using *reinterpret_cast<uint32_t*>(&a) go away
template<> inline std::ostream& show_binary(double a) { uint64_t b; memcpy(&b, &a , sizeof(a)); return show_binary(std::cout, b); } ///< TODO: Make warning when using *reinterpret_cast<uint64_t*>(&a) go away

/*! Print \p a showing the \p n \em least \em signficant bits. */
template<class T> inline std::ostream& show_binary(std::ostream& os,
                                                   T a,
                                                   uint n,
                                                   char separator = '_',
                                                   uint group_length = 8) {
    if (n > 0) {
        for (ssize_t i = n-1; i >= 0; i--) {
            if (i and           // if not first
                i % group_length == 0    // and between groups
                and i+1 < 8*sizeof(T)) { // not last
                os << separator; // print separator
            }
            os << (pnw::get_bit(a, i) ? '1' : '0');
        }
    }
    return os;
}
/*! Print \p a showing the \p n \em least \em signficant bits. */
template<class T> inline std::ostream& show_binary(T a, int n, char separator = '_', uint group_length = 8) {
    return show_binary(std::cout, a, n, separator, group_length);
}
