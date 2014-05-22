/*! \file decode.h
 * \brief Decode Builtin C Types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \name Decoding of unsigned C types from char buffer (buf).
 */

#pragma once
#include <sys/types.h> /* Defines in_addr_t, needed in <netinet/in.h> */
#include <netinet/in.h>
#include <string.h>
#ifndef VXWORKS
#include <inttypes.h>
#endif
#include "utils.h"
#include "endian_utils.h"
#include "bitwise.h"
#include "byterev.h"
#include "stdio_x.h"
#include "string_x.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \name Decoding of Signed/Unsigned C types from char buffer (buf).
 *
 * - \c be: \em Big-Endian
 * - \c le: \em Little-Endian
 * - \c ne: \em Native-Endian
 */

/* @{ */

static inline pure uint8_t bdec_u8(const unsigned char *buf) { return buf[0]; }
static inline pure  int8_t bdec_s8(const unsigned char *buf) { return buf[0]; }

static inline pure uint16_t bdec_u16be(const unsigned char *buf) { return uint16_cat2be(buf[0], buf[1]); }
static inline pure uint16_t bdec_u16le(const unsigned char *buf) { return uint16_cat2le(buf[0], buf[1]); }
static inline pure uint16_t bdec_s16be(const unsigned char *buf) { return uint16_cat2be(buf[0], buf[1]); }
static inline pure uint16_t bdec_s16le(const unsigned char *buf) { return uint16_cat2le(buf[0], buf[1]); }
static inline pure uint32_t bdec_u16ne(const unsigned char *buf)
{
  uint16_t a;
  ((uint8_t*)(&a))[0] = buf[0];
  ((uint8_t*)(&a))[1] = buf[1];
  return a;
}

static inline pure uint32_t bdec_u24be(const unsigned char *buf) { return uint32_cat3be(buf[0], buf[1], buf[2]); }
static inline pure uint32_t bdec_u24le(const unsigned char *buf) { return uint32_cat3le(buf[0], buf[1], buf[2]); }
static inline pure uint32_t bdec_s24be(const unsigned char *buf) { return uint32_cat3be(buf[0], buf[1], buf[2]); }
static inline pure uint32_t bdec_s24le(const unsigned char *buf) { return uint32_cat3le(buf[0], buf[1], buf[2]); }
static inline pure uint32_t bdec_u24ne(const unsigned char *buf)
{
  uint32_t a;
  ((uint8_t*)(&a))[0] = buf[0];
  ((uint8_t*)(&a))[1] = buf[1];
  ((uint8_t*)(&a))[2] = buf[2];
  ((uint8_t*)(&a))[3] = 0;
  return a;
}

static inline pure uint32_t bdec_u32be(const unsigned char *buf) { return uint32_cat4be(buf[0], buf[1], buf[2], buf[3]); }
static inline pure uint32_t bdec_u32le(const unsigned char *buf) { return uint32_cat4le(buf[0], buf[1], buf[2], buf[3]); }
static inline pure uint32_t bdec_s32be(const unsigned char *buf) { return uint32_cat4be(buf[0], buf[1], buf[2], buf[3]); }
static inline pure uint32_t bdec_s32le(const unsigned char *buf) { return uint32_cat4le(buf[0], buf[1], buf[2], buf[3]); }
static inline pure uint32_t bdec_u32ne(const unsigned char *buf)
{
  uint32_t a;
  ((uint8_t*)(&a))[0] = buf[0];
  ((uint8_t*)(&a))[1] = buf[1];
  ((uint8_t*)(&a))[2] = buf[2];
  ((uint8_t*)(&a))[3] = buf[3];
  return a;
}

static inline pure uint64_t bdec_u64be(const unsigned char *buf) { return uint64_cat8be(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]); }
static inline pure uint64_t bdec_u64le(const unsigned char *buf) { return uint64_cat8le(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]); }
static inline pure uint64_t bdec_s64be(const unsigned char *buf) { return uint64_cat8be(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]); }
static inline pure uint64_t bdec_s64le(const unsigned char *buf) { return uint64_cat8le(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]); }
static inline pure uint64_t bdec_u64ne(const unsigned char *buf)
{
  uint64_t a;
  ((uint8_t*)(&a))[0] = buf[0];
  ((uint8_t*)(&a))[1] = buf[1];
  ((uint8_t*)(&a))[2] = buf[2];
  ((uint8_t*)(&a))[3] = buf[3];
  ((uint8_t*)(&a))[4] = buf[4];
  ((uint8_t*)(&a))[5] = buf[5];
  ((uint8_t*)(&a))[6] = buf[6];
  ((uint8_t*)(&a))[7] = buf[7];
  return a;
}

/* ---------------------------- Group Separator ---------------------------- */

static inline pure float bdec_f32be(const unsigned char *buf)
{
  float f;
  uint32_t a;
#ifndef NDEBUG
  if (sizeof(f) != sizeof(a)) { PERR("sizeof(f) != sizeof(a)\n"); }
#endif
  a = bdec_u32be(buf);
  memcpy4(&f, &a);
  return f;
}

static inline pure float bdec_f32le(const unsigned char *buf)
{
  float f;
  uint32_t a;
#ifndef NDEBUG
  if (sizeof(f) != sizeof(a)) { PERR("sizeof(f) != sizeof(a)\n"); }
#endif
  a = bdec_u32be(buf);
  memcpy4(&f, &a);
  return f;
}

/* ---------------------------- Group Separator ---------------------------- */

static inline pure double bdec_f64be(const unsigned char *buf)
{
  double f;
  uint64_t a;
#ifndef NDEBUG
  if (sizeof(f) != sizeof(a)) { PERR("sizeof(f) != sizeof(a)\n"); }
#endif
  a = bdec_u64be(buf);
  memcpy8(&f, &a);
  return f;
}

static inline pure double bdec_f64le(const unsigned char *buf)
{
  double f;
  uint64_t a;
#ifndef NDEBUG
  if (sizeof(f) != sizeof(a)) { PERR("sizeof(f) != sizeof(a)\n"); }
#endif
  a = bdec_u64le(buf);
  memcpy8(&f, &a);
  return f;
}

/* ---------------------------- Group Separator ---------------------------- */

static inline pure size_t bdec_sizebe(const unsigned char *buf)
{
  uint64_t a64 = bdec_u64be(buf);
  if (sizeof(size_t) != sizeof(uint64_t) &&
      a64 >= UINT32_MAX) {
    PWARN("Truncating decoded size from %zd to %zd\n", (size_t)a64, (size_t)UINT32_MAX);
  }
  return a64;
}
static inline pure size_t bdec_sizele(const unsigned char *buf)
{
  uint64_t a64 = bdec_u64le(buf);
  if (sizeof(size_t) != sizeof(uint64_t) &&
      a64 >= UINT32_MAX) {
    PWARN("Truncating decoded size from %zd to %zd\n", (size_t)a64, (size_t)UINT32_MAX);
  }
  return a64;
}

/* ---------------------------- Group Separator ---------------------------- */

/* @} */

static inline char *
alloc_cbuf_dec_cstr_lengthU32be(unsigned char *buf)
{
  uint32_t len = bdec_u32be(buf);

  buf += sizeof(len);
  char * str = (char *) malloc(len) + 1;

  strncpy(str, (const char*)buf, len);
  buf += len;
  str[len] = '\0';

  return str;
}

#ifdef __cplusplus
}
#endif
