/*!
 * \file PLOADSTATE_enum.h
 * \brief Progressive Load/Unload State.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-08-15 17:11
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Progressive Load/Unload State.
 */
typedef enum {
  PLOADSTATE_UNLOADED,          /**< Unloaded. */
  PLOADSTATE_LOADING,           /**< Loading. */
  PLOADSTATE_LOADED,            /**< Loaded */
  PLOADSTATE_UNLOADING          /**< Unloading. */
} __attribute__ ((packed))  PLOADSTATE_t;

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
