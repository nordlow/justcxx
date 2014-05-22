#include "mul.h"
#include "transpose.h"
#include "binlog.h"
#include "meman.h"

#include <stdlib.h>

void
dmatrix_mul_orig(double **a, double **b, double **c, int am, int an, int bn)
{
  int ai, aj, bj;
  for (ai = 0; ai < am; ai++)
    for (bj = 0; bj < bn; bj++) {
      double sum = 0;
      for (aj = 0; aj < an; aj++) {
	sum += b[ai][aj] * c[aj][bj];
      }
      a[ai][bj] = sum;
    }
}

#define P1L (0)
#define P2L (1)
#define P3L (2)
#define P4L (3)
#define P5L (4)
#define P6L (5)
#define RXL (6)
#define RYL (7)
#define RZL (8)
#define C1L (9)
#define C2L (10)
#define C3L (11)
#define C4L (12)
#define C5L (13)
#define C6L (14)

#define P1 (1 << P1L)
#define P2 (1 << P2L)
#define P3 (1 << P3L)
#define P4 (1 << P4L)
#define P5 (1 << P5L)
#define P6 (1 << P6L)
#define RX (1 << RXL)
#define RY (1 << RYL)
#define RZ (1 << RZL)
#define C1 (1 << C1L)
#define C2 (1 << C2L)
#define C3 (1 << C3L)
#define C4 (1 << C4L)
#define C5 (1 << C5L)
#define C6 (1 << C6L)

#define ALL ((1 << 15) - 1)

int
dmatrix_classify3x3(const double **in)
{
  int form = ALL;

  /* Classify based on the diagonal elements. */

  if (in[0][0] != 0)
    form &= P1 | P5 | RX | RY | RZ | C4 | C5;
  if (in[1][1] != 0)
    form &= P1 | P6 | RX | RY | RZ | C1 | C2;
  if (in[2][2] != 0)
    form &= P1 | P4 | RX | RY | RZ | C3 | C6;

  /* Classify based on the upper triangular elements. */

  if (in[0][1] != 0)
    form &= P2 | P4 | RZ | C2 | C3 | C4 | C6;
  if (in[0][2] != 0)
    form &= P3 | P6 | RY | C1 | C2 | C5 | C6;
  if (in[1][2] != 0)
    form &= P2 | P5 | RX | C2 | C3 | C4 | C5;

  /* Classify based on the lower triangular elements. */

  if (in[1][0] != 0)
    form &= P3 | P4 | RZ | C1 | C3 | C5 | C6;
  if (in[2][0] != 0)
    form &= P2 | P6 | RY | C1 | C2 | C3 | C4;
  if (in[2][1] != 0)
    form &= P3 | P5 | RX | C1 | C4 | C5 | C6;

  /* If multiple classifications apply, return only one. Doing this makes it
     possible to do a "switch" on the returned value. */

  if (form & P1)
    return P1L;
  if (form & P2)
    return P2L;
  if (form & P3)
    return P3L;
  if (form & P4)
    return P4L;
  if (form & P5)
    return P5L;
  if (form & P6)
    return P6L;

  if (form & RX)
    return RXL;
  if (form & RY)
    return RYL;
  if (form & RZ)
    return RZL;

  if (form & C1)
    return C1L;
  if (form & C2)
    return C2L;
  if (form & C3)
    return C3L;
  if (form & C4)
    return C4L;
  if (form & C5)
    return C5L;
  if (form & C6)
    return C6L;

  return (form);
}

/*!
 * This routine transforms a vector "v" by a matrix "M" whose structure
 * is specified by "form".  The result is returned in the vector "w".
 * Note that "v" and "w" cannot be the same vector.
 *
 * All zero elements must still be stored in the matrix "M"; the form
 * argument simply indicates which ones to use.  Thus, no storage is
 * saved in this implementation -- just time.
 */
void
double_transform3x3(double *y0, double *y1, double *y2,
		 const double **m, uint form,
		 double *x0, double *x1, double *x2)
{
  switch (form) {
  case P1L:
    *y0 = m[0][0] * *x0;
    *y1 = m[1][1] * *x1;
    *y2 = m[2][2] * *x2;
    break;
  case P2L:
    *y0 = m[0][1] * *x0;
    *y1 = m[1][2] * *x1;
    *y2 = m[2][0] * *x2;
    break;
  case P3L:
    *y0 = m[0][2] * *x0;
    *y1 = m[1][0] * *x1;
    *y2 = m[2][1] * *x2;
    break;
  case P4L:
    *y0 = m[0][1] * *x0;
    *y1 = m[1][0] * *x1;
    *y2 = m[2][2] * *x2;
    break;
  case P5L:
    *y0 = m[0][0] * *x0;
    *y1 = m[1][2] * *x1;
    *y2 = m[2][1] * *x2;
    break;
  case P6L:
    *y0 = m[0][2] * *x0;
    *y1 = m[1][1] * *x1;
    *y2 = m[2][0] * *x2;
    break;
  case RXL:
    *y0 = m[0][0] * *x0;
    *y1 = m[1][1] * *x1 + m[1][2] * *x2;
    *y2 = m[2][1] * *x1 + m[2][2] * *x2;
    break;
  case RYL:
    *y0 = m[0][0] * *x0 + m[0][2] * *x2;
    *y1 = m[1][1] * *x1;
    *y2 = m[2][0] * *x0 + m[2][2] * *x2;
    break;
  case RZL:
    *y0 = m[0][0] * *x0 + m[0][1] * *x1;
    *y1 = m[1][0] * *x0 + m[1][1] * *x1;
    *y2 = m[2][2] * *x2;
    break;
  case C1L:
    *y0 = m[0][2] * *x2;
    *y1 = m[1][0] * *x0 + m[1][1] * *x1;
    *y2 = m[2][0] * *x0 + m[2][1] * *x1;
    break;
  case C2L:
    *y0 = m[0][1] * *x1 + m[0][2] * *x2;
    *y1 = m[1][1] * *x1 + m[1][2] * *x2;
    *y2 = m[2][0] * *x0;
    break;
  case C3L:
    *y0 = m[0][1] * *x1;
    *y1 = m[1][0] * *x0 + m[1][2] * *x2;
    *y2 = m[2][0] * *x0 + m[2][2] * *x2;
    break;
  case C4L:
    *y0 = m[0][0] * *x0 + m[0][1] * *x1;
    *y1 = m[1][2] * *x2;
    *y2 = m[2][0] * *x0 + m[2][1] * *x1;
    break;
  case C5L:
    *y0 = m[0][0] * *x0 + m[0][2] * *x2;
    *y1 = m[1][0] * *x0 + m[1][2] * *x2;
    *y2 = m[2][1] * *x1;
    break;
  case C6L:
    *y0 = m[0][1] * *x1 + m[0][2] * *x2;
    *y1 = m[1][0] * *x0;
    *y2 = m[2][1] * *x1 + m[2][2] * *x2;
    break;
  default:
    *y0 = m[0][0] * *x0 + m[0][1] * *x1 + m[0][2] * *x2;
    *y1 = m[1][0] * *x0 + m[1][1] * *x1 + m[1][2] * *x2;
    *y2 = m[2][0] * *x0 + m[2][1] * *x1 + m[2][2] * *x2;
    break;
  }
}

void
dmatrix_mmul3x3(double **y,
	     const double **m, uint form, const double **x, int n)
{
  switch (form) {
  case P1L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][0] * x[0][i];
      y[1][i] = m[1][1] * x[1][i];
      y[2][i] = m[2][2] * x[2][i];
    }
    break;
  case P2L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][1] * x[0][i];
      y[1][i] = m[1][2] * x[1][i];
      y[2][i] = m[2][0] * x[2][i];
    }
    break;
  case P3L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][2] * x[0][i];
      y[1][i] = m[1][0] * x[1][i];
      y[2][i] = m[2][1] * x[2][i];
    }
    break;
  case P4L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][1] * x[0][i];
      y[1][i] = m[1][0] * x[1][i];
      y[2][i] = m[2][2] * x[2][i];
    }
    break;
  case P5L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][0] * x[0][i];
      y[1][i] = m[1][2] * x[1][i];
      y[2][i] = m[2][1] * x[2][i];
    }
    break;
  case P6L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][2] * x[0][i];
      y[1][i] = m[1][1] * x[1][i];
      y[2][i] = m[2][0] * x[2][i];
    }
    break;
  case RXL:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][0] * x[0][i];
      y[1][i] = m[1][1] * x[1][i] + m[1][2] * x[2][i];
      y[2][i] = m[2][1] * x[1][i] + m[2][2] * x[2][i];
    }
    break;
  case RYL:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][0] * x[0][i] + m[0][2] * x[2][i];
      y[1][i] = m[1][1] * x[1][i];
      y[2][i] = m[2][0] * x[0][i] + m[2][2] * x[2][i];
    }
    break;
  case RZL:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][0] * x[0][i] + m[0][1] * x[1][i];
      y[1][i] = m[1][0] * x[0][i] + m[1][1] * x[1][i];
      y[2][i] = m[2][2] * x[2][i];
    }
    break;
  case C1L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][2] * x[2][i];
      y[1][i] = m[1][0] * x[0][i] + m[1][1] * x[1][i];
      y[2][i] = m[2][0] * x[0][i] + m[2][1] * x[1][i];
    }
    break;
  case C2L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][1] * x[1][i] + m[0][2] * x[2][i];
      y[1][i] = m[1][1] * x[1][i] + m[1][2] * x[2][i];
      y[2][i] = m[2][0] * x[0][i];
    }
    break;
  case C3L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][1] * x[1][i];
      y[1][i] = m[1][0] * x[0][i] + m[1][2] * x[2][i];
      y[2][i] = m[2][0] * x[0][i] + m[2][2] * x[2][i];
    }
    break;
  case C4L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][0] * x[0][i] + m[0][1] * x[1][i];
      y[1][i] = m[1][2] * x[2][i];
      y[2][i] = m[2][0] * x[0][i] + m[2][1] * x[1][i];
    }
    break;
  case C5L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][0] * x[0][i] + m[0][2] * x[2][i];
      y[1][i] = m[1][0] * x[0][i] + m[1][2] * x[2][i];
      y[2][i] = m[2][1] * x[1][i];
    }
    break;
  case C6L:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][1] * x[1][i] + m[0][2] * x[2][i];
      y[1][i] = m[1][0] * x[0][i];
      y[2][i] = m[2][1] * x[1][i] + m[2][2] * x[2][i];
    }
    break;
  default:
    for (int i = 0; i < n; i++) {
      y[0][i] = m[0][0] * x[0][i] + m[0][1] * x[1][i] + m[0][2] * x[2][i];
      y[1][i] = m[1][0] * x[0][i] + m[1][1] * x[1][i] + m[1][2] * x[2][i];
      y[2][i] = m[2][0] * x[0][i] + m[2][1] * x[1][i] + m[2][2] * x[2][i];
    }
    break;
  }
}

void
dmatrix_mmul_straight(double **out,
		   const double **in1, const double **in2,
		   int w1, int h1, int w2)
{
  double **old_out = NULL;
  int self_multiply;		/* flags for self multiply */

  /* \TODO: Optimize especially for the case when out == in1 and out != in2
     in which case we only a temporary row buffer. */
  self_multiply = ((void *) out == (void *) in1 ||
		   (void *) out == (void *) in2);

  if (self_multiply) {
    old_out = out;
    out = doubleAA_malloc(h1, w2);
  }

  for (int y = 0; y < h1; y++) {
    for (int x = 0; x < w2; x++) {
      double sum = 0;
      for (int i = 0; i < w1; i++) {
	sum += in1[y][i] * in2[i][y];
      }
      out[y][x] = sum;
    }
  }

  if (self_multiply) {
    darray_copy(old_out[0], out[0], w2 * h1);
    dmatrix_free(out);
  }
}

void
dmatrix_mmul_tr(double **out,
	     const double **in1, const double **in2, int w1, int h1, int h2)
{
  double **old_out = NULL;
  int self_multiply;

  /* \TODO: Optimize especially for the case when out == in1 and out != in2
     in which case we only a temporary row buffer. */
  self_multiply = ((void *) out == (void *) in1 ||
		   (void *) out == (void *) in2);

  if (self_multiply) {
    old_out = out;
    out = doubleAA_malloc(h2, h1);
  }

  for (int y1 = 0; y1 < h1; y1++) {
    for (int y2 = 0; y2 < h2; y2++) {
      double sum = 0;
      for (int x = 0; x < w1; x++) {
	sum += in1[y1][x] * in2[y2][x];
      }
      out[y1][y2] = sum;
    }
  }

  if (self_multiply) {
    darray_copy(old_out[0], out[0], h1 * h2);
    dmatrix_free(out);
  }
}

void
dmatrix_mmul_strassen(double **out,
		   const double **in1, const double **in2,
		   int w1, int h1, int w2)
{
  double **in2_tr;

  if (w1 != h1 || w1 != w2 || !IS_BINPOW(w1)) {
    leprintf("invalid argument dimensions.\n");
    return;
  }

  in2_tr = doubleAA_malloc(w2, w1);
  llongmatrix_transpose((long long **) in2_tr, (const long long **) in2, w1, w2);

  /* Reuse. */
  dmatrix_mmul_tr_strassen(out, in1, (const double **) in2_tr, w1, h1, w2);

  dmatrix_free(in2_tr);
}

#ifdef HAVE_LIBGMP_
void
mpzm_mmul_tr(mpz_t ** out, const mpz_t ** in1, const mpz_t ** in2,
	     int w1, int h1, int h2)
{
  int x, y1, y2;
  mpz_t product, sum;
  mpz_t **old_out = NULL;
  int self_multiply;

  /* \TODO: Optimize especially for the case when out == in1 and out != in2
     in which case we only a temporary row buffer. */
  self_multiply = ((void *) out == (void *) in1 ||
		   (void *) out == (void *) in2);

  if (self_multiply) {
    old_out = out;
    out = mpzm_allocate(h1, h2);
  }

  mpz_init(sum);
  mpz_init(product);

  for (y1 = 0; y1 < h1; y1++)
    for (y2 = 0; y2 < h2; y2++) {
      mpz_set_ui(sum, 0);
      for (x = 0; x < w1; x++) {
	mpz_mul(product, in1[y1][x], in2[y2][x]);
	mpz_add(sum, sum, product);
      }
      mpz_set(out[y1][y2], sum);
    }

  mpz_clear(sum);
  mpz_clear(product);

  if (self_multiply) {
    mpza_copy(old_out[0], (const mpz_t *) out[0], h1 * h2);
    mpzm_free(out, h1 * h2);
  }
}
#endif

/* FIXME:
   Call dmatrix_mmul_straight dmatrix_mmul_tr depending on the dimensions. */
void
dmatrix_mmul(double **out, const double **in1, const double **in2,
	  int w1, int h1, int w2)
{
  double **in2_tr;

  in2_tr = doubleAA_malloc(w2, w1);
  llongmatrix_transpose((long long **) in2_tr, (const long long **) in2, w1, w2);

  /* Reuse. */
  dmatrix_mmul_tr(out, in1, (const double **) in2_tr, w1, h1, w2);

  dmatrix_free(in2_tr);
}

/*!
 * Strassen Matrix Multiply:
 *
 * http://www.supercomp.org/sc96/proceedings/SC96PROC/JACOBSON/INDEX.HTM
 * http://www.supercomp.org/sc98/TechPapers/sc98_FullAbstracts/Thottethodi872/
 *
 * | C11 C12 |  =  | A11 A12 |  \/  | B11 B12 |
 * | C21 C22 |  =  | A21 A22 |  /\  | B21 B22 |
 *
 * P = (A11 + A22) * (B11 + B22 )
 * Q = (A21 + A22) *     B11
 * R =     A11     * (B12 - B22)
 * S =     A22     * (B21 - B11)
 * T = (A11 + A12) *     B22
 * U = (A21 - A11) * (B11 + B12)
 * V = (A12 - A22) * (B21 - B22)
 *
 * C11 = P + S - T + V
 * C12 = R + T
 * C21 = Q + S
 * C22 = P + R - Q + U
 */
void
dmatrix_mmul_tr_strassen(double **out,
		      const double **in1, const double **in2,
		      int w1, int h1, int w2)
{
  int i, n, nn;

  /* Common memory block for all sub blocks and temporary blocks. */
  double *buffer;
  double **temp_rows;

  double **a11, **a12, **a21, **a22;
  double **b11, **b12, **b21, **b22;
  double **c11, **c12, **c21, **c22;
  double **p, **p1, **p2;
  double **q, **q1;
  double **r, **r2;
  double **s, **s2;
  double **t, **t1;
  double **u, **u1, **u2;
  double **v, **v1, **v2;

  const int recurse_limit = 2;

  if (w1 <= recurse_limit) {
    /* Call normal multiplication. */
    dmatrix_mmul_tr(out, in1, in2, w1, h1, w2);
    return;
  }

  n = w1 / 2;
  nn = n * n;

  /* Allocate in regions and temps. */

  buffer = darray_malloc(25 * nn);

  temp_rows = malloc(25 * n * sizeof(double *));
  for (i = 0; i < 25 * n; i++)
    temp_rows[i] = buffer + i * n;

  a11 = temp_rows + 0 * n;
  a12 = temp_rows + 1 * n;
  a21 = temp_rows + 2 * n;
  a22 = temp_rows + 3 * n;

  b11 = temp_rows + 4 * n;
  b12 = temp_rows + 5 * n;
  b21 = temp_rows + 6 * n;
  b22 = temp_rows + 7 * n;

  p = temp_rows + 8 * n;
  p1 = temp_rows + 9 * n;
  p2 = temp_rows + 10 * n;

  q = temp_rows + 11 * n;
  q1 = temp_rows + 12 * n;

  r = temp_rows + 13 * n;
  r2 = temp_rows + 14 * n;

  s = temp_rows + 15 * n;
  s2 = temp_rows + 16 * n;

  t = temp_rows + 17 * n;
  t1 = temp_rows + 18 * n;

  u = temp_rows + 19 * n;
  u1 = temp_rows + 20 * n;
  u2 = temp_rows + 21 * n;

  v = temp_rows + 22 * n;
  v1 = temp_rows + 23 * n;
  v2 = temp_rows + 24 * n;

  /* Get in regions. */

/*      puts("in1:"); */
/*      dmatrix_print (in1, 2*n, 2*n); */

  dmatrix_subreadcopy(a11, in1, 0, 0, n, n);
  dmatrix_subreadcopy(a12, in1, 0, n, n, n);
  dmatrix_subreadcopy(a21, in1, n, 0, n, n);
  dmatrix_subreadcopy(a22, in1, n, n, n, n);

/*      puts("a11:"); */
/*      dmatrix_print (a11, n, n); */

/*      puts("a12:"); */
/*      dmatrix_print (a12, n, n); */

/*      puts("a21:"); */
/*      dmatrix_print (a21, n, n); */

/*      puts("a22:"); */
/*      dmatrix_print (a22, n, n); */

  dmatrix_subreadcopy(b11, in2, 0, 0, n, n);
  dmatrix_subreadcopy(b12, in2, 0, n, n, n);
  dmatrix_subreadcopy(b21, in2, n, 0, n, n);
  dmatrix_subreadcopy(b22, in2, n, n, n, n);

  /* Calculate temps. */

  /* P = (A11 + A22) * (B11 + B22 ) */
  darray_add2(p1[0], a11[0], a22[0], nn);
  darray_add2(p2[0], b11[0], b22[0], nn);
  dmatrix_mmul_tr_strassen(p, (const double **) p1, (const double **) p2,
			n, n, n);

  /* Q = (A21 + A22) *     B11      */
  darray_add2(q1[0], a21[0], a22[0], nn);
  dmatrix_mmul_tr_strassen(q, (const double **) q1, (const double **) b11,
			n, n, n);

  /* R =     A11     * (B12 - B22)  */
  darray_sub2(r2[0], b12[0], b22[0], nn);
  dmatrix_mmul_tr_strassen(r, (const double **) a11, (const double **) r2,
			n, n, n);

  /* S =     A22     * (B21 - B11)  */
  darray_sub2(s2[0], b21[0], b11[0], nn);
  dmatrix_mmul_tr_strassen(s, (const double **) a22, (const double **) s2,
			n, n, n);

  /* T = (A11 + A12) *     B22      */
  darray_add2(t1[0], a11[0], a12[0], nn);
  dmatrix_mmul_tr_strassen(t, (const double **) t1, (const double **) b22,
			n, n, n);

  /* U = (A21 - A11) * (B11 + B12)  */
  darray_sub2(u1[0], a21[0], a11[0], nn);
  darray_add2(u2[0], b11[0], b12[0], nn);
  dmatrix_mmul_tr_strassen(u, (const double **) u1, (const double **) u2,
			n, n, n);

  /* V = (A12 - A22) * (B21 - B22)  */
  darray_sub2(v1[0], a12[0], a22[0], nn);
  darray_add2(v2[0], b21[0], b22[0], nn);
  dmatrix_mmul_tr_strassen(v, (const double **) v1, (const double **) v2,
			n, n, n);

  /* No freeing of a11, a12, a21, a22, they are reused as out regions */
  c11 = a11;
  c12 = a12;
  c21 = a21;
  c22 = a22;

  /* which are calculated, */

  /* C11 = P + S - T + V */
  darray_add2(c11[0], p[0], s[0], nn);
  darray_sub1(c11[0], t[0], nn);
  darray_add1(c11[0], v[0], nn);

  /* C12 = R + T */
  darray_add2(c12[0], r[0], t[0], nn);

  /* C21 = Q + S */
  darray_add2(c21[0], q[0], s[0], nn);

  /* C22 = P + R - Q + U */
  darray_add2(c22[0], p[0], r[0], nn);
  darray_sub1(c22[0], q[0], nn);
  darray_add1(c22[0], u[0], nn);

  /* and written back. */

  dmatrix_subwritecopy(out, 0, 0, (const double **) c11, n, n);
  dmatrix_subwritecopy(out, 0, n, (const double **) c12, n, n);
  dmatrix_subwritecopy(out, n, 0, (const double **) c21, n, n);
  dmatrix_subwritecopy(out, n, n, (const double **) c22, n, n);

/*      puts("out:"); */
/*      dmatrix_print (out, 2*n, 2*n); */

/*      puts("c11:"); */
/*      dmatrix_print (c11, n, n); */

/*      puts("c12:"); */
/*      dmatrix_print (c12, n, n); */

/*      puts("c21:"); */
/*      dmatrix_print (c21, n, n); */

/*      puts("c22:"); */
/*      dmatrix_print (c22, n, n); */

  /* Finally free everything. */

  free(buffer);
  free(temp_rows);
}

void
mvmul4x4f(float out[4], const float in1[4][4], const float in2[4])
{
#ifdef HAVE_CPU_3DNOW
  __asm__ volatile (		/* Uses mm0 ... mm5 */
		     "movq   (%2), %%mm0 \n\t"	/* Read X[ 0 ] and X[ 1 ] */
		     "movq  8(%2), %%mm1 \n\t"	/* Read X[ 2 ] and X[ 3 ] */
		     "movq   (%1), %%mm2 \n\t"	/* Read M[ 0 ] and M[ 1 ] */
		     "movq  8(%1), %%mm3 \n\t"	/* Read M[ 2 ] and M[ 3 ] */
		     "pfmul %%mm0, %%mm2 \n\t" "pfmul %%mm1, %%mm3 \n\t" "pfadd %%mm2, %%mm3 \n\t" "pfacc %%mm3, %%mm3 \n\t" "movq 16(%1), %%mm4 \n\t"	/* Read M[ 4 ] and M[ 5 ] */
		     "movq 24(%1), %%mm5 \n\t"	/* Read M[ 6 ] and M[ 7 ] */
		     "pfmul %%mm0, %%mm4 \n\t" "pfmul %%mm1, %%mm5 \n\t" "pfadd %%mm4, %%mm5 \n\t" "pfacc %%mm5, %%mm5 \n\t" "punpckldq %%mm5, %%mm3 \n\t"	/* mm3 = mm5(63:32):mm3(31:0) */
		     "movq  %%mm3,   (%0)\n\t"	/* Write Y[ 0 ] and Y[ 1 ] */
		     "movq 32(%1), %%mm2 \n\t"	/* Read M[ 0 ] and M[ 1 ] */
		     "movq 40(%1), %%mm3 \n\t"	/* Read M[ 2 ] and M[ 3 ] */
		     "pfmul %%mm0, %%mm2 \n\t" "pfmul %%mm1, %%mm3 \n\t" "pfadd %%mm2, %%mm3 \n\t" "pfacc %%mm3, %%mm3 \n\t" "movq 48(%1), %%mm4 \n\t"	/* Read M[ 4 ] and M[ 5 ] */
		     "movq 56(%1), %%mm5 \n\t"	/* Read M[ 6 ] and M[ 7 ] */
		     "pfmul %%mm0, %%mm4 \n\t" "pfmul %%mm1, %%mm5 \n\t" "pfadd %%mm4, %%mm5 \n\t" "pfacc %%mm5, %%mm5 \n\t" "punpckldq %%mm5, %%mm3 \n\t"	/* mm3 = mm5(63:32):mm3(31:0) */
		     "movq  %%mm3,   8(%0)\n\t"	/* Write Y[ 2 ] and Y[ 3 ] */
		     ::"r" (out), "r"(in1), "r"(in2));
#else
  int y;
  for (y = 0; y < 4; y++) {
    out[y] =
      in1[y][0] * in2[0] +
      in1[y][1] * in2[1] + in1[y][2] * in2[2] + in1[y][3] * in2[3];
  }
#endif
}
