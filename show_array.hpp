/*! \file show_array.hpp
 * \brief Show std::array.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-09 14:46
 */

#pragma once
#include "show.hpp"
#include <array>

/*! Print \p v to \p os. */
template<class T, std::size_t N>
inline std::ostream& operator << (std::ostream& os, const std::array<T, N>& v) {
    show_each(v, os, "[", "]");
    os << " : std::array<" << typenameof<T>() << "," << N << ">";
    return os;
}
