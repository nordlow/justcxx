/*! \file shapelayer.hpp
 * \brief Shape Layer.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <math.h>
#include "utils.h"

/*! Shape Layer. */
typedef uint32_t layer_t;

#define VLAYER_LOWEST (UINT16_MIN) /**< Lowest Object Layer (possible) */
#define VLAYER_HIGHEST (UINT16_MAX-1) /**< Highest object layer (possible) */
