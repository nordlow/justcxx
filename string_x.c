#include "string_x.h"
#include "stdio_x.h"
#include <ctype.h>

/* ========================================================================= */

void *
memdrop(void * b, size_t bn, size_t bi, size_t an)
{
#ifndef NDEBUG
  if (bi > bn) { PERR("bi > bn\n"); return b; }
  if (bi + an > bn) { PERR("bi+an > bn\n"); return b; }
#endif
  memmove((char*)b + bi, (char*)b + bi + an, bn - bi - an); /* "left-push" 3:nd part of b */
  b = realloc(b, bn - an);
  return b;
}

void *
memins(void * b, size_t bn, size_t bi, const void * a, size_t an)
{
#ifndef NDEBUG
  if (bi > bn) { PERR("bi > bn\n"); return b; }
#endif
  b = realloc(b, an + bn);	/* preserves 1:st part of b */
  memmove((char*)b + bi + an, (char*)b + bi, bn - bi); /* "right-push" 2:nd part of b */
  memcpy((char*)b + bi, a, an);	/* insert a into b at bi */
  return b;
}

void
strtolower(char *a)
{
  while (*a) {
    *a = tolower(*a);
    a++;
  }
}

void
strupper(char *a)
{
  while (*a) {
    *a = toupper(*a);
    a++;
  }
}

char *
pnw_strcasestr(const char *haystack, const char *needle)
{
  char * haystack2, * needle2, * hit;

  haystack2 = strdup(haystack);
  strtolower(haystack2);

  needle2 = strdup(needle);
  strtolower(needle2);

  hit = strstr(haystack2, needle2);
  if (hit) {			/* if any needle2 was found */
    /* calculate address to hit in originial haystack */
    hit = hit - haystack2 + (char*)haystack;
  }

  free(haystack2);
  free(needle2);

  return hit;
}

void
str_sieve_out_ch(char *s1, const char *s2, char ch)
{
  while (*s2) {
    if (*s2 != ch) {
      *s1++ = *s2++;
    } else {
      s2++;
    }
  }
  *s1 = *s2;			/* Copy null. */
}

char *
strowrite(char * dst, const char * src)
{
  if (dst != src) {
    if (dst) {
      free(dst);
    }
    dst = strdup(src);
  }
  return dst;
}

char *
strnapp(char * dst, size_t * dstlen, const char * src, size_t srcnum)
{
  dst = realloc(dst, *dstlen + srcnum + 1); /* enlarge allocated */
  memcpy(dst + *dstlen, src, srcnum); /* copy src bytes to dst  */
  *dstlen += srcnum;		      /* set new dst length */
  dst[*dstlen] = '\0';		      /* set terminator */
  return dst;			      /* return dst */
}

/* ========================================================================= */
