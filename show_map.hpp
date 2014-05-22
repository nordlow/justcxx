/*! \file show_map.hpp
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-09 12:55
 */

#pragma once
#include <map>

/*! Print Each Element in \p v to \p os. */
template<class T, class U>
inline std::ostream& show_each(const std::map<T, U>& v,
                               std::ostream& os = std::cout,
                               const char * opening = "[",
                               const char * closing = "]",
                               const char * separator = " ")
{
    os << opening << ' ';
    for (const auto& elt : v) {         // C++11. TODO: Provide std::for_each aswell.
        show_each(elt, os, "", "", "→");     // skip element type string
        os << separator;
    }
    os << closing;
    return os;
}

/*! Print \p v to \p os. */
template<class T, class U>
inline std::ostream& operator << (std::ostream& os, const std::map<T, U>& v)
{
    show_each(v, os, "{", "}");
    os << " : std::map<" << typenameof<T>() << "," << typenameof<U>() << ">";
    return os;
}
