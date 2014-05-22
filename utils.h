/*! \file utils.h
 * \brief Various utilities for C-programming.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#ifdef HAVE_WCHAR_H
#include <wchar.h>
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <malloc.h>
#include <sys/types.h> /* Defines in_addr_t, needed in <netinet/in.h> */
#include <netinet/in.h>
#include "pnw_types.h"
#include "math_x.h"
#include "cc_features.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAXPATHDEPTH (128)

/*! \name Type Aliases.
 *
 * Naming Conventions:
 *
 * - Fixed point precision:
 *   u8 (Unsigned), s8 (Signed), b8 (unsigned or signed), ..., int, uint
 * - Floating point precision:                               f32 (IEEE-754)
 * - Floating point precision:                               f64 (IEEE-754)
 * - Multi (arbitrary) Precision fixed point integer (Z):    mpz
 * - Multi (arbitrary) Precision fixed point rational (Q):   mpq
 * - Multi (arbitrary) Precision Floating point number:      mpf
 *
 * - Arrays are specified by the suffix "a" such in u16v_copy.
 *
 * - Type names should be as abstract and general as possible, in contrast to
 *   variable names that should be as declarative and clear as possible.
 *
 * Example:
 * typedef struct { s16 a0, a1, a2, a3; } s16v_t;
 * s16v_t screen_rectangle;
 */
/* @{ */
typedef uint8_t  b8_t;
typedef uint16_t b16_t;
typedef uint32_t b32_t;
typedef uint64_t b64_t;
typedef float    f32_t;
typedef double   f64_t;
/* @} */

/*! Macro to get the size of a static array. */
#define	ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))
/*! Alias for \c ARRAY_LENGTH. */
#define	ARRL(a) ARRAY_LENGTH(a)

/*! Zero Pointer Constant. */
#ifndef	NULL
#define	NULL ((void *) 0)
#endif

/*! \name Boolean Constants. */
/* @{ */
#ifndef	FALSE
#define	FALSE (0)
#endif
#ifndef	TRUE
#define	TRUE (!FALSE)
#endif
/* @} */

/*! Constant used in inline assembler optimized functions. */
extern const uint64_t g_uint64_ones;
/*
  #ifndef __GNUC__
  #define __asm__ asm
  #define inline inline
  #define volatile volatile
  #endif
*/

/*!
 * Duplicate Memory Area \p s of size \p n bytes it an allocated
 * memory area and return this allocated area.
 * \return a pointer to the duplicated string which is to be freed
 *         with free(3) after usage.
 */
static inline char *
memdup(const char *s, size_t n)
{
  char * t = (char*)malloc(n);
  memcpy(t, s, n);
  return t;
}

/*!
 * Allocate memory similar to \c realloc() except that when \p n is
 * zero then \p ptr will be \c free()ed (if defined) and the zero
 * pointer will be returned.
 *
 * \see http://en.wikipedia.org/wiki/Malloc#realloc
 */
static inline void* dynalloc(void *ptr, size_t n)
{
  if (n != 0) {
    return realloc(ptr, n);
  } else {
    if (ptr) {
      free(ptr);
      /* PNOTE("ptr %p freeed\n", ptr); */
    } else {
      /* PNOTE("ptr already 0\n"); */
    }
    return 0;
  }
}

/*!
 * Alias to \c memalign(boundary, n) to make it easier to find.
 */
static inline void* malloc_aligned(size_t boundary, size_t n) {
  return memalign(boundary, n); }
static inline void* calloc_aligned(size_t boundary, size_t n) {
  void * p = memalign(boundary, n);
  memset(p, 0, n);              /* zero it */
  return p;
}

#ifdef HAVE_CPU_3DNOW
/*! Fast Empty MMX State */
#define MMX_EMMS() __asm__ volatile("femms\n\t")
#elif HAVE_CPU_MMX
/*! Empty MMX State */
#define MMX_EMMS() __asm__ volatile("emms\n\t")
#endif

/*! Factorials */
double log10factorial(double a);
#define log10P(n,r) (log10factorial(n)-log10factorial((n)-(r)))
#define log10C(n,r) (log10P((n),(r))-log10factorial(r))

/*! C Control characters */
/* @{ */
#define NEW_LINE       ('\n')
#define HORIZONTAL_TAB ('\t')
#define VERTICAL_TAB   ('\v')
#define BACK_SPACE     ('\b')
#define CARRIGE_RETURN ('\r')
#define FORM_FEED      ('\f')
#define ALERT          ('\a')
#define BACK_SLASH     ('\\')
#define QUESTION_MARK  ('\?')
#define SINGLE_QUOTE   ('\'')
#define DOUBLE_QUOTE   ('"')
/* @} */

#ifndef NDEBUG
#define FREEZ(a) { free(a); a = 0; }
#else
#define FREEZ(a) { free(a); }
#endif

/*! Construct 16-bit (2-Byte) Repeat of the 8-bit value \p a. */
static inline uint16_t uint16_rep2(uint8_t a) { return (((uint16_t)a) | ((uint16_t)a << 8)); }
/*! Construct 24-bit (3-Byte) Repeat of the 8-bit value \p a. */
static inline uint8_t uint32_rep3(uint8_t a) { return (((uint8_t)a) | ((uint8_t)a << 8) | ((uint8_t)a << 16)); }
/*! Construct 32-bit (4-Byte) Repeat of the 8-bit value \p a. */
static inline uint8_t uint32_rep4(uint8_t a) { return (((uint8_t)a) | ((uint8_t)a << 8) | ((uint8_t)a << 16) | ((uint8_t)a << 24)); }
/*! Construct 64-bit (8-Byte) Repeat of the 8-bit value \p a. */
static inline uint64_t uint64_rep8(uint8_t a) { return (((uint64_t)a <<  0) | ((uint64_t)a <<  8) | ((uint64_t)a << 16) | ((uint64_t)a << 24) |
                                                        ((uint64_t)a << 32) | ((uint64_t)a << 40) | ((uint64_t)a << 48) | ((uint64_t)a << 56)); }

/*! \em Shuffle all \p n elements contained in the pointer-array \p a. */
void ptrarray_shuffle(void ** a, size_t n);

/*! Check for \em Oddity / \em Evenity.
 * Separate integer variants could be needed for
 * some dumb compilers to prevent from generating machine code that uses
 * integer divison.
 */
#define IS_ODD(a) ((a) % 2 == 1 )
#define IS_EVEN(a) ((a) % 2 == 0 )

/*!
 * Calculate the television-area using the ratios r0 and r1 and the diameter d.
 * Units in inches and square-inches.
 * - 28-inch (3:4):  TV_AREA(28, 3, 4)
 * - 32-inch (9:16): TV_AREA(32, 9, 16)
 */
#define TV_AREA(d, r0, r1) (std::sqr(d) * r0 * r1 / (std::sqr(r0) + std::sqr(r1)))

/*! Round a to nearest integer towards 0. */
#define INT_FLOOR(a) (((a) > 0) ? (int) (a) : -(int) -(a))
/*! Round a to nearest integer away from 0. */
#define INT_CEILING(a)                                                  \
  ((a) == (int) (a) ? (a) : (a) > 0 ? 1 + (int) (a) : -(1 + (int) -(a)))
/*! Round to nearest integer. */
#define INT_ROUND(a) (((a) > 0) ? (int) ((a) + 0.5) : -(int) (0.5 - (a)))
/*! Rint, round, nearbyint fail! Why!??. */
#define ROUND(a) (floor ((double) (a) + 0.5))
/*! Round to nearest integer. */
#define F_ROUND(a) (floor((double) (a) + 0.5))/*** Use rint instead?. */
/*! Round number a to d decimal points. */
#define F_NROUND(a, d) (floor((a) * pow(10.0, (d)) + 0.5) / pow(10.0, (d)))

/*! Return true if \p a and \p b have the same signs. */
#define SAME_SIGNS(a, b) ((((int) (a)) ^ ((int) (b))) >= 0 )

/*! Logically toggle the value of \p a.
 * If it is zero (FALSE) set it to TRUE.
 * If it is non-zero set it to FALSE.
 */
#define TOGGLE(a) { a = !a; }

/*! Close a file descriptor \p a and then zero it. */
#define CLOSEZ(a) { close(a); a = 0; }

/*! Non-ANSI compliant function declarations to get rid of warnings. */
int snprintf(char *str, size_t size, const char *format, ...);

/* char *strdup (const char *s); */

/*! Example that shows how \c printf() is implemented.
 * Use like: \c pnw_printf ("ss", "pelle", "rules");
 */
void pnw_print(char *fmt, ...);

float va_sum(size_t cnt, ...);

/*! \name Calculate the Minimum, Maximum and Absolute Value.
 *
 * The first three have relatively low clock counts and allow the CPU
 * to perform parallel work if there are any opportunities.  The last
 * two lead the CPU to speculatively perform wrong work that needs to
 * be undone (this is usually just a few clocks of addition overhead).
 * The major point of which is that the CPU waits roughly the length
 * of its pipeline before it can perform real work.
 *
 * One way to work around a situation such as 4/5 is that if there are
 * only two cases, you actually perform the work for both cases, and
 * use a predicate mask to select between the answers.  For example to
 * perform a max, min and abs functions on an x86 compiler:
 */
static inline spure int int_min_no_branching(int a, int b) {
  b = b-a;
  a += b & (b>>31);
  return a;
}

static inline spure int int_max_no_branching(int a, int b) {
  b = a-b;
  a -= b & (b>>31);
  return a;
}

static inline spure int int_abs_no_branching(int a) {
  return a - ((a+a) & (a>>31));
}

void intarray_add2_int(int *a, const int *b, int c, size_t n);
void intarray_sub2_int(int *a, const int *b, int c, size_t n);
void intarray_print(const int *a, size_t n);
void sizearray_print(const char *name, const size_t *a, size_t n);

/*! Binary search for key in \p a of length \p n.
 * \return index to hit in \p a if key was found or, if not found,
 *         minus one (-1)times the position were it should be inserted
 *         times.
 */
int intarray_bsearch(const int * a, size_t n, int key);

void farray_fprint(FILE * stream, const float *a, size_t n);
void farray_print(const float *a, size_t n);

/*! Scale the sequence \p a by the value \p b.
 * to keep absolute values after Direct+Inverse transform.
 */
void farray_scale(float * a, float b, size_t n);

/*! \name Copy Data. */
/* @{ */
void b16v_copy(uint16_t *a, const uint16_t *b, size_t n);
void b32v_copy(uint32_t *a, const uint32_t *b, size_t n);
void intarray_copy(int *a, const int *b, size_t n);
void farray_copy(float *a, const float *b, size_t n);
void darray_copy(double *a, const double *b, size_t n);
void fmatrix_copy(float **a, const float **b, size_t m, size_t n);
void dmatrix_copy(double **a, const double **b, size_t m, size_t n);
/* @} */

/*! \name Generate Ramps. */
/* @{ */
void intarray_ramp(int *a, size_t n);
void sizearray_ramp(size_t *a, size_t n);
void farray_ramp(float *a, size_t n, float min, float step);
void darray_ramp(double *a, size_t n, double min, double step);
void fmatrix_ramp(float **a, size_t m, size_t n,
                  float min, float istep, float jstep);
void dmatrix_ramp(double **a, size_t m, size_t n,
                  double min, double istep, double jstep);
/* @} */

/*! \name Generate Zeros. */
/* @{ */
void intarray_zeros(int *a, size_t n);
void fmatrix_zeros(float **a, size_t m, size_t n);
void dmatrix_zeros(double **a, size_t m, size_t n);
/* @} */

/*! \name Generate Ones. */
/* @{ */
void farray_ones(float *a, size_t n);
void darray_ones(double *a, size_t n);
void fmatrix_ones(float **a, size_t m, size_t n);
void dmatrix_ones(double **a, size_t m, size_t n);
/* @} */

/*! \name Generate Signals. */
/* @{ */
void farray_coswave(float *out, size_t n, float ampl, float freq, float phase);
void darray_coswave(double *out, size_t n, double ampl, double freq, double phase);
/* @} */

/*!* \name Generate Signals. */
/* @{ */
void farray_sinwave(float *out, size_t n, float ampl, float freq, float phase);
void darray_sinwave(double *out, size_t n, double ampl, double freq, double phase);
/* @} */

/*! \name Generate Signals. */
/* @{ */
void farray_flowerspiral(float *x, float *y, size_t n,
                         float ampl, float freq, float phase);
void darray_flowerspiral(double *x, double *y, size_t n,
                         double ampl, double freq, double phase);
/* @} */

void farray_add1(float *a, const float *b, size_t n);
void darray_add1(double *a, const double *b, size_t n);

void farray_sub1(float *a, const float *b, size_t n);
void darray_sub1(double *a, const double *b, size_t n);

void darray_mul1(double *a, const double *b, size_t n);

void darray_div1(double *a, const double *b, size_t n);

void darray_add2(double *a, const double *b, const double *c, size_t n);
void darray_sub2(double *a, const double *b, const double *c, size_t n);
void darray_mul2(double *a, const double *b, const double *c, size_t n);
void darray_div2(double *a, const double *b, const double *c, size_t n);
void darray_pow2(double *a, const double *b, const double *c, size_t n);

void darray_add2_dbl(double *a, const double *b, double c, size_t n);
void darray_sub2_dbl(double *a, const double *b, double c, size_t n);
void darray_mul2_dbl(double *a, const double *b, double c, size_t n);
void darray_div2_dbl(double *a, const double *b, double c, size_t n);

void darray_min2_dbl(double *a, const double * b, double c, size_t n);
void darray_max2_dbl(double *a, const double * b, double c, size_t n);

void darray_pow2_dbl(double *a, const double *b, double c, size_t n);
void darray_pow2_double_rev(double *a, const double *b, double c, size_t n);

void darray_neg(double *a, const double *b, size_t n);
void darray_abs(double *a, const double *b, size_t n);
void darray_inv(double *a, const double *b, size_t n);

double darray_sum(const double *a, size_t n);
double darray_prod(const double *a, size_t n);

double darray_min(const double *a, size_t n);
double darray_max(const double *a, size_t n);

void darray_cos(double *a, const double *b, size_t n);
void darray_sin(double *a, const double *b, size_t n);
void darray_tan(double *a, const double *b, size_t n);

void darray_acos(double *a, const double *b, size_t n);
void darray_asin(double *a, const double *b, size_t n);
void darray_atan(double *a, const double *b, size_t n);

void darray_eud2D(double *a, const double *b, const double *c, size_t n);

void darray_extremes(double *a, size_t n, double *min_ret, double *max_ret);

void darray_fprint(FILE * stream, const double *a, size_t n);
void ldarray_fprint(FILE * stream, const long double *a, size_t n);

void darray_print(const double *a, size_t n);
void ldarray_print(const long double *a, size_t n);

void darray_fprintComplex(FILE * stream, const double *re, const double *im, size_t n);
void darray_printComplex(const double *re, const double *im, size_t n);

int fmatrix_any(const float **a, size_t m, size_t n);
int dmatrix_any(const double **a, size_t m, size_t n);

int fmatrix_all(const float **a, size_t m, size_t n);
int dmatrix_all(const double **a, size_t m, size_t n);

void fmatrix_eye(float **a, size_t m, size_t n);
void dmatrix_eye(double **a, size_t m, size_t n);

void fmatrix_diag(float **a, float *b, size_t m, size_t n);
void dmatrix_diag(double **a, double *b, size_t m, size_t n);

void fmatrix_subcopy(float **a, int a_i0, int a_j0,
                     const float **b, int b_i0, int b_j0, size_t m, size_t n);

void dmatrix_subcopy(double **a, int a_i0, int a_j0,
                     const double **b, int b_i0, int b_j0, size_t m, size_t n);

void fmatrix_subreadcopy(float **a,
                         const float **b, int b_i0, int b_j0, size_t m, size_t n);

void dmatrix_subreadcopy(double **a,
                         const double **b, int b_i0, int b_j0, size_t m, size_t n);

void fmatrix_subwritecopy(float **a, int a_i0, int a_j0,
                          const float **b, size_t m, size_t n);

void dmatrix_subwritecopy(double **a, int a_i0, int a_j0,
                          const double **b, size_t m, size_t n);

void fmatrix_colcat(float **a, const float **b, const float **c,
                    size_t m, size_t bn, size_t cn);

void dmatrix_colcat(double **a, const double **b, const double **c,
                    size_t m, size_t bn, size_t cn);

void fmatrix_rowcat(float **a, const float **b, const float **c,
                    size_t bm, size_t cm, size_t n);

void dmatrix_rowcat(double **a, const double **b, const double **c,
                    size_t bm, size_t cm, size_t n);

void intmatrix_fprint(FILE * stream, const int **a, size_t m, size_t n);
void fmatrix_fprint(FILE * stream, const float **a, size_t m, size_t n);
void dmatrix_fprint(FILE * stream, const double **a, size_t m, size_t n);

void fmatrix_print(const float **a, size_t m, size_t n);
void dmatrix_print(const double **a, size_t m, size_t n);

void dmatrix_fprintComplex(FILE * stream,
                           const double **re, const double **im, size_t m, size_t n);

void dmatrix_printComplex(const double **re, const double **im, size_t m, size_t n);

/*! Binary Coded Decimal to Binary. */
static inline spure int bcd_to_bin(unsigned char c)
{
  return (c & 0x0f) + (c >> 4) * 10;
}

/*! Binary to Binary Coded Decimal. */
static inline spure unsigned char bin_to_bcd(int i)
{
  int x;
  x = i / 10;
  return (unsigned char)(x * 16 + i % 10);
}

/*! Separate all \em Non-Zero elements of \p a of length \p n to the left.
 *
 * Takes the integer array \p a containing \p n elements and moves all its non-zero
 * elements to the beginning of the array while at the same preserving their
 * order.
 *
 * It then returns the number of non-zero elements that was contained in the
 * array.
 *
 * Can be used in a server application to which multiple clients connect and
 * disconnect dynamically. It is typically called whenever the server shuts
 * down an existing connection.
 */
size_t intarray_sepNZs(int * a, size_t n);

/* @} */

/*! \name Host / Network (Big-Endian) Byte Order Conversions.
 * Use number of bits in naming conventions for more readable and consistent
 * code.
 */
/* @{ */
#define hton8(a)  (a)
#define hton16(a) (htons (a))
#define hton32(a) (htonl (a))
#define ntoh8(a)  (a)
#define ntoh16(a) (ntohs (a))
#define ntoh32(a) (ntohl (a))
static inline spure uint64_t hton64(uint64_t a)
{
  return (((uint64_t)(htonl((a) & 0xffffffff)) << 32) |
          htonl(((a) >> 32) & 0xffffffff ));
}
static inline spure uint64_t ntoh64(uint64_t a)
{
  return (((uint64_t)(ntohl((a) & 0xffffffff)) << 32) |
          ntohl(((a) >> 32) & 0xffffffff ));
}
/* @} */

/*! Kronecker Tensor Product. */
void dmatrix_kronecker(double **out, const double **in1, const double **in2,
                       int w_in1, int h_in1, int w_in2, int h_in2);

/*! Returns the clock speed of the system's CPU in MHz, as reported by
 * /proc/cpuinfo. On a multiprocessor machine, returns the speed of the first
 * CPU. On error returns 0.
 */
float wuur get_CPU_clock_speed(void);

#if 0
/*! Shift Right.
 * This is my ugly try at performing optimized float multiplication and
 * division of powers of two. However, such a function already exists in the
 * standard C library: ldexpf. Also see ldexp for double and ldexpl for long
 * double.
 */
static inline spure float float_shl(float a, uint shift) {
  int x = 0x1111;
  uint32_t *aip = (uint32_t*)&a;
  uint man = (((*(aip) >> 24) & 0x7f) - shift) << 24;
  *aip = (*aip & 0x80ffffff) | man;
  return a;
}

static inline void test_shl() {
  float x;
  uint s;
  printf("x and s: "); scanf("%f %d", &x, &s);
  float y = float_shl(x, s);
  printf("y: %f\n", y);
}
#endif

pure int file_is_binary(const char *path);

#ifdef __cplusplus
}
#endif
