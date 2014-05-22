/*! \file show_vector.hpp
 * \brief Show std::vector.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-09 14:46
 */

#pragma once
#include <ostream>
#include <vector>
#include "show.hpp"

/*! Print \p v to \p os. */
inline std::ostream & show_each(const std::vector<bool> & v,
                                std::ostream& os = std::cout,
                                const char * opening = "[",
                                const char * closing = "]",
                                const char * separator = " ",
                                int grouping = 8)
{
    os << opening << ' ';
    size_t j = 0;
    for (const auto& elt : v) {
        show(os, elt);
        ++j;
        if (grouping) {
            if (j % 8 == 0 and j != v.size()) { os << '_'; }
        } else {
            os << separator;
        }
    }
    os << ' ' << closing;
    return os;
}

/*! Print \p v to \p os. */
template<class T>
inline std::ostream& operator << (std::ostream& os, const std::vector<T>& v) {
    show_each(v, os, "[", "]");
    os << " : std::vector<" << typenameof<T>() << ">";
    return os;
}
