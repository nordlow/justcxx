/*!
 * \file BINUM_enum.h
 * \brief Binary Number Type
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-11-17 14:33
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Binary Number Type
 */
typedef enum {
  BINUM_any_,
  BINUM_UINT8, BINUM_UINT16, BINUM_UINT24, BINUM_UINT32, BINUM_UINT64,
  BINUM_INT8, BINUM_INT16, BINUM_INT24, BINUM_INT32, BINUM_INT64,
  BINUM_F32, BINUM_F64, BINUM_F96,
  BINUM_MPZ, BINUM_MPQ,
  BINUM_undefined_
} __attribute__ ((packed)) BINUM_t;

#define BINUM_NUM (BINUM_undefined_)

#ifdef __cplusplus
}
#endif
