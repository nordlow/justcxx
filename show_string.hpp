/*! \file show_string.hpp
 * \brief Show std::string.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <ostream>
#include <string>

// Force Double Quotes around C++ STL strings.
inline std::ostream& show(std::ostream& os, const std::string& a) {
    return os << "\"" << a << "\"";
}
