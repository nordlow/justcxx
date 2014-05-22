/*! \file: show_units.hpp
 * \brief: Pretty Output of Units.
 * \author: Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date: 2012-05-13 22:52
 */

#pragma once
#include <iostream>
#include <algorithm>
#include "units.hpp"

namespace pnw {
/*! Convert and Print \p x to \p os to SI-postfixes format. */
template<class T>
inline std::ostream& siunit_show(const T& x, std::ostream& os = std::cout)
{
    std::pair<T,uchar> y = siunit_convert(x);
    os << y.first;
    if (y.second) { os << y.second; }
    return os;
}
}
