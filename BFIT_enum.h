/*! \file BFIT_enum.h
 * \brief Box Fitting Type Code.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Box Fitting Type Code.
 */
typedef enum {
  BFIT_INNER_X,			/**< Inside X-dimensions. */
  BFIT_INNER_Y,			/**< Inside Y-dimensions. */
  BFIT_OUTER_X,			/**< Outside X-dimensions. */
  BFIT_OUTER_Y,			/**< Outside Y-dimensions. */
  BFIT_XY                       /**< Along both X and Y-dimension. */
} __attribute__ ((packed)) BFIT_t;

#ifdef __cplusplus
}
#endif
