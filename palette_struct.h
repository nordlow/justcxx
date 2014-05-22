/*!
 * \file palette_struct.h
 * \brief Palette Structure.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "color.hpp"
#include "color_constants.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */

/*!
 * Set of Colors.
 */
typedef struct cpal_t {
  color_t * cA;                 /**< Palette Elements. */
  size_t    cN;                 /**< Number of Palette Elements. */
} cpal_t;

/* ========================================================================== */

#ifdef __cplusplus
}
#endif
