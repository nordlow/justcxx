/*! \file show_unordered_map.hpp
 * \brief Show std::unordered_map.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-09 12:54
 */

#pragma once
#include "show.hpp"
#include <unordered_map>
#include <boost/unordered_map.hpp>

/*! Print Each Element in \p v to \p os. */
template<class T, class U>
inline std::ostream& show_each(const std::unordered_map<T, U>& v,
                               std::ostream& os = std::cout,
                               const char * opening = "[",
                               const char * closing = "]",
                               const char * separator = " ")
{
    os << opening << ' ';
    for (const auto& e : v) {         // TODO: Provide std::for_each aswell.
        os << e;
        //show(os, e);
        //show_each(e, os, "", "", "→");     // skip element type string
        os << separator;
    }
    os << closing;
    return os;
}

/*! Print Each Element in \p v to \p os. */
template<class T, class U>
inline std::ostream& show_each(const boost::unordered_map<T, U>& v,
                               std::ostream& os = std::cout,
                               const char * opening = "[",
                               const char * closing = "]",
                               const char * separator = " ")
{
    os << opening << ' ';
    for (const auto& e : v) {         // TODO: Provide std::for_each aswell.
        os << e;
        //show(os, e);
        //show_each(e, os, "", "", "→");     // skip element type string
        os << separator;
    }
    os << closing;
    return os;
}

/*! Print \p v to \p os. */
template<class T, class U>
inline std::ostream& operator << (std::ostream& os, const std::unordered_map<T, U>& v)
{
    show_each(v, os, "{", "}");
    os << " : std::unordered_map<" << typenameof<T>() << "," << typenameof<U>() << ">";
    return os;
}

/*! Print \p v to \p os. */
template<class T, class U>
inline std::ostream& operator << (std::ostream& os, const boost::unordered_map<T, U>& v)
{
    show_each(v, os, "{", "}");
    os << " : boost::unordered_map<" << typenameof<T>() << "," << typenameof<U>() << ">";
    return os;
}
