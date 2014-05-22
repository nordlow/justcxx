/*! \file string_x.h
 * \brief Extensions to C String Manipulation Functions (string.h).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <stdlib.h>
#include <string.h>
#include "stdio_x.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Drop \p an bytes from buffer \p b of length \p bn at index \p bi.
 * \return the resulting buffer
 */
void * memdrop(void * b, size_t bn, size_t bi, size_t an);

/*!
 * Insert buffer \p a of length \p an into buffer \p b of length \p bn
 * at index \p bi.
 * \return the resulting buffer
 */
void * memins(void * b, size_t bn, size_t bi, const void * a, size_t an);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Insert C-string \p a into C-string \p b at index \p bi.
 * \return the resulting C-string
 */
int strins(char * b, size_t bi, const char * a);

/*!
 * Insert character \p a into C-string \p b at index \p bi.
 * \return the resulting C-string
 */
int strins1(char * b, size_t bi, char a);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Lowercase C-string \p a inline.
 */
void strtolower(char * a);

/*!
 * Uppercase C-string \p a inline.
 */
void strupper(char * a);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * My own ineffective \c strstr()-wrapped version of \c strcasestr().
 */
char *pnw_strcasestr(const char *haystack, const char *needle);

/*!
 * Filter out the elements in \p s2 NOT EQUAL to \p ch and put them in
 * \p s1.
 */
void str_sieve_out_ch(char *s1, const char *s2, char ch);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Overwrite source C-string \p src into destination C-string \p dst and return it.
 */
char * strowrite(char * dst, const char * src);

/*!
 * Append char \p ch do \p dst and return \p dst.
 * Also Increases *dstlen with \p srcnum.
 */
char * strnapp(char * dst, size_t * dstlen, const char * src, size_t srcnum);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Allocate and Create new C-string = \p a + \p b and return it.
 * \return the resulting path, that is later released with free()
 */
static inline
char * strnadd(const char * a, size_t an,
               const char * b, size_t bn)
{
  char * c = (char*)malloc(an + bn + 1);
  memcpy(c     , a, an);
  memcpy(c + an, b, bn);
  c[an + bn] = '\0';
  return c;
}

/*!
 * Allocate and Create new C-string = \p a + \p b and return it.
 * \return the resulting path, that is later released with free()
 */
static inline
char * stradd(const char * a, const char * b)
{
  return strnadd(a, strlen(a),
                 b, strlen(b));
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Return pointer to last element of \p str or NULL if \p str is NULL
 * or empty.
 */
static inline
char * strlast(char * str)
{
  if (str) {
    size_t len = strlen(str);
    if (len > 0) {
      return &str[len-1];
    } else {
      return NULL;
    }
  } else {
    return NULL;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Safe \c memcpy() that only performs copy if \p dest and \p src are
 * non-overlapping, otherwise it prints out an error message and stops
 * program.
 */
static inline void *guarded_memcpy(void *dest, const void *src, size_t n)
{
  char *d = (char*)dest, *s = (char*)src;
  if ((d <= s && d + n > s) ||
      (s <= d && s + n > d)) {
    PERR("Memory areas dest:%p:%zu and src:%p:%zu overlap!",
         dest, n, src, n);
    return NULL;
  } else {
    return memcpy(dest, src, n);
  }
}

/*! Only use safe \c memcpy() through \c guarded_memcpy() if we are
 * compiling with extra debug checks (\c DEBUG_CHECK_ALL).
 */
#ifndef NDEBUG
/* #ifndef memcpy */
/* #define memcpy(dest, src, n) guarded_memcpy(dest, src, n); */
/* #endif */
#endif

/* ---------------------------- Group Separator ---------------------------- */

static inline void memcpy1(void * dest, const void * src)
{
  ((char*)dest)[0] = ((char*)src)[0];
}

static inline void memcpy2(void * dest, const void * src)
{
  ((char*)dest)[0] = ((char*)src)[0];
  ((char*)dest)[1] = ((char*)src)[1];
}

static inline void memcpy4(void * dest, const void * src)
{
  ((char*)dest)[0] = ((char*)src)[0];
  ((char*)dest)[1] = ((char*)src)[1];
  ((char*)dest)[2] = ((char*)src)[2];
  ((char*)dest)[3] = ((char*)src)[3];
}

static inline void memcpy8(void * dest, const void * src)
{
  ((char*)dest)[0] = ((char*)src)[0];
  ((char*)dest)[1] = ((char*)src)[1];
  ((char*)dest)[2] = ((char*)src)[2];
  ((char*)dest)[3] = ((char*)src)[3];
  ((char*)dest)[4] = ((char*)src)[4];
  ((char*)dest)[5] = ((char*)src)[5];
  ((char*)dest)[6] = ((char*)src)[6];
  ((char*)dest)[7] = ((char*)src)[7];
}

/* ---------------------------- Group Separator ---------------------------- */

/* Inline copying for i386 and later. */
#if defined (__GNUC__) && defined (__i386__)
#define PNW_MEMCPY_INC(dst, src, n)                 \
  __asm__ volatile ("cld\n\trep\n\tmovsl" : :       \
                    "D" (dst), "S" (src), "c" (n))
#define PNW_MEMCPY_DEC(dst, src, n)                                     \
  __asm__ volatile ("std\n\trep\n\tmovsl" : :                           \
                    "D" ((dst) + (n) - 1), "S" ((src) + (n) - 1), "c" (n))
#define PNW_NORMALIZE_NOT_ZERO(p, n)                            \
  {                                                             \
    __asm__ __volatile ("std\n\trepe\n\tscasl" : "=c" (n) :     \
                        "a" (0), "D" ((p) + (n) - 1), "0" (n) : \
                        "cx", "di");                            \
    (n)++;                                                      \
  }
#else
#define PNW_MEMCPY_INC(dst, src, n) (memcpy(dst, src, n))
#define PNW_MEMCPY_DEC(dst, src, n) (memcpy(dst, src, n))
#endif

#define PNW_MEMCPY32(dst, src, n) PNW_MEMCPY_INC(dst, src, n)

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
