/*!
 * \file sign.h
 * \brief Calculations of different kinds of signs.
 */

#pragma once

/*!
 * Sign of \p a, as either -1, 0, or 1.
 *
 * Same as Java's BigInteger.signum().
 */
#define SGN(a) (((a) < 0) ? -1 : (a) > 0 ? 1 : 0)

/*!
 * Compare Sign of \p a and \p b, as either -1, 0, or 1.
 */
#define CMPSGN(a, b) (((a) < (b)) ? -1 : (a) > (b) ? 1 : 0)

/*!
 * Binary sign of \p a.
 */
#define BSGN(a) (((a) < 0) ? -1 : 0)

/* ========================================================================= */

static inline pure int charp_sgn(const void *a, const void *b)
{
  if        (*((char *) a) < *((char *) b))  { return -1;
  } else if (*((char *) a) == *((char *) b)) { return 0; }
  else                                         { return 1; }
}

static inline pure int shortp_sgn(const void *a, const void *b)
{
  if        (*((short *) a) < *((short *) b))  { return -1;
  } else if (*((short *) a) == *((short *) b)) { return 0; }
  else                                         { return 1; }
}

static inline pure int intp_sgn(const void *a, const void *b)
{
  if        (*((int *) a) < *((int *) b))  { return -1;
  } else if (*((int *) a) == *((int *) b)) { return 0; }
  else                                         { return 1; }
}

static inline pure int floatp_sgn(const void *a, const void *b)
{
  if        (*((float *) a) < *((float *) b))  { return -1;
  } else if (*((float *) a) == *((float *) b)) { return 0; }
  else                                         { return 1; }
}

static inline pure int doublep_sgn(const void *a, const void *b)
{
  if        (*((double *) a) < *((double *) b))  { return -1;
  } else if (*((double *) a) == *((double *) b)) { return 0; }
  else                                         { return 1; }
}

static inline pure int ldoublep_sgn(const void *a, const void *b)
{
  if        (*((long double *) a) < *((long double *) b))  { return -1;
  } else if (*((long double *) a) == *((long double *) b)) { return 0; }
  else                                         { return 1; }
}

/* ========================================================================= */

static inline pure char charp_sgn2(const void *a, const void *b)
{
  if (*((char *) a) < *((char *) b)) { return -1; }
  else                               { return 1; }
}

static inline pure short shortp_sgn2(const void *a, const void *b)
{
  if (*((short *) a) < *((short *) b)) { return -1; }
  else                                 { return 1; }
}

static inline pure int intp_sgn2(const void *a, const void *b)
{
  if (*((int *) a) < *((int *) b)) { return -1; }
  else                             { return 1; }
}

static inline pure int floatp_sgn2(const void *a, const void *b)
{
  if (*((float *) a) < *((float *) b)) { return -1; }
  else                                 { return 1; }
}

static inline pure int doublep_sgn2(const void *a, const void *b)
{
  if (*((double *) a) < *((double *) b)) { return -1; }
  else                                 { return 1; }
}

static inline pure int ldoublep_sgn2(const void *a, const void *b)
{
  if (*((long double *) a) < *((long double *) b)) { return -1; }
  else                                 { return 1; }
}

/* ========================================================================= */

static inline spure float float_sgn(float a) { return SGN(a); }
static inline spure double double_sgn(double a) { return SGN(a); }
