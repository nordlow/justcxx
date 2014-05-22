#include "cmp.h"
#include "perm.h"
#include "sortn.h"
#include "qsort.h"
#include "qsort_macro.h"
#include "extremes.h"
#include "is_aligned.h"
#include "stdio_x.h"

#define QSORT_USE_SORTN (1)
#define QSORT_USE_RANGESWAP_OPT (1)

#if 0
void chararray_qsort_alt(char *a, size_t n) { QSORT(char, a, n, char_ptr_lt); }
void shortarray_qsort_alt(short *a, size_t n) { QSORT(short, a, n, short_ptr_lt); }
void intarray_qsort_alt(int *a, size_t n) { QSORT(int, a, n, int_ptr_lt); }
void uintarray_qsort_alt(uint *a, size_t n) { QSORT(uint, a, n, uint_ptr_lt); }
void farray_qsort_alt(float *a, size_t n) { QSORT(float, a, n, float_ptr_lt); }
void darray_qsort_alt(double *a, size_t n) { QSORT(double, a, n, double_ptr_lt); }
void ldarray_qsort_alt(ldouble *a, size_t n) { QSORT(ldouble, a, n, ldouble_ptr_lt); }
#endif

/* ---------------------------- Group Separator ---------------------------- */

/* Sort a arrays into ascending order. The sort algorithm is an
   optimised quicksort, as described in Jon L. Bentley and M. Douglas
   McIlroy's Engineering a Sort Function", Software-Practice and
   Experience, Vol. 23(11) P. 1249-1265 (November 1993). This algorithm
   gives n log (n) performance on many arrays that would take quadratic time
   with a standard quicksort. */

/* ---------------------------- Group Separator ---------------------------- */

/* Calculate Index of the Median of the three Indexed Values. */
static inline int floati_median3(const float *x, size_t a, size_t b, size_t c)
{
  return (x[a] < x[b] ?
	  (x[b] < x[c] ? b : x[a] < x[c] ? c : a) :
	  (x[b] > x[c] ? b : x[a] > x[c] ? c : a));
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void floati_swap(float *x, size_t a, size_t b)
{
  float t = x[a]; x[a] = x[b]; x[b] = t;
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void floati_swapi(float *x, size_t *idx, size_t a, size_t b)
{
  floati_swap(x, a, b);
  SWAP(idx[a], idx[b]);
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void floati_rangeswap(float *x, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { floati_swap(x, a, b); }
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void floati_rangeswapi(float *x, size_t *idx, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { floati_swapi(x, idx, a, b); }
}

static void
farray_qsort1(float *x, size_t off, size_t len)
{
  float v;

#if QSORT_USE_SORTN
  if (len <= SORTN_MAX) { SORTN(x+off, len); return; }
#endif

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = floati_median3(x, l, l + s, l + 2 * s);
      m = floati_median3(x, m - s, m, m + s);
      n = floati_median3(x, n - 2 * s, n - s, n);
    }
    m = floati_median3(x, l, m, n); /* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	floati_swap(x, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	floati_swap(x, c, d--);
      c--;
    }
    if (b > c)
      break;
    floati_swap(x, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  floati_rangeswap(x, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  floati_rangeswap(x, b, n - s, s);

  /* recursively sort non-partition-elements */
#if QSORT_USE_SORTN
  const int t = b - a;
  if (t <= SORTN_MAX) { SORTN(x+off, t); }
  else        { farray_qsort1(x, off, t); }

  const int u = d - c;
  const int off1 = n - u;
  if (u <= SORTN_MAX) { SORTN(x+off1, u); }
  else        { farray_qsort1(x, off1, u); }
#else
  if ((s = b - a) > 1)
    farray_qsort1(x, off, s);
  if ((s = d - c) > 1)
    farray_qsort1(x, n - s, s);
#endif
}

void
farray_qsort(float *a, size_t n)
{
  farray_qsort1(a, 0, n);
}

static void
farray_iqsort1(float *x, size_t *idx, size_t off, size_t len)
{
  float v;

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = floati_median3(x, l, l + s, l + 2 * s);
      m = floati_median3(x, m - s, m, m + s);
      n = floati_median3(x, n - 2 * s, n - s, n);
    }
    m = floati_median3(x, l, m, n);	/* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	floati_swapi(x, idx, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	floati_swapi(x, idx, c, d--);
      c--;
    }
    if (b > c)
      break;
    floati_swapi(x, idx, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  floati_rangeswapi(x, idx, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  floati_rangeswapi(x, idx, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1)
    farray_iqsort1(x, idx, off, s);
  if ((s = d - c) > 1)
    farray_iqsort1(x, idx, n - s, s);
}

void
farray_iqsort(float *a, size_t *idx, size_t n)
{
  farray_iqsort1(a, idx, 0, n);
}

bool
farray_is_sorted(float *a, size_t n)
{
  for (size_t j = 0; j < n - 1; j++) {
    if (a[j] > a[j + 1]) {
      return false;
    }
  }
  return true;
}

/* ---------------------------- Group Separator ---------------------------- */

/* Calculate Index of the Median of the three Indexed Values. */
static inline int doublei_median3(const double *x, size_t a, size_t b, size_t c)
{
  return (x[a] < x[b] ?
	  (x[b] < x[c] ? b : x[a] < x[c] ? c : a) :
	  (x[b] > x[c] ? b : x[a] > x[c] ? c : a));
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void doublei_swap(double *x, size_t a, size_t b)
{
  double t = x[a]; x[a] = x[b]; x[b] = t;
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void doublei_swapi(double *x, size_t *idx, size_t a, size_t b)
{
  doublei_swap(x, a, b);
  SWAP(idx[a], idx[b]);
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void doublei_rangeswap(double *x, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { doublei_swap(x, a, b); }
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void doublei_rangeswapi(double *x, size_t *idx, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { doublei_swapi(x, idx, a, b); }
}

static void
darray_qsort1(double *x, size_t off, size_t len)
{
  double v;

#if QSORT_USE_SORTN
  if (len <= SORTN_MAX) { SORTN(x+off, len); return; }
#endif

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = doublei_median3(x, l, l + s, l + 2 * s);
      m = doublei_median3(x, m - s, m, m + s);
      n = doublei_median3(x, n - 2 * s, n - s, n);
    }
    m = doublei_median3(x, l, m, n); /* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	doublei_swap(x, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	doublei_swap(x, c, d--);
      c--;
    }
    if (b > c)
      break;
    doublei_swap(x, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  doublei_rangeswap(x, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  doublei_rangeswap(x, b, n - s, s);

  /* recursively sort non-partition-elements */
#if QSORT_USE_SORTN
  const int t = b - a;
  if (t <= SORTN_MAX) { SORTN(x+off, t); }
  else        { darray_qsort1(x, off, t); }

  const int u = d - c;
  const int off1 = n - u;
  if (u <= SORTN_MAX) { SORTN(x+off1, u); }
  else        { darray_qsort1(x, off1, u); }
#else
  if ((s = b - a) > 1)
    darray_qsort1(x, off, s);
  if ((s = d - c) > 1)
    darray_qsort1(x, n - s, s);
#endif
}

void
darray_qsort(double *a, size_t n)
{
  darray_qsort1(a, 0, n);
}

static void
darray_iqsort1(double *x, size_t *idx, size_t off, size_t len)
{
  double v;

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = doublei_median3(x, l, l + s, l + 2 * s);
      m = doublei_median3(x, m - s, m, m + s);
      n = doublei_median3(x, n - 2 * s, n - s, n);
    }
    m = doublei_median3(x, l, m, n);	/* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	doublei_swapi(x, idx, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	doublei_swapi(x, idx, c, d--);
      c--;
    }
    if (b > c)
      break;
    doublei_swapi(x, idx, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  doublei_rangeswapi(x, idx, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  doublei_rangeswapi(x, idx, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1)
    darray_iqsort1(x, idx, off, s);
  if ((s = d - c) > 1)
    darray_iqsort1(x, idx, n - s, s);

}

void
darray_iqsort(double *a, size_t *idx, size_t n)
{
  darray_iqsort1(a, idx, 0, n);
}

bool
darray_is_sorted(double *a, size_t n)
{
  for (size_t j = 0; j < n - 1; j++) {
    if (a[j] > a[j + 1]) {
      return false;
    }
  }
  return true;
}

/* ---------------------------- Group Separator ---------------------------- */

/* Calculate Index of the Median of the three Indexed Values. */
static inline int ldoublei_median3(const ldouble *x, size_t a, size_t b, size_t c)
{
  return (x[a] < x[b] ?
	  (x[b] < x[c] ? b : x[a] < x[c] ? c : a) :
	  (x[b] > x[c] ? b : x[a] > x[c] ? c : a));
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void ldoublei_swap(ldouble *x, size_t a, size_t b)
{
  ldouble t = x[a]; x[a] = x[b]; x[b] = t;
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void ldoublei_swapi(ldouble *x, size_t *idx, size_t a, size_t b)
{
  ldoublei_swap(x, a, b);
  SWAP(idx[a], idx[b]);
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void ldoublei_rangeswap(ldouble *x, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { ldoublei_swap(x, a, b); }
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void ldoublei_rangeswapi(ldouble *x, size_t *idx, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { ldoublei_swapi(x, idx, a, b); }
}

static void
ldarray_qsort1(ldouble *x, size_t off, size_t len)
{
  ldouble v;

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = ldoublei_median3(x, l, l + s, l + 2 * s);
      m = ldoublei_median3(x, m - s, m, m + s);
      n = ldoublei_median3(x, n - 2 * s, n - s, n);
    }
    m = ldoublei_median3(x, l, m, n); /* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	ldoublei_swap(x, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	ldoublei_swap(x, c, d--);
      c--;
    }
    if (b > c)
      break;
    ldoublei_swap(x, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  ldoublei_rangeswap(x, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  ldoublei_rangeswap(x, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1)
    ldarray_qsort1(x, off, s);
  if ((s = d - c) > 1)
    ldarray_qsort1(x, n - s, s);

}

void
ldarray_qsort(ldouble *a, size_t n)
{
  ldarray_qsort1(a, 0, n);
}

static void
ldarray_iqsort1(ldouble *x, size_t *idx, size_t off, size_t len)
{
  ldouble v;

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = ldoublei_median3(x, l, l + s, l + 2 * s);
      m = ldoublei_median3(x, m - s, m, m + s);
      n = ldoublei_median3(x, n - 2 * s, n - s, n);
    }
    m = ldoublei_median3(x, l, m, n);	/* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	ldoublei_swapi(x, idx, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	ldoublei_swapi(x, idx, c, d--);
      c--;
    }
    if (b > c)
      break;
    ldoublei_swapi(x, idx, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  ldoublei_rangeswapi(x, idx, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  ldoublei_rangeswapi(x, idx, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1)
    ldarray_iqsort1(x, idx, off, s);
  if ((s = d - c) > 1)
    ldarray_iqsort1(x, idx, n - s, s);

}

void
ldarray_iqsort(ldouble *a, size_t *idx, size_t n)
{
  ldarray_iqsort1(a, idx, 0, n);
}

bool
ldarray_is_sorted(ldouble *a, size_t n)
{
  for (size_t j = 0; j < n - 1; j++) {
    if (a[j] > a[j + 1]) {
      return false;
    }
  }
  return true;
}

/* ---------------------------- Group Separator ---------------------------- */

/* Calculate Index of the Median of the three Indexed Values. */
static inline int shorti_median3(const short *x, size_t a, size_t b, size_t c)
{
  return (x[a] < x[b] ?
	  (x[b] < x[c] ? b : x[a] < x[c] ? c : a) :
	  (x[b] > x[c] ? b : x[a] > x[c] ? c : a));
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void shorti_swap(short *x, size_t a, size_t b)
{
  short t = x[a]; x[a] = x[b]; x[b] = t;
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void shorti_swapi(short *x, size_t *idx, size_t a, size_t b)
{
  shorti_swap(x, a, b);
  SWAP(idx[a], idx[b]);
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void shorti_rangeswap(short *x, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { shorti_swap(x, a, b); }
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void shorti_rangeswapi(short *x, size_t *idx, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { shorti_swapi(x, idx, a, b); }
}

/* ---------------------------- Group Separator ---------------------------- */

static void
shortarray_qsort1(short *x, size_t off, size_t len)
{
  short v;

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = shorti_median3(x, l, l + s, l + 2 * s);
      m = shorti_median3(x, m - s, m, m + s);
      n = shorti_median3(x, n - 2 * s, n - s, n);
    }
    m = shorti_median3(x, l, m, n);	/* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	shorti_swap(x, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	shorti_swap(x, c, d--);
      c--;
    }
    if (b > c)
      break;
    shorti_swap(x, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  shorti_rangeswap(x, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  shorti_rangeswap(x, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1)
    shortarray_qsort1(x, off, s);
  if ((s = d - c) > 1)
    shortarray_qsort1(x, n - s, s);

}

void
shortarray_qsort(short *a, size_t n)
{
  shortarray_qsort1(a, 0, n);
}

static void
shortarray_iqsort1(short *x, size_t *idx, size_t off, size_t len)
{
  short v;

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = shorti_median3(x, l, l + s, l + 2 * s);
      m = shorti_median3(x, m - s, m, m + s);
      n = shorti_median3(x, n - 2 * s, n - s, n);
    }
    m = shorti_median3(x, l, m, n);	/* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	shorti_swapi(x, idx, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	shorti_swapi(x, idx, c, d--);
      c--;
    }
    if (b > c)
      break;
    shorti_swapi(x, idx, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  shorti_rangeswapi(x, idx, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  shorti_rangeswapi(x, idx, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1)
    shortarray_iqsort1(x, idx, off, s);
  if ((s = d - c) > 1)
    shortarray_iqsort1(x, idx, n - s, s);

}

void
shortarray_iqsort(short *a, size_t *idx, size_t n)
{
  shortarray_iqsort1(a, idx, 0, n);
}

bool
shortarray_is_sorted(short *a, size_t n)
{
  for (size_t j = 0; j < n - 1; j++) {
    if (a[j] > a[j + 1]) {
      return false;
    }
  }
  return true;
}

/* ---------------------------- Group Separator ---------------------------- */

/* Calculate Index of the Median of the three Indexed Values. */
static inline int sizei_median3(const size_t *x, size_t a, size_t b, size_t c)
{
  return (x[a] < x[b] ?
	  (x[b] < x[c] ?
	   b : x[a] < x[c] ? c : a) :
	  (x[b] > x[c] ?
	   b : x[a] > x[c] ? c : a));
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void sizei_swap(size_t *x, size_t a, size_t b)
{
  int t = x[a]; x[a] = x[b]; x[b] = t;
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void sizei_swapi(size_t *x, size_t *idx, size_t a, size_t b)
{
  sizei_swap(x, a, b);
  SWAP(idx[a], idx[b]);
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void sizei_rangeswap(size_t *x, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { sizei_swap(x, a, b); }
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static inline void sizei_rangeswapi(size_t *x, size_t *idx, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { sizei_swapi(x, idx, a, b); }
}

/* ---------------------------- Group Separator ---------------------------- */

/* Calculate Index of the Median of the three Indexed Values. */
static inline int inti_median3(const int *x, size_t a, size_t b, size_t c)
{
  return (x[a] < x[b] ?
	  (x[b] < x[c] ?
	   b : x[a] < x[c] ? c : a) :
	  (x[b] > x[c] ?
	   b : x[a] > x[c] ? c : a));
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void inti_swap(int *x, size_t a, size_t b)
{
  int t = x[a]; x[a] = x[b]; x[b] = t;
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void inti_swapi(int *x, size_t *idx, size_t a, size_t b)
{
  inti_swap(x, a, b);
  SWAP(idx[a], idx[b]);
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void inti_rangeswap(int *x, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { inti_swap(x, a, b); }
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void inti_rangeswapi(int *x, size_t *idx, size_t a, size_t b, size_t n)
{
  for (size_t i = 0; i < n; i++, a++, b++) { inti_swapi(x, idx, a, b); }
}

static void
intarray_qsort1(int *x, size_t off, size_t len)
{
  int v;

#if QSORT_USE_SORTN
  if (len <= SORTN_MAX) { SORTN(x+off, len); return; }
#endif

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = inti_median3(x, l, l + s, l + 2 * s);
      m = inti_median3(x, m - s, m, m + s);
      n = inti_median3(x, n - 2 * s, n - s, n);
    }
    m = inti_median3(x, l, m, n); /* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	inti_swap(x, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	inti_swap(x, c, d--);
      c--;
    }
    if (b > c)
      break;
    inti_swap(x, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  inti_rangeswap(x, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  inti_rangeswap(x, b, n - s, s);

  /* recursively sort non-partition-elements */
#if QSORT_USE_SORTN
  const int t = b - a;
  if (t <= SORTN_MAX) { SORTN(x+off, t); }
  else        { intarray_qsort1(x, off, t); }

  const int u = d - c;
  const int off1 = n - u;
  if (u <= SORTN_MAX) { SORTN(x+off1, u); }
  else        { intarray_qsort1(x, off1, u); }
#else
  if ((s = b - a) > 1)
    intarray_qsort1(x, off, s);
  if ((s = d - c) > 1)
    intarray_qsort1(x, n - s, s);
#endif
}

void
intarray_qsort(int *a, size_t n)
{
  intarray_qsort1(a, 0, n);
}

static void
intarray_iqsort1(int *x, size_t *idx, size_t off, size_t len)
{
  int v;

  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = inti_median3(x, l, l + s, l + 2 * s);
      m = inti_median3(x, m - s, m, m + s);
      n = inti_median3(x, n - 2 * s, n - s, n);
    }
    m = inti_median3(x, l, m, n);	/* mid-size, med of 3 */
  }
  v = x[m];

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c && x[b] <= v) {
      if (x[b] == v)
	inti_swapi(x, idx, a++, b);
      b++;
    }
    while (c >= b && x[c] >= v) {
      if (x[c] == v)
	inti_swapi(x, idx, c, d--);
      c--;
    }
    if (b > c)
      break;
    inti_swapi(x, idx, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  inti_rangeswapi(x, idx, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  inti_rangeswapi(x, idx, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1)
    intarray_iqsort1(x, idx, off, s);
  if ((s = d - c) > 1)
    intarray_iqsort1(x, idx, n - s, s);

}

void
intarray_iqsort(int *a, size_t *idx, size_t n)
{
  intarray_iqsort1(a, idx, 0, n);
}

bool
intarray_is_sorted(int *a, size_t n)
{
  for (size_t j = 0; j < n - 1; j++) {
    if (a[j] > a[j + 1]) {
      return false;
    }
  }
  return true;
}

/* ---------------------------- Group Separator ---------------------------- */

/* Calculate Index of the Median of the three Indexed Values. */
static inline int anyi_median3(const char *x, size_t size, size_t a, size_t b, size_t c,
			       int(*cmp)(const void *, const void *))
{
  const int ab = cmp(x+a*size, x+b*size);
  const int bc = cmp(x+b*size, x+c*size);
  const int ac = cmp(x+a*size, x+c*size);
  return ((ab < 0) ?
	  ((bc < 0) ? b : (ac < 0) ? c : a) :
	  ((bc > 0) ? b : (ac > 0) ? c : a));
}

/* ---------------------------- Group Separator ---------------------------- */

static inline void anyi_swap_memcpy(char *x, size_t size, size_t a, size_t b)
{
    if (a != b) {
      char t[size];
      memcpy(t, x+a*size, size);	    /* t = x[a]; */
      memcpy(x+a*size, x+b*size, size); /* x[a] = x[b]; */
      memcpy(x+b*size, t, size);	    /* x[b] = t; */
    }
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void anyi_swap(char *x, size_t size, size_t a, size_t b)
{
  switch (size) {
  case 1:
    SWAP(x[a*size], x[b*size]);
    break;
  case 2:
    if (is_aligned2(x)) {
      SWAP(((uint16_t*)(x))[a],
           ((uint16_t*)(x))[b]);
    } else {
      SWAP(x[a*size+0], x[b*size+0]);
      SWAP(x[a*size+1], x[b*size+1]);
    }
    break;
  case 3:
    SWAP(x[a*size+0], x[b*size+0]);
    SWAP(x[a*size+1], x[b*size+1]);
    SWAP(x[a*size+2], x[b*size+2]);
    break;
  case 4:
    if (is_aligned4(x)) {
      SWAP(((uint32_t*)(x))[a],
           ((uint32_t*)(x))[b]);
    } else {
      SWAP(x[a*size+0], x[b*size+0]);
      SWAP(x[a*size+1], x[b*size+1]);
      SWAP(x[a*size+2], x[b*size+2]);
      SWAP(x[a*size+3], x[b*size+3]);
    }
    break;
  case 8:
    if (is_aligned8(x)) {
      SWAP(((uint64_t*)(x))[a],
           ((uint64_t*)(x))[b]);
    } else {
      SWAP(x[a*size+0], x[b*size+0]);
      SWAP(x[a*size+1], x[b*size+1]);
      SWAP(x[a*size+2], x[b*size+2]);
      SWAP(x[a*size+3], x[b*size+3]);
      SWAP(x[a*size+4], x[b*size+4]);
      SWAP(x[a*size+5], x[b*size+5]);
      SWAP(x[a*size+6], x[b*size+6]);
      SWAP(x[a*size+7], x[b*size+7]);
    }
    break;
  case 16:
    if (is_aligned8(x)) {
      SWAP(((uint64_t*)(x))[a+0],
           ((uint64_t*)(x))[b+0]);
      SWAP(((uint64_t*)(x))[a+1],
           ((uint64_t*)(x))[b+1]);
    } else {
      anyi_swap_memcpy(x,size, a,b);
    }
    break;
  default: {
    anyi_swap_memcpy(x,size, a,b);
    break;
  }
  }
}

static inline void anyi_swap2(char *x,size_t xsize,
                              char *y,size_t ysize,
                              size_t a, size_t b)
{
  anyi_swap(x, xsize, a, b);
  anyi_swap(y, ysize, a, b);
}

/*! Swaps \p x[\p a] with \p x[\p b]. */
static inline void anyi_swapi(char *x, size_t size, size_t *idx, size_t a, size_t b)
{
  anyi_swap(x, size, a, b);
  SWAP(idx[a], idx[b]);
}

/* ---------------------------- Group Separator ---------------------------- */

static void anyi_rangeswap_memcpy(char *x, size_t size, size_t a, size_t b, size_t n)
{
  if (a != b) {
    char t[size*n];
    memcpy(t, x+a*size, size*n);
    memcpy(x+a*size, x+b*size, size*n);
    memcpy(x+b*size, t, size*n);
  }
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void anyi_rangeswap(char *x, size_t size, size_t a, size_t b, size_t n)
{
#ifdef QSORT_USE_RANGESWAP_OPT
#  ifdef DEBUG_CHECK_ALL
  if ((a <= b && a + n > b) ||
      (b <= a && b + n > a)) {
    PERR("Memory areas a:%p:%zu and b:%p:%zu overlap!", x+a, n, x+b, n);
    return;
  }
#  endif
  if (n == 1) {
    anyi_swap(x, size, a,b);
  } else {
    anyi_rangeswap_memcpy(x, size, a, b, n);
  }
#else
  for (size_t i = 0; i < n; i++, a++, b++) { anyi_swap(x,size, a, b); }
#endif
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void anyi_rangeswapi(char *x, size_t size, size_t *idx, size_t a, size_t b, size_t n)
{
#ifdef QSORT_USE_RANGESWAP_OPT
  anyi_rangeswap(x, size, a, b, n);
  sizei_rangeswap(idx, a, b, n);
#else
  for (size_t i = 0; i < n; i++, a++, b++) { anyi_swapi(x,size, idx, a, b); }
#endif
}

/*! Swaps \p x[ \p a .. (\p a + \p n - 1)] with \p x[ \p b .. (\p b + \p n - 1)]. */
static void anyi_rangeswap2(char *x,size_t xsize,
                            char *y,size_t ysize,
                            size_t a, size_t b, size_t n)
{
#ifdef QSORT_USE_RANGESWAP_OPT
  anyi_rangeswap(x, xsize, a, b, n);
  anyi_rangeswap(y, ysize, a, b, n);
#else
  for (size_t i = 0; i < n; i++, a++, b++) { anyi_swap2(x,xsize, y,ysize, a, b); }
#endif
}

/* ---------------------------- Group Separator ---------------------------- */

static void
aqsort1(void *x, size_t size, size_t off, size_t len,
        int(*cmp)(const void *, const void *))
{
  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = anyi_median3(x, size, l, l + s, l + 2 * s, cmp);
      m = anyi_median3(x, size, m - s, m, m + s, cmp);
      n = anyi_median3(x, size, n - 2 * s, n - s, n, cmp);
    }
    m = anyi_median3(x, size, l, m, n, cmp); /* mid-size, med of 3 */
  }
  char v[size]; memcpy(v, (char*)x+m*size, size); /* v = x[m]; */

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c &&
	   cmp((char*)x+b*size, v) <= 0) { /* x[b] <= v */
      if (cmp((char*)x+b*size, v) == 0)    /* x[b] == v */
	anyi_swap(x, size, a++, b);
      b++;
    }
    while (c >= b &&
	   cmp((char*)x+c*size, v) >= 0) { /* x[c] >= v */
      if (cmp((char*)x+c*size, v) == 0)    /* x[c] == v */
	anyi_swap(x, size, c, d--);
      c--;
    }
    if (b > c)
      break;
    anyi_swap(x, size, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  anyi_rangeswap(x, size, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  anyi_rangeswap(x, size, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1)
    aqsort1(x, size, off, s, cmp);
  if ((s = d - c) > 1)
    aqsort1(x, size, n - s, s, cmp);
}

void aqsort(void *a, size_t n, size_t size,
            int(*cmp)(const void *, const void *))
{
  aqsort1(a, size, 0, n, cmp);
}

/* ========================================================================= */

static void
iqsort1(void *x, size_t size, size_t *idx, size_t off, size_t len,
        int(*cmp)(const void *, const void *))
{
  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = anyi_median3(x, size, l, l + s, l + 2 * s, cmp);
      m = anyi_median3(x, size, m - s, m, m + s, cmp);
      n = anyi_median3(x, size, n - 2 * s, n - s, n, cmp);
    }
    m = anyi_median3(x, size, l, m, n, cmp);	/* mid-size, med of 3 */
  }
  char v[size]; memcpy(v, (char*)x+m*size, size); /* v = x[m]; */

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c &&
	   cmp((char*)x+b*size, v) <= 0) { /* x[b] <= v */
      if (cmp((char*)x+b*size, v) == 0)    /* x[b] == v */
	anyi_swapi(x, size, idx, a++, b);
      b++;
    }
    while (c >= b &&
	   cmp((char*)x+c*size, v) >= 0) { /* x[c] >= v */
      if (cmp((char*)x+c*size, v) == 0)    /* x[c] == v */
	anyi_swapi(x, size, idx, c, d--);
      c--;
    }
    if (b > c)
      break;
    anyi_swapi(x, size, idx, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  anyi_rangeswapi(x, size, idx, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  anyi_rangeswapi(x, size, idx, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1)
    iqsort1(x, size, idx, off, s, cmp);
  if ((s = d - c) > 1)
    iqsort1(x, size, idx, n - s, s, cmp);

}

void
iqsort(void *a, size_t *idx, size_t n, size_t size,
       int(*cmp)(const void *, const void *))
{
  iqsort1(a, size, idx, 0, n, cmp);
}

/* ========================================================================= */

static void
xqsort1(void *x,size_t xsize,
        void *y,size_t ysize,
        size_t off, size_t len, int(*cmp)(const void *, const void *))
{
  /* choose a partition element, v */
  int m = off + len / 2;		/* small arrays, middle element */
  if (len > 7) {
    int l = off;
    size_t n = off + len - 1;
    if (len > 40) {		/* big arrays, pseudomedian of 9 */
      int s = len / 8;
      l = anyi_median3(x, xsize, l, l + s, l + 2 * s, cmp);
      m = anyi_median3(x, xsize, m - s, m, m + s, cmp);
      n = anyi_median3(x, xsize, n - 2 * s, n - s, n, cmp);
    }
    m = anyi_median3(x, xsize, l, m, n, cmp);	/* mid-size, med of 3 */
  }
  char v[xsize]; memcpy(v, (char*)x+m*xsize, xsize); /* v = x[m]; */

  /* establish Invariant: v* (<v)* (>v)* v* */
  int a = off;
  int b = a;
  int c = off + len - 1;
  int d = c;
  while (true) {
    while (b <= c &&
	   cmp((char*)x+b*xsize, v) <= 0) { /* x[b] <= v */
      if (cmp((char*)x+b*xsize, v) == 0) {  /* x[b] == v */
	anyi_swap2(x,xsize, y,ysize, a++, b);
      }
      b++;
    }
    while (c >= b &&
	   cmp((char*)x+c*xsize, v) >= 0) { /* x[c] >= v */
      if (cmp((char*)x+c*xsize, v) == 0) {  /* x[c] == v */
	anyi_swap2(x,xsize, y,ysize, c, d--);
      }
      c--;
    }
    if (b > c)
      break;
    anyi_swap2(x,xsize, y,ysize, b++, c--);
  }

  /* swap partition elements back to middle */
  int n = off + len;
  int s = int_min2(a - off, b - a);
  anyi_rangeswap2(x,xsize, y,ysize, off, b - s, s);
  s = int_min2(d - c, n - d - 1);
  anyi_rangeswap2(x,xsize, y,ysize, b, n - s, s);

  /* recursively sort non-partition-elements */
  if ((s = b - a) > 1) {
    xqsort1(x,xsize, y,ysize, off, s, cmp);
  }
  if ((s = d - c) > 1) {
    xqsort1(x,xsize, y,ysize, n - s, s, cmp);
  }
}

void
xqsort(void *x, size_t *y, size_t n,
       size_t xsize, size_t ysize,
       int(*cmp)(const void *, const void *))
{
  xqsort1(x,xsize,
          y,ysize,
          0, n, cmp);
}

bool
anyv_is_sorted(void *a, size_t size, size_t n,
               int(*cmp)(const void *, const void *))
{
  for (size_t j = 0; j < n - 1; j++) {
    if (cmp((char*)a+j*size, (char*)a+j*size) > 0) { /* a[j] > a[j+1] */
      return false;
    }
  }
  return true;
}
