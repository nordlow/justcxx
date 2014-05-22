/*!
 * \file VARG_enum.h
 * \brief (Variable) Argument Function Class.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Variable Argument Function Class
 */
typedef enum {
  VARG_0i1o,
  VARG_1i1o,
  VARG_2i1o,

  VARG_1i0o,
  VARG_1i1o,
  VARG_1i2o,

  VARG_MiMo,
} __attribute__ ((packed)) VARG_t;

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
