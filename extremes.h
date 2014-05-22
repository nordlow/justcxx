/*!
 * \file extremes.h
 * \brief Calculcations of Extremes: min, max and abs.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-09-25 15:58
 */

#pragma once
#include <math.h>
#include <sys/types.h>
#include <inttypes.h>
#include "cc_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Return absolute value.
 * We can also use int abs(int j) in stdlib.h
 */
#undef	ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))

/*! Return minimum of two.
 */
#undef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#undef MIN2
#define MIN2(a, b) (MIN(a, b))

/*! Return maximum of two.
 */
#undef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#undef MAX2
#define MAX2(a, b) (MAX(a, b))

/*! Return minimum of three.
 */
#undef MIN3
#define MIN3(a, b, c) (MIN2(a, MIN2(b, c)))

/*! Return maximum of three.
 */
#undef MAX3
#define MAX3(a, b, c) (MAX2(a, MAX2(b, c)))

/*! Return minimum of four.
 */
#undef MIN4
#define MIN4(a, b, c, d) (MIN2(a, MIN3(b, c, d)))

/*! Return maximum of four.
 */
#undef MAX4
#define MAX4(a, b, c, d) (MAX2(a, MAX3(b, c, d)))

/*! Return minimum of five.
 */
#undef MIN5
#define MIN5(a, b, c, d, e) (MIN2(a, MIN4(b, c, d, e)))

/*! Return maximum of five.
 */
#undef MAX5
#define MAX5(a, b, c, d, e) (MAX2(a, MAX4(b, c, d, e)))

/*! Return minimum of six.
 */
#undef MIN6
#define MIN6(a, b, c, d, e, f) (MIN2(a, MIN5(b, c, d, e, f)))

/*! Return maximum of six.
 */
#undef MAX6
#define MAX6(a, b, c, d, e, f) (MAX2(a, MAX5(b, c, d, e, f)))

/*! Return minimum of seven.
 */
#undef MIN7
#define MIN7(a, b, c, d, e, f, g) (MIN2(a, MIN6(b, c, d, e, f, g)))

/*! Return maximum of seven.
 */
#undef MAX7
#define MAX7(a, b, c, d, e, f, g) (MAX2(a, MAX6(b, c, d, e, f, g)))

static inline spure int int_min2(int a, int b) { return a < b ? a : b; }
static inline spure unsigned int uint_min2(unsigned int a, unsigned int b) { return a < b ? a : b; }
static inline spure size_t size_min2(size_t a, size_t b) { return a < b ? a : b; }
static inline spure uint64_t uint64_min2(uint64_t a, uint64_t b) { return a < b ? a : b; }
static inline spure off_t off_min2(off_t a, off_t b) { return a < b ? a : b; }

static inline spure int int_max2(int a, int b) { return a > b ? a : b; }
static inline spure unsigned int uint_max2(unsigned int a, unsigned int b) { return a > b ? a : b; }
static inline spure size_t size_max2(size_t a, size_t b) { return a > b ? a : b; }
static inline spure uint64_t uint64_max2(uint64_t a, uint64_t b) { return a > b ? a : b; }
static inline spure off_t off_max2(off_t a, off_t b) { return a > b ? a : b; }

static inline float float_min(float a, float b) { return MIN2(a, b); }
static inline float float_min2(float a, float b) { return MIN2(a, b); }
static inline float float_min3(float a, float b, float c) { return MIN3(a, b, c); }
static inline float float_min4(float a, float b, float c, float d) { return MIN4(a, b, c, d); }
static inline float float_min5(float a, float b, float c, float d, float e) { return MIN5(a, b, c, d, e); }
static inline float float_min6(float a, float b, float c, float d, float e, float f) { return MIN6(a, b, c, d, e, f); }
static inline float float_min7(float a, float b, float c, float d, float e, float f, float g) { return MIN7(a, b, c, d, e, f, g); }
static inline float float_max(float a, float b) { return MAX2(a, b); }
static inline float float_max2(float a, float b) { return MAX2(a, b); }
static inline float float_max3(float a, float b, float c) { return MAX3(a, b, c); }
static inline float float_max4(float a, float b, float c, float d) { return MAX4(a, b, c, d); }
static inline float float_max5(float a, float b, float c, float d, float e) { return MAX5(a, b, c, d, e); }
static inline float float_max6(float a, float b, float c, float d, float e, float f) { return MAX6(a, b, c, d, e, f); }
static inline float float_max7(float a, float b, float c, float d, float e, float f, float g) { return MAX7(a, b, c, d, e, f, g); }

static inline double double_min(const double a, const double b) { return MIN2(a, b); }
static inline double double_min2(const double a, const double b) { return MIN2(a, b); }
static inline double double_max2(const double a, const double b) { return MAX2(a, b); }

static inline spure float float_abs(float a) { return fabsf(a); }
static inline spure double double_abs(const double a) { return fabs(a); }

#ifdef __cplusplus
}
#endif
