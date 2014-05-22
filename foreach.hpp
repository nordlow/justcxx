/*!
 * \file foreach.hpp
 * \brief Wrapper for forward and backward (reverse) foreach.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://www.boost.org/doc/libs/1_48_1/doc/html/foreach.html
 */

#pragma once
#include <boost/foreach.hpp>

#define foreach         BOOST_FOREACH ///< \em Forward \c foreach.
#define reverse_foreach BOOST_REVERSE_FOREACH // \em Backward (reverse) \c foreach.
