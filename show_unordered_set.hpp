/*! \file show_unordered_set.hpp
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-09 12:56
 */

#pragma once
#include "show.hpp"
#include <unordered_set>
#include <boost/unordered_set.hpp>

/*! Print \p v to \p os. */
template<class T>
inline std::ostream& operator << (std::ostream& os, const std::unordered_set<T>& v)
{
    show_each(v, os, "{", "}");
    os << " : std::unordered_set<" << typenameof<T>() << ">";
    return os;
}

/*! Print \p v to \p os. */
template<class T>
inline std::ostream& operator << (std::ostream& os, const boost::unordered_set<T>& v)
{
    show_each(v, os, "{", "}");
    os << " : boost::unordered_set<" << typenameof<T>() << ">";
    return os;
}
