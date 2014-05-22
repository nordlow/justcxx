/*!
 * \file SMODE_enum.h
 * \brief Search Mode Enumeration Type
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Search Mode. */
typedef enum {
  SMODE_PREFIX = 0,             /**< Prefix */
  SMODE_SUBSTRING = 1,          /**< Substring */
  SMODE_FUZZY = 2,              /**< Fuzzy Search */

  SMODE_NUM = 3
} __attribute__((packed)) SMODE_t;

#ifdef __cplusplus
}
#endif
