/*! \file clamp.h
 * \brief Clamping to \em Range.
 */

#pragma once
#include "cc_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Make \p a to be \em smaller than or equal to the \em higher bound/limit (floor) \p h. */
#define HIGH_CLAMP(a, h) (((a) > (h)) ? (h) : (a))
/*! Make \p a to be \em larger than or equal to the \em lower bound/limit (floor) \p l. */
#define LOW_CLAMP(a, l) (((a) < (l)) ? (l) : (a))
/*! Clamp \p a inside the limits set by low \p l and high \p h. */
#define CLAMP(l, a, h) (((a) < (l)) ? (l) : (a) > (h) ? (h) : (a))

/*! Ensures that \p a is between the limits set by low (\p l) and high (\p h). */
static inline spure int int_clamp(int l, int a, int h)
{
  return (((a) < (l)) ? (l) : (a) > (h) ? (h) : (a));
}
/*! Ensures that \p a is between the limits set by low (\p l) and high (\p h). */
static inline spure uint uint_clamp(uint l, uint a, uint h)
{
  return (((a) < (l)) ? (l) : (a) > (h) ? (h) : (a));
}
/*! Ensures that \p a is between the limits set by low (\p l) and high (\p h). */
static inline spure float float_clamp(float l, float a, float h)
{
  return (((a) < (l)) ? (l) : (a) > (h) ? (h) : (a));
}
/*! Ensures that \p a is between the limits set by low (\p l) and high (\p h). */
static inline spure double double_clamp(double l, double a, double h)
{
  return (((a) < (l)) ? (l) : (a) > (h) ? (h) : (a));
}

/*! Ensures that \p a is cyclically between the limits set by low (\p l) and high (\p h). */
static inline spure uint uint_cyclic_clamp(uint l, uint a, uint h)
{
  uint m = h-l+1;		/* modulo */
  /* PTEST("\n"); */
  if (a < l) {
    /* PTOWRITE("\n"); */
  } else if (a > h) {
    a = l + (a - (h+1)) % m;
  }
  return a;
}

#ifdef __cplusplus
}
#endif
