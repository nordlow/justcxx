/*!
 * \file show_set.hpp
 * \brief Show std::set.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-09 12:56
 */

#pragma once
#include "show.hpp"
#include <set>

/*! Print \p v to \p os. */
template<class T>
inline std::ostream& operator << (std::ostream& os, const std::set<T>& v) {
    show_each(v, os, "{", "}");
    os << " : std::set<" << typenameof<T>() << ">";
    return os;
}
