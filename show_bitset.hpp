/*! \file show_bitset.hpp
 * \brief Show std::bitset.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-09 14:45
 */

#pragma once
#include "show.hpp"
#include <bitset>

/*! Print \p v to \p os. */
template<size_t N>
inline std::ostream & show_each(const std::bitset<N> & v,
                                std::ostream& os = std::cout,
                                const char * opening = "[",
                                const char * closing = "]",
                                const char * separator = " ",
                                uint grouping = 8)
{
    os << opening << ' ';
    for (size_t i = 0; i < v.size(); i++) { // todo: Use algorithm
        show(os, v[i]);
        if (grouping) {
            auto j = i+1;
            if (j % 8 == 0 and j != v.size()) { os << '_'; }
        } else {
            os << separator;
        }
    }
    os << ' ' << closing;
    return os;
}

/*! Print \p v to \p os. */
template<size_t N>
inline std::ostream& operator << (std::ostream& os, const std::bitset<N>& v) {
    show_each(v, os, "[", "]");
    os << " : std::bitset<" << N << ">";
    return os;
}
