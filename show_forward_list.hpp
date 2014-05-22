/*! \file show_forward_list.hpp
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "show.hpp"
#include <forward_list>

/*! Print \p v to \p os. */
template<class T>
inline std::ostream& operator << (std::ostream& os, const std::forward_list<T>& v)
{
    show_each(v, os, "(", ")", "->");
    os << " : std::forward_list<" << typenameof<T>() << ">";
    return os;
}
