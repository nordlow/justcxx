/*! \file CORNSTYLE_enum.hpp
 * \brief Edge type used to specify box rounding types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/*! Corner Style Type Code. */
typedef enum {
  CORNSTYLE_ELL,                /**< Elliptic. */
  CORNSTYLE_ELL_MIN,            /**< Circular Minimum (Absolute Rounding Radius) */
  CORNSTYLE_ELL_MAX,            /**< Circular Maximum (Absolute Rounding Radius) */

  CORNSTYLE_LIN,                /**< Linear (Straight Line). */
  CORNSTYLE_LIN_MIN,            /**< Circular Minimum (Absolute Rounding Radius) */
  CORNSTYLE_LIN_MAX,            /**< Circular Maximum (Absolute Rounding Radius) */
} __attribute__ ((packed)) CORNSTYLE_t;
