/*! \file pnw_types.h
 * \brief Abbreviations of C types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <stdlib.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \name Limits for bit-specific types.
 */
/* @{ */
#ifndef  UINT8_MAX
#  define  UINT8_MAX ((uint8_t) 255)
#endif
#ifndef  INT8_MAX
#  define  INT8_MAX ((int8_t) 127)
#endif
#ifndef  INT8_MIN
#  define  INT8_MIN ((int8_t) -128)
#endif
#ifndef UINT16_MAX
#  define UINT16_MAX ((uint16_t) 65535)
#endif
#ifndef INT16_MAX
#  define INT16_MAX ((int16_t) 32767)
#endif
#ifndef INT16_MIN
#  define INT16_MIN ((int16_t) -INT16_MAX - 1)
#endif
#ifndef UINT32_MAX
#  define UINT32_MAX ((uint32_t) 4294967295UL)
#endif
#ifndef INT32_MAX
#  define INT32_MAX ((int32_t) 2147483647L)
#endif
#ifndef INT32_MIN
#define INT32_MIN ((int32_t) -INT32_MAX - 1L)
#endif
#ifndef UINT64_MAX
#  define UINT64_MAX ((uint64_t) 18446744073709551615ULL)
#endif
#ifndef INT64_MAX
#  define INT64_MAX ((int64_t) 9223372036854775807LL)
#endif
#ifndef INT64_MIN
#  define INT64_MIN ((int64_t) -INT64_MAX - 1LL)
#endif
#ifndef F32_MAX
#  define F32_MAX ((f32_t) 3.40282347e+38F)
#endif
#ifndef F32_MIN
#  define F32_MIN ((f32_t) 1.17549435e-38F)
#endif
#ifndef F64_MAX
#  define F64_MAX ((f64_t) 1.7976931348623157e+308)
#endif
#ifndef F64_MIN
#  define F64_MIN ((f64_t) 2.2250738585072014e-308)
#endif
#ifdef __i386__
#  ifndef F96_MAX
#    define F96_MAX ((f96_t) LDBL_MAX)
#  endif
#  ifndef F96_MI
#    define F96_MIN ((f96_t) LDBL_MIN)
#  endif
#endif
/* @} */

#define  UINT8_MIN ((uint8_t) 0)
#define UINT16_MIN ((uint16_t) 0)
#define UINT24_MIN ((uint32_t) 0)
#define UINT32_MIN ((uint32_t) 0)
#define UINT64_MIN ((uint64_t) 0)

#define INT24_MIN ((int32_t) -INT24_MAX - 1)
#define UINT24_MAX ((uint32_t) (1<<24)-1)
#define INT24_MAX ((int32_t) (1<<23)-1)

#if defined(AMIGA)
typedef u_int8_t  uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int64_t uint64_t;
typedef s_int8_t  sint8_t;
typedef s_int16_t sint16_t;
typedef s_int32_t sint32_t;
typedef s_int64_t sint64_t;
#endif
#if defined(__linux__)
typedef unsigned char uchar;
typedef long double ldouble;
#endif
#ifdef __APPLE_CC__
typedef unsigned char      uchar;
typedef unsigned long int  ulong;
typedef unsigned short int ushort;
typedef unsigned int       uint;
typedef unsigned long long ullong;
typedef signed char      schar;
typedef signed short     sshort;
typedef signed int       sint;
typedef signed long      slong;
typedef signed long long sllong;
typedef signed long long llong;
#endif
/* Note: Moved these two at the end to avoid strange problem UINT8,16,32,64
   _MAX not being defined. */
#if defined(HAVE_STDINT_H)
#  include <stdint.h>
#endif
#if defined(HAVE_INTTYPES_H)
#  include <inttypes.h>
#endif

#ifdef __cplusplus
}
#endif
