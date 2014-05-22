/*! \file show_list.hpp
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "show.hpp"
#include <list>

/*! Print \p v to \p os. */
template<class T>
inline std::ostream& operator << (std::ostream& os, const std::list<T>& v)
{
    show_each(v, os, "(", ")", "->");
    os << " : std::list<" << typenameof<T>() << ">";
    return os;
}
