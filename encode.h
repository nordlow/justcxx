/*! \file encode.h
 * \brief Encode builtin C Types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Encoding of unsigned C types to char buffer (\c cbuf).
 */

#pragma once

#include <sys/types.h> /* Defines in_addr_t, needed in <netinet/in.h> */
#include <netinet/in.h>
#include <string.h>

#ifndef VXWORKS
#include <inttypes.h>
#endif

#include "bitwise.h"
#include "byterev.h"
#include "utils.h"
#include "string_x.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * \name Encoding of unsigned C types to char buffer (\p cbuf).
 */

/* @{ */

static inline int benc_u8(unsigned char *cbuf, const uint8_t *a) {
  cbuf[0] = *a;
  return sizeof(*a);
}

static inline int benc_s8(unsigned char *cbuf, const int8_t *a) {
  cbuf[0] = *a;
  return sizeof(*a);
}

/* ---------------------------- Group Separator ---------------------------- */

static inline int benc_u16be(unsigned char *cbuf, const uint16_t *a)
{
  uint16_t y = hton16(*a);	/* a to big-endian */
  memcpy2(cbuf, &y);
  return sizeof(*a);
}

static inline int benc_u16le(unsigned char *cbuf, const uint16_t *a)
{
  uint16_t y = hton16(*a);	/* a to big-endian */
  y = bswap_16(y);              /* a to little-endian */
  memcpy2(cbuf, &y);
  return sizeof(*a);
}

static inline int benc_s16be(unsigned char *cbuf, const int16_t *a)
{
  return benc_u16be(cbuf, (const uint16_t*) a);
}

static inline int benc_s16le(unsigned char *cbuf, const int16_t *a)
{
  return benc_u16le(cbuf, (const uint16_t*) a);
}

/* ---------------------------- Group Separator ---------------------------- */

static inline int benc_u32be(unsigned char *cbuf, const uint32_t *a)
{
  uint32_t y = hton32(*a);	/* a to big-endian */
  memcpy4(cbuf, &y);
  return sizeof(*a);
}

static inline int benc_u32le(unsigned char *cbuf, const uint32_t *a)
{
  uint32_t y = hton32(*a);	/* a to big-endian */
  y = bswap_32(y);		 /* a to little-endian */
  memcpy4(cbuf, &y);
  return sizeof(*a);
}

static inline int benc_s32be(unsigned char *cbuf, const int32_t *a)
{
  return benc_u32be(cbuf, (const uint32_t*) a);
}

static inline int benc_s32le(unsigned char *cbuf, const int32_t *a)
{
  return benc_u32le(cbuf, (const uint32_t*) a);
}

/* ---------------------------- Group Separator ---------------------------- */

static inline int benc_u64be(unsigned char *cbuf, const uint64_t *a)
{
  uint64_t y = hton64(*a);	/* a to big-endian */
  memcpy8(cbuf, &y);
  return sizeof(*a);
}

static inline int benc_u64le(unsigned char *cbuf, const uint64_t *a)
{
  uint64_t y = hton64(*a);	/* a to big-endian */
  y = bswap_64(y);		 /* a to little-endian */
  memcpy8(cbuf, &y);
  return sizeof(*a);
}

static inline int benc_s64be(unsigned char *cbuf, const int64_t *a)
{
  return benc_u64be(cbuf, (const uint64_t*) a);
}

static inline int benc_s64le(unsigned char *cbuf, const int64_t *a)
{
  return benc_u64le(cbuf, (const uint64_t*) a);
}

/* ---------------------------- Group Separator ---------------------------- */

static inline int benc_f32be(unsigned char *cbuf, const float *a)
{
  uint32_t l; memcpy4(&l, a);
  return benc_u32be(cbuf, &l);
}

static inline int benc_f32le(unsigned char *cbuf, const float *a)
{
  uint32_t l; memcpy4(&l, a);
  return benc_u32le(cbuf, &l);
}

/* ---------------------------- Group Separator ---------------------------- */

static inline int benc_f64be(unsigned char *cbuf, const double *a)
{
  uint64_t l; memcpy8(&l, a);
  return benc_u64be(cbuf, &l);
}

static inline int benc_f64le(unsigned char *cbuf, const double *a)
{
  uint64_t l; memcpy8(&l, a);
  return benc_u64le(cbuf, &l);
}

/* ---------------------------- Group Separator ---------------------------- */

static inline int
benc_sizebe(unsigned char * cbuf, const size_t * a)
{
  uint64_t x_U64 = *a;      /* NOTE: Extende to 64-bit for consistency on all architectures */
  return benc_u64be(cbuf, &x_U64);
}

static inline int
benc_sizele(unsigned char * cbuf, const size_t * a)
{
  uint64_t x_U64 = *a;      /* NOTE: Extende to 64-bit for consistency on all architectures */
  return benc_u64le(cbuf, &x_U64);
}

/* ---------------------------- Group Separator ---------------------------- */

/* @} */

static inline int
benc_cstr_lengthU8(unsigned char *cbuf, const char *str)
{
  uint8_t len = strlen(str);
  cbuf += benc_u8(cbuf, &len);
  memcpy(cbuf, str, len);
  cbuf += len;
  return (sizeof(len)+len);
}

static inline int
benc_cstr_lengthU16be(unsigned char *cbuf, const char *str)
{
  uint16_t len = strlen(str);
  cbuf += benc_u16be(cbuf, &len);
  memcpy(cbuf, str, len);
  cbuf += len;
  return (sizeof(len)+len);
}

static inline int
benc_cstr_lengthU32be(unsigned char *cbuf, const char *str)
{
  uint32_t len = strlen(str);
  cbuf += benc_u32be(cbuf, &len);
  memcpy(cbuf, str, len);
  cbuf += len;
  return (sizeof(len)+len);
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
