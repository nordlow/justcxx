#include "utils.h"

#include "stdio_x.h"
#include "sortn.h"
#include "extremes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <float.h>

#ifdef HAVE_READLINE_READLINE_H
#  include <readline/readline.h>
#  include <readline/history.h>
#endif

/* ========================================================================= */

const uint64_t g_uint64_ones = 0xffffffffffffffffULL;

/* ---------------------------- Group Separator ---------------------------- */

void
pnw_print(char *fmt, ...)
{
  va_list ap;
  int d;
  char c, *s;

  va_start(ap, fmt);
  while (*fmt) {
    switch (*fmt++) {
    case 's':			/* string */
      s = va_arg(ap, char *);
      printf("string %s\n", s);
      break;
    case 'd':			/* int */
      d = va_arg(ap, int);
      printf("int %d\n", d);
      break;
    case 'c':			/* char */
      /* need a cast here since va_arg only
         takes fully promoted types */
      c = (char) va_arg(ap, int);
      printf("char %c\n", c);
      break;
    }
  }
  va_end(ap);
}

/* ---------------------------- Group Separator ---------------------------- */

float
va_sum(size_t cnt, ...)
{
  float sum = 0;
  va_list ap;
  va_start(ap, cnt);
  for (size_t i = 0; i < cnt; i++) {
    sum += va_arg(ap, double);
  }
  va_end(ap);
  return sum;
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_add1(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] += b[i];
  }
}

void
darray_add1(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] += b[i];
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_sub1(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] -= b[i];
  }
}

void
darray_sub1(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] -= b[i];
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_mul1(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] *= b[i];
  }
}

void
darray_mul1(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] *= b[i];
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_div1(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] /= b[i];
  }
}

void
darray_div1(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] /= b[i];
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_scale(float * a, float b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] *= b;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
b16v_copy(uint16_t *a, const uint16_t *b, size_t n)
{
  memcpy(a, b, n * sizeof(uint16_t));
}

void
b32v_copy(uint32_t *a, const uint32_t *b, size_t n)
{
  memcpy(a, b, n * sizeof(uint32_t));
}

void
intarray_copy(int *a, const int *b, size_t n)
{
  memcpy(a, b, n * sizeof(int));
}

void
farray_copy(float *a, const float *b, size_t n)
{
  memcpy(a, b, n * sizeof(float));
}

void
darray_copy(double *a, const double *b, size_t n)
{
  memcpy(a, b, n * sizeof(double));
}

void
fmatrix_copy(float **a, const float **b, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    farray_copy(a[i], b[i], n);
  }
}

void
dmatrix_copy(double **a, const double **b, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    darray_copy(a[i], b[i], n);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
intarray_zeros(int *a, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = 0;
  }
}

void
fmatrix_zeros(float **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i][j] = 0.0;
    }
  }
}

void
dmatrix_zeros(double **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i][j] = 0.0;
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
intarray_add2_int(int *a, const int *b, int c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] + c;
  }
}

void
intarray_sub2_int(int *a, const int *b, int c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] - c;
  }
}

void
intarray_print(const int *a, size_t n)
{
  /* printf("%2s: ", name); */
  for (size_t i = 0; i < n; i++) {
    printf("%2d ", a[i]);
  }
  printf("\n");
}

void
sizearray_print(const char *name, const size_t *a, size_t n)
{
  printf("%2s: ", name);
  for (size_t i = 0; i < n; i++) {
    printf("%2zd ", a[i]);
  }
  printf("\n");
}

/* ---------------------------- Group Separator ---------------------------- */

int intarray_bsearch(const int * a, size_t n, int key)
{
  int low = 0;
  int high = n - 1;

  while (low <= high) {
    int mid = (low + high) / 2;
    int midVal = a[mid];

    if (midVal < key) {
      low = mid + 1;
    } else if (midVal > key) {
      high = mid - 1;
    } else {
      return mid; // key found
    }
  }

  return -(low + 1);  // key not found.
}

/* ---------------------------- Group Separator ---------------------------- */

void
intarray_ramp(int *a, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = i;
  }
}

void
sizearray_ramp(size_t *a, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = i;
  }
}

void
farray_ramp(float *a, size_t n, float min, float step)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = min + (float) j *step;
  }
}

void
darray_ramp(double *a, size_t n, double min, double step)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = min + (double) j *step;
  }
}

void
fmatrix_ramp(float **a, size_t m, size_t n, float min, float istep, float jstep)
{
  for (size_t i = 0; i < m; i++) {
    farray_ramp(a[i], n, min + i * istep, jstep);
  }
}

void
dmatrix_ramp(double **a, size_t m, size_t n, double min, double istep, double jstep)
{
  for (size_t i = 0; i < m; i++) {
    darray_ramp(a[i], n, min + i * istep, jstep);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_ones(float *a, size_t n)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = 1;
  }
}

void
darray_ones(double *a, size_t n)
{
  for (size_t j = 0; j < n; j++) {
    a[j] = 1;
  }
}

void
fmatrix_ones(float **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i][j] = 1.0;
    }
  }
}

void
dmatrix_ones(double **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i][j] = 1.0;
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_coswave(float *out, size_t n, float ampl, float freq, float phase)
{
  for (size_t j = 0; j < n; j++) {
    out[j] = ampl * cos(phase + (float) j / n * freq * M_2PI);
  }
}

void
darray_coswave(double *out, size_t n, double ampl, double freq, double phase)
{
  for (size_t j = 0; j < n; j++) {
    out[j] = ampl * cos(phase + (double) j / n * freq * M_2PI);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_sinwave(float *out, size_t n, float ampl, float freq, float phase)
{
  for (size_t j = 0; j < n; j++) {
    out[j] = ampl * sin(phase + (float) j / n * freq * M_2PI);
  }
}

void
darray_sinwave(double *out, size_t n, double ampl, double freq, double phase)
{
  for (size_t j = 0; j < n; j++) {
    out[j] = ampl * sin(phase + (double) j / n * freq * M_2PI);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_flowerspiral(float *x, float *y, size_t n,
                    float ampl, float freq, float phase)
{
  for (size_t j = 0; j < n; j++) {
    float r = (float) j / 4 * sin((float) 16 * j / n * freq * M_2PI);
    x[j] = ampl * (j + r) * cos(phase + (float) j / n * freq * M_2PI);
    y[j] = ampl * (j + r) * sin(phase + (float) j / n * freq * M_2PI);
  }
}

void
darray_flowerspiral(double *x, double *y, size_t n,
                    double ampl, double freq, double phase)
{
  for (size_t j = 0; j < n; j++) {
    double r = (double) j / 4 * sin((double) 16 * j / n * freq * M_2PI);
    x[j] = ampl * (j + r) * cos(phase + (double) j / n * freq * M_2PI);
    y[j] = ampl * (j + r) * sin(phase + (double) j / n * freq * M_2PI);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_add2(float *a, const float *b, const float *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] + c[i];
  }
}

void
farray_sub2(float *a, const float *b, const float *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] - c[i];
  }
}

void
farray_mul2(float *a, const float *b, const float *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] * c[i];
  }
}

void
farray_div2(float *a, const float *b, const float *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] / c[i];
  }
}

void
farray_min2_dbl(float *a, const float * b, float c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = MIN2(b[i], c);
  }
}

void
farray_max2_dbl(float *a, const float * b, float c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = MAX2(b[i], c);
  }
}

void
farray_pow2(float *a, const float *b, const float *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = pow(b[i], c[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_add2_dbl(float *a, const float *b, float c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] + c;
  }
}

void
farray_sub2_dbl(float *a, const float *b, float c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] - c;
  }
}

void
farray_mul2_dbl(float *a, const float *b, float c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] * c;
  }
}

void
farray_div2_dbl(float *a, const float *b, float c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] / c;
  }
}

void
farray_pow2_dbl(float *a, const float *b, float c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = pow(b[i], c);
  }
}

void
farray_pow2_double_rev(float *a, const float *b, float c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = pow(c, b[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_neg(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = -b[i];
  }
}

void
farray_abs(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = fabs(b[i]);
  }
}

void
farray_inv(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = 1 / b[i];
  }
}

/* ---------------------------- Group Separator ---------------------------- */

float
farray_sum(const float *a, size_t n)
{
  float sum;
  sum = a[0];
  for (size_t i = 1; i < n; i++) {
    sum += a[i];
  }
  return sum;
}

float
farray_prod(const float *a, size_t n)
{
  float prod;
  prod = a[0];
  for (size_t i = 1; i < n; i++) {
    prod *= a[i];
  }
  return prod;
}

float
farray_min(const float *a, size_t n)
{
  float min;
  min = a[0];
  for (size_t i = 1; i < n; i++) {
    min = MIN2(min, a[i]);
  }
  return min;
}

float
farray_max(const float *a, size_t n)
{
  float max;
  max = a[0];
  for (size_t i = 1; i < n; i++) {
    max = MAX2(max, a[i]);
  }
  return max;
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_cos(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = cos(b[i]);
  }
}

void
farray_sin(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = sin(b[i]);
  }
}

void
farray_tan(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = tan(b[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_acos(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = acos(b[i]);
  }
}

void
farray_asin(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = asin(b[i]);
  }
}

void
farray_atan(float *a, const float *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = atan(b[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_eud2D(float *a, const float *b, const float *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = hypot(b[i], c[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*
 * Zero length arrays should at least be safe.
 */
void
farray_extremes(float *a, size_t n, float *min_ret, float *max_ret)
{
  float min = FLT_MAX, max = FLT_MIN;
  for (size_t j = 0; j < n; j++) {
    min = MIN2(min, a[j]);
    max = MAX2(max, a[j]);
  }
  *min_ret = min;
  *max_ret = max;
}

/* ---------------------------- Group Separator ---------------------------- */

void
farray_fprint(FILE * stream, const float *a, size_t n)
{
  char str[64];
  size_t maxlen = 0;
  for (size_t j = 0; j < n; j++) {
    snprintf(str, sizeof(str), "%g", a[j]);
    maxlen = MAX2(maxlen, strlen(str));
  }
  fprintf(stream, "[ ");
  for (size_t j = 0; j < n; j++) {
    fprintf(stream, "%*g ", (int)maxlen, a[j]);
  }
  fprintf(stream, "]\n");
}

void
farray_print(const float *a, size_t n)
{
  farray_fprint(stdout, a, n);
}

void
farray_fprintComplex(FILE * stream, const float *re, const float *im, size_t n)
{
  for (size_t j = 0; j < n; j++) {
    fprintf(stream, "%g+%gi ", re[j], im[j]);
  }
  fprintf(stream, "\n");
}

void
farray_printComplex(const float *re, const float *im, size_t n)
{
  farray_fprintComplex(stdout, re, im, n);
}

/* ---------------------------- Group Separator ---------------------------- */

int
fmatrix_any(const float **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      if (a[i][j] == 1) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

int
dmatrix_any(const double **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      if (a[i][j] == 1) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/* ---------------------------- Group Separator ---------------------------- */

int
fmatrix_all(const float **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      if (a[i][j] == 0) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

int
dmatrix_all(const double **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      if (a[i][j] == 0) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_eye(float **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      if (i == j) {
        a[i][j] = 1.0;
      } else {
        a[i][j] = 0.0;
      }
    }
  }
}

void
dmatrix_eye(double **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      if (i == j) {
        a[i][j] = 1.0;
      } else {
        a[i][j] = 0.0;
      }
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_diag(float **a, float *b, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      if (i == j) {
        a[i][i] = b[i];
      } else {
        a[i][j] = 0.0;
      }
    }
  }
}

void
dmatrix_diag(double **a, double *b, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      if (i == j) {
        a[i][i] = b[i];
      } else {
        a[i][j] = 0.0;
      }
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_subcopy(float **a, int a_i0, int a_j0,
                const float **b, int b_i0, int b_j0, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i + a_i0][j + a_j0] = b[i + b_i0][j + b_j0];
    }
  }
}

void
dmatrix_subcopy(double **a, int a_i0, int a_j0,
                const double **b, int b_i0, int b_j0, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i + a_i0][j + a_j0] = b[i + b_i0][j + b_j0];
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_subreadcopy(float **a,
                    const float **b, int b_i0, int b_j0, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i][j] = b[i + b_i0][j + b_j0];
    }
  }
}

void
dmatrix_subreadcopy(double **a,
                    const double **b, int b_i0, int b_j0, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i][j] = b[i + b_i0][j + b_j0];
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_subwritecopy(float **a, int a_i0, int a_j0,
                     const float **b, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i + a_i0][j + a_j0] = b[i][j];
    }
  }
}

void
dmatrix_subwritecopy(double **a, int a_i0, int a_j0,
                     const double **b, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i + a_i0][j + a_j0] = b[i][j];
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_colcat(float **a, const float **b, const float **c,
               size_t m, size_t bn, size_t cn)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < bn; j++) {
      a[i][j] = b[i][j];
    }
  }
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < cn; j++) {
      a[i][bn + j] = c[i][j];
    }
  }
}

void
dmatrix_colcat(double **a, const double **b, const double **c,
               size_t m, size_t bn, size_t cn)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < bn; j++) {
      a[i][j] = b[i][j];
    }
  }
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < cn; j++) {
      a[i][bn + j] = c[i][j];
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_rowcat(float **a, const float **b, const float **c,
               size_t bm, size_t cm, size_t n)
{
  for (size_t i = 0; i < bm; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i][j] = b[i][j];
    }
  }
  for (size_t i = 0; i < cm; i++) {
    for (size_t j = 0; j < n; j++) {
      a[bm + i][j] = c[i][j];
    }
  }
}

void
dmatrix_rowcat(double **a, const double **b, const double **c,
               size_t bm, size_t cm, size_t n)
{
  for (size_t i = 0; i < bm; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i][j] = b[i][j];
    }
  }
  for (size_t i = 0; i < cm; i++) {
    for (size_t j = 0; j < n; j++) {
      a[bm + i][j] = c[i][j];
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
intmatrix_fprint(FILE * stream, const int **a, size_t m, size_t n)
{
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      fprintf(stream, "%d ", a[i][j]);
    }
    putc('\n', stream);
  }
}

void
fmatrix_fprint(FILE * stream, const float **a, size_t m, size_t n)
{
  char str[64];
  size_t maxlen = 0;

  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      snprintf(str, sizeof(str), "%g", a[i][j]);
      maxlen = MAX2(maxlen, strlen(str));
    }
  }

  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      fprintf(stream, "%*g  ", (int)maxlen, a[i][j]);
    }
    fprintf(stream, "\n");
  }
}

void
dmatrix_fprint(FILE * stream, const double **a, size_t m, size_t n)
{
  char str[64];
  size_t maxlen = 0;

  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      snprintf(str, sizeof(str), "%g", a[i][j]);
      maxlen = MAX2(maxlen, strlen(str));
    }
  }

  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      fprintf(stream, "%*g  ", (int)maxlen, a[i][j]);
    }
    fprintf(stream, "\n");
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_print(const float **a, size_t m, size_t n)
{
  fmatrix_fprint(stdout, a, m, n);
}

void
dmatrix_print(const double **a, size_t m, size_t n)
{
  dmatrix_fprint(stdout, a, m, n);
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_fprintComplex(FILE * stream,
                      const float **re, const float **im, size_t m, size_t n)
{
}

void
dmatrix_fprintComplex(FILE * stream,
                      const double **re, const double **im, size_t m, size_t n)
{
}

/* ---------------------------- Group Separator ---------------------------- */

void
fmatrix_printComplex(const float **re, const float **im, size_t m, size_t n)
{
}

void
dmatrix_printComplex(const double **re, const double **im, size_t m, size_t n)
{
}

/* ---------------------------- Group Separator ---------------------------- */

void
darray_add2(double *a, const double *b, const double *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] + c[i];
  }
}

void
darray_sub2(double *a, const double *b, const double *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] - c[i];
  }
}

void
darray_mul2(double *a, const double *b, const double *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] * c[i];
  }
}

void
darray_div2(double *a, const double *b, const double *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] / c[i];
  }
}

void
darray_min2_dbl(double *a, const double * b, double c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = MIN2(b[i], c);
  }
}

void
darray_max2_dbl(double *a, const double * b, double c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = MAX2(b[i], c);
  }
}

void
darray_pow2(double *a, const double *b, const double *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = pow(b[i], c[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
darray_add2_dbl(double *a, const double *b, double c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] + c;
  }
}

void
darray_sub2_dbl(double *a, const double *b, double c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] - c;
  }
}

void
darray_mul2_dbl(double *a, const double *b, double c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] * c;
  }
}

void
darray_div2_dbl(double *a, const double *b, double c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = b[i] / c;
  }
}

void
darray_pow2_dbl(double *a, const double *b, double c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = pow(b[i], c);
  }
}

void
darray_pow2_double_rev(double *a, const double *b, double c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = pow(c, b[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
darray_neg(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = -b[i];
  }
}

void
darray_abs(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = fabs(b[i]);
  }
}

void
darray_inv(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = 1 / b[i];
  }
}

/* ---------------------------- Group Separator ---------------------------- */

double
darray_sum(const double *a, size_t n)
{
  double sum;
  sum = a[0];
  for (size_t i = 1; i < n; i++) {
    sum += a[i];
  }
  return sum;
}

double
darray_prod(const double *a, size_t n)
{
  double prod;
  prod = a[0];
  for (size_t i = 1; i < n; i++) {
    prod *= a[i];
  }
  return prod;
}

double
darray_min(const double *a, size_t n)
{
  double min;
  min = a[0];
  for (size_t i = 1; i < n; i++) {
    min = MIN2(min, a[i]);
  }
  return min;
}

double
darray_max(const double *a, size_t n)
{
  double max;
  max = a[0];
  for (size_t i = 1; i < n; i++) {
    max = MAX2(max, a[i]);
  }
  return max;
}

/* ---------------------------- Group Separator ---------------------------- */

void
darray_cos(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = cos(b[i]);
  }
}

void
darray_sin(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = sin(b[i]);
  }
}

void
darray_tan(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = tan(b[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
darray_acos(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = acos(b[i]);
  }
}

void
darray_asin(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = asin(b[i]);
  }
}

void
darray_atan(double *a, const double *b, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = atan(b[i]);
  }
}

/* ======================================================================== */

void
darray_eud2D(double *a, const double *b, const double *c, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    a[i] = hypot(b[i], c[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*
 * Zero length arrays should at least be safe.
 */
void
darray_extremes(double *a, size_t n, double *min_ret, double *max_ret)
{
  double min = DBL_MAX, max = DBL_MIN;
  for (size_t j = 0; j < n; j++) {
    min = MIN2(min, a[j]);
    max = MAX2(max, a[j]);
  }
  *min_ret = min;
  *max_ret = max;
}

/* ---------------------------- Group Separator ---------------------------- */

void
darray_fprint(FILE * stream, const double *a, size_t n)
{
  char str[64];
  size_t maxlen = 0;
  for (size_t j = 0; j < n; j++) {
    snprintf(str, sizeof(str), "%g", a[j]);
    maxlen = MAX2(maxlen, strlen(str));
  }
  fprintf(stream, "[ ");
  for (size_t j = 0; j < n; j++) {
    fprintf(stream, "%*g ", (int)maxlen, a[j]);
  }
  fprintf(stream, "]\n");
}

void
ldarray_fprint(FILE * stream, const long double *a, size_t n)
{
  char str[64];
  size_t maxlen = 0;
  for (size_t j = 0; j < n; j++) {
    snprintf(str, sizeof(str), "%Lg", a[j]);
    maxlen = MAX2(maxlen, strlen(str));
  }
  fprintf(stream, "[ ");
  for (size_t j = 0; j < n; j++) {
    fprintf(stream, "%*Lg ", (int)maxlen, a[j]);
  }
  fprintf(stream, "]\n");
}

void
darray_print(const double *a, size_t n)
{
  darray_fprint(stdout, a, n);
}

void
ldarray_print(const long double *a, size_t n)
{
  ldarray_fprint(stdout, a, n);
}

void
darray_fprintComplex(FILE * stream, const double *re, const double *im, size_t n)
{
  for (size_t j = 0; j < n; j++) {
    fprintf(stream, "%g+%gi ", re[j], im[j]);
  }
  fprintf(stream, "\n");
}

void
darray_printComplex(const double *re, const double *im, size_t n)
{
  darray_fprintComplex(stdout, re, im, n);
}

/* ---------------------------- Group Separator ---------------------------- */

size_t
intarray_sepNZs(int * a, size_t n)
{
  size_t i, ni;		/* index, non-zero index */
  for (i = 0, ni = 0; i < n; i++) {
    if (a[i]) {			/* if its non-zero */
      a[ni] = a[i];		/* copy it to the beginning */
      ni++;			/* and step forward */
    }
  }
  return ni;
}

/* ---------------------------- Group Separator ---------------------------- */

void
dmatrix_kronecker(double **out, const double **in1, const double **in2,
                  int w_in1, int h_in1, int w_in2, int h_in2)
{
  for (int i1 = 0; i1 < h_in1; i1++) {
    for (int j1 = 0; j1 < w_in1; j1++) {
      for (int i2 = 0; i2 < h_in2; i2++) {
        for (int j2 = 0; j2 < w_in2; j2++) {
          /* Write access stride of out becomes minimal when the
           * i2- and j2-loops are made the innermost. */
          out[i1 * h_in2 + i2][j1 * w_in2 + j2] = in1[i1][j1] * in2[i2][j2];
        }
      }
    }
  }
}

float get_CPU_clock_speed (void)
{
  /* Read the entire contents of /proc/cpuinfo into the buffer. */
  FILE* fp;
  if ((fp = fopen("/proc/cpuinfo", "rb")) == NULL) {
    return 0;
  }

  char buffer[1024];
  size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
  fclose(fp);

  /* Bail if read failed or if buffer isn't big enough. */
  if (bytes_read == 0 || bytes_read == sizeof(buffer)) {
    return 0;
  }

  buffer[bytes_read] = '\0';	/* set NULL-terminator */

  /* Locate the line that starts with "CPU MHz". */
  char * match = strstr(buffer, "CPU MHz");
  if (match == NULL)
    return 0;

  /* Parse the line to extract the clock speed. */
  float clock_speed;
  if (sscanf(match, "CPU MHz : %f", &clock_speed) == 1) {
    return clock_speed;
  } else {
    return 0;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void test_stack_carray_auto_length_arithmetic(void)
{
  const int x[] = { 1, 2, 3 };
  int sz_x0 = sizeof(x[0]);
  int sz_x = sizeof(x);
  int num = sz_x / sz_x0;
  printf("sz_x0:%d sz_x:%d num:%d\n", sz_x0, sz_x, num);
}

/* ---------------------------- Group Separator ---------------------------- */

#if 0
pure int file_is_binary(const char *path)
{
  int ip;
  char buf[32];
  int i, c, size;
  int strange = 0;

  ip = open(path, O_RDONLY);
  if (ip < 0) die("cannot open file '%s' in read mode.", path);
  size = read(ip, buf, sizeof(buf));
  close(ip);
  if (size < 0)
    return 1;
  if (size >= 7 && locatestring(buf, "!<arch>", MATCH_AT_FIRST))
    return 1;
  for (i = 0; i < size; i++) {
    c = (unsigned char)buf[i];
    if (c <= 8)
      return 1;
    if (c >= 14 && c < 32)
      return 1;
    if (c > 128)
      strange++;

  }
  if (((float)strange)/size > 0.3f)
    return 1;

  return 0;
}
#endif

void ptrarray_shuffle(void ** a, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    size_t j = rand() % n;
    ptr_swap(&a[i], &a[j]);
  }
}
