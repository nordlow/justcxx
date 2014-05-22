/*!
 * \file math_x.h
 * \brief Mathematics (math.h) Extensions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://www.boost.org/doc/libs/1_49_0/libs/math/doc/sf_and_dist/html/math_toolkit/toolkit/internals1/constants.html
 * \todo Copy Constants to math_x.hpp using \c ratio_pi.
 */

#pragma once
#include <math.h>
#include <stdlib.h>
#include "sortn.h"
#include "cc_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \name Digit Separators in C++.
 * We can write the example number 1234567 as below int j = NUM( 1,234,567 );
 * \see http://jrb-programming.blogspot.se/2013/02/digit-separators-in-c.html
 */
#define NUM(...) NUM_HELPER(__VA_ARGS__,,,,,,,,,)
#define NUM_HELPER(a,b,c,d,e,f,g,...) a##b##c##d##e##f##g

/*! \name Pi Fractions.
 * \todo Replace with C++11 constexpr for example:
 */
/* @{ */
#ifndef M_PI
#define M_PI     (3.1415926535897932) /**< Pi. */
#endif
#ifndef M_2PI
#define M_2PI    (6.28318530717959) /**< 2 Pi. */
#endif
#ifndef M_PI_2
#define M_PI_2   (1.57079632679490) /**< Pi over 2. */
#endif
#ifndef M_PI_4
#define M_PI_4   (0.785398163397448) /**< Pi over 4. */
#endif
#ifndef M_3PI_2
#define M_3PI_2  (4.71238898038469) /**< 3 Pi over 2. */
#endif
#ifndef M_2PI_3
#define M_2PI_3  (2.09439510239320) /**< 2 Pi over 3. */
#endif
#ifndef M_2PI_5
#define M_2PI_5  (1.25663706143592) /**< 2 Pi over 5. */
#endif
#ifndef M_2PI_24
#define M_2PI_24 (0.261799387799149) /**< 2 Pi over 24. */
#endif
#ifndef M_2PI_60
#define M_2PI_60 (0.104719755119660) /**< 2 Pi over 60. */
#endif
#ifndef M_2PI_M
#define M_2PI_M  (6.28318530717959e-06) /**< 2 Pi over one million 1000000. */
#endif
#ifndef M_2PI_G
#define M_2PI_G  (6.28318530717959e-09) /**< 2 Pi over one billion 1000000000. */
#endif
#ifndef M_4PI
#define M_4PI    (12.5663706143592) /**< 4 Pi. */
#endif
#ifndef M_4PI_3
#define M_4PI_3  (4.18879020478639) /**< 4 Pi over 3. */
#endif
#ifndef M_1_PI
#define M_1_PI   (0.318309886183791) /**< 1 over Pi. */
#endif
#ifndef M_1_2PI
#define M_1_2PI  (0.159154943091895) /**< 1 over 2 Pi. */
#endif
/* @} */

/*! \name E Fractions. */
/* @{ */
#ifndef M_E
#define M_E      (2.71828182845905)
#endif
/* @} */

/*! \name Quadrant Offset. */
/* @{ */
#define M_Q0 (0)
#define M_Q1 (M_PI_2)
#define M_Q2 (M_PI)
#define M_Q3 (M_3PI_2)
/* @} */

/*! \name Rational Fractions. */
/* @{ */
#define M_1_3 (0.33333333333333) /**< One Third. */
#define M_2_3 (0.66666666666667) /**< Two Thirds. */
#define M_1_7 (0.14285714285714) /**< One Seventh. */
/* @} */

/*! \name Square roots. */
/* @{ */
/*  #define M_SQRT2  (1.41421356237310) */
#define M_SQRT3  (1.73205080756888)
#define M_SQRT5  (2.23606797749979)
#define M_SQRT7  (2.64575131106459)
#define M_SQRT10 (3.16227766016838)
#define M_SQRT11 (3.31662479035540)
#define M_SQRT13 (3.60555127546399)
#define M_SQRT17 (4.12310562561766)
/* @} */

/*! \name Square root fractions. */
/* @{ */
#define M_SQRT2_2  (0.707106781186548)
#define M_SQRT3_2  (0.866025403784439)
/* @} */

/*! \name The Golden Ratio and Number. */
/* @{ */
/*! The Golden Number. The inverse of The Golden Ration (M_GOLDRAT)
 * Defined as (+ 1.0 + sqrt(5.0)) / 2.0.
 */
#define M_GOLDNUM (1.61803398874989)
/*! The Golden Ratio. The inverse of The Golden Number (M_GOLDNUM)
 * Defined as (- 1.0 + sqrt(5.0)) / 2.0.
 */
#define M_GOLDRAT (0.618033988749895)
/* @{ */

/*! \name Angle Conversions. */
/* @{ */
/*! Degrees to Radians Conversion Factor. */
#define M_DEG2RAD (0.0174532925199433)
/*! Radians to Degrees Conversion Factor. */
#define M_RAD2DEG (57.2957795130823)
/*! Convert Azimuth Angle \p a in Degrees to Radians for use in libc functions. */
static inline float float_azim2rad(float a) { return (90.0f - a) * (float)M_DEG2RAD; }
/*! Convert Normal Angle \p a in Degrees to Radians for use in libc functions. */
static inline float float_deg2rad(float a) { return a * (float)M_DEG2RAD; }
/*! Convert Normal Angle \p a in Degrees to Radians for use in libc functions. */
static inline float float_rad2deg(float a) { return a * (float)M_RAD2DEG; }
/* @} */

#define M_KILOBYTE (1   << 10)	/**< KiloByte */
#define M_MEGABYTE (1   << 20)	/**< MegaByte */
#define M_GIGABYTE (1   << 30)	/**< GigaByte */
#define M_TERABYTE (1LL << 40)	/**< TeraByte */
#define M_PETABYTE (1LL << 50)	/**< PetaByte, BubbaByte */
#define M_EXABYTE  (1LL << 60)	/**< ExaByte */

#define M_KB (1   << 10)	/**< KiloByte */
#define M_MB (1   << 20)	/**< MegaByte */
#define M_GB (1   << 30)	/**< GigaByte */
#define M_TB (1LL << 40)	/**< TeraByte */
#define M_PB (1LL << 50)	/**< PetaByte, BubbaByte */
#define M_EB (1LL << 60)	/**< ExaByte */

#define M_KILO (1000)                  /**< Kilo */
#define M_MEGA (1000000)               /**< Mega */
#define M_GIGA (1000000000)            /**< Giga */
#define M_TERA (1000000000000LL)       /**< Tera */
#define M_PETA (1000000000000000LL)    /**< Peta */
#define M_EXA  (1000000000000000000LL) /**< Exa */

#define MA_MILLI(a) (a / 1000)  /**< Milli times \p a */
#define MA_MICRO(a) (a / 1000000) /**< Micro times \p a */
#define MA_NANO(a)  (a / 1000000000) /**< Nano times \p a */
#define MA_PICO(a)  (a / 1000000000000LL) /**< Pico times \p a */

#define MA_CENTI(a) (a * 100)     /**< Centi times \p a */
#define MA_KILO(a)  (a * 1000)    /**< Kilo times \p a */
#define MA_MEGA(a)  (a * 1000000) /**< Mega times \p a */
#define MA_GIGA(a)  (a * 1000000000) /**< Giga times \p a */
#define MA_TERA(a)  (a * 1000000000000LL) /**< Tera times \p a */
#define MA_PETA(a)  (a * 1000000000000000LL) /**< Peta times \p a */
#define MA_EXA(a)   (a * 1000000000000000000LL) /**< Exa times \p a */

/*! Return square of \p a. */
#undef	SQR
#define SQR(a) ((a) * (a))

/*! Return cube of \p a. */
#undef  CUBE
#define CUBE(a) ((a) * (a) * (a))

// TODO: C++ify
#define POLYNOM1(x, c)  ((c)[0] * (x) + (c)[1])
#define POLYNOM2(x, c)  (POLYNOM1((x), (c)) * (x) + (c)[2])
#define POLYNOM3(x, c)  (POLYNOM2((x), (c)) * (x) + (c)[3])
#define POLYNOM4(x, c)  (POLYNOM3((x), (c)) * (x) + (c)[4])
#define POLYNOM5(x, c)  (POLYNOM4((x), (c)) * (x) + (c)[5])
#define POLYNOM6(x, c)  (POLYNOM5((x), (c)) * (x) + (c)[6])
#define POLYNOM7(x, c)  (POLYNOM6((x), (c)) * (x) + (c)[7])
#define POLYNOM8(x, c)  (POLYNOM7((x), (c)) * (x) + (c)[8])
#define POLYNOM9(x, c)  (POLYNOM8((x), (c)) * (x) + (c)[9])

/*! \name Hypotenuse - Euclidean distance.
 * Calculate the Euclidean Distance from origin to vector which coordinates are
 * specified by a, b, ....
 */
/* @{ */
#undef HYPOT2D
#define HYPOT2D(a,b) (sqrt(SQR(a)+SQR(b)))
#undef HYPOT3D
#define HYPOT3D(a,b,c) (sqrt(SQR(a)+SQR(b)+SQR(c)))
#undef HYPOT4D
#define HYPOT4D(a,b,c,d) (sqrt(SQR(a)+SQR(b)+SQR(c)+SQR(d)))

static inline spure float float_hypot2d(float a, float b) {
#ifdef hypotf			/* when possible */
  return hypotf(a, b);		/* use C library version */
#else
  return (float)HYPOT2D(a, b);
#endif
}
static inline spure double double_hypot2d(double a, double b) {
#ifdef hypot			/* when possible */
  return hypotf(a, b);		/* use C library version */
#else
  return HYPOT2D(a, b);
#endif
}
static inline spure float float_hypot3d(float a, float b, float c) { return (float)HYPOT3D(a, b, c); }
static inline spure double double_hypot3d(double a, double b, double c) { return HYPOT3D(a, b, c); }

/* @} */

#ifdef __cplusplus
}
#endif
