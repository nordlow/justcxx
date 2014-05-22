/*! \file adaptive_unordered_map.hpp
 * \brief Adaptive Version of \© unordered_map. Should automatically pick \c nedtrie when using int-type like keys (up to 64-bit).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(HAVE_UNORDERED_MAP)
# include <unordered_map>
#elif defined(HAVE_TR1_UNORDERED_MAP)
# include <tr1/unordered_map>
#elif defined(HAVE_BOOST_UNORDERED_MAP_HPP)
# include <boost/unordered_map.hpp>
# include "boost/tuple/tuple.hpp"
#else
# include <unordered_map>
#endif

#include "deps/nedtries/nedtrie.h"

template<Tp>
class adaptive_unordered_map
{
};
