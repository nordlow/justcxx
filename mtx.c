#include "mtx.h"

#include "timing.h"
#include "mul.h"
#include "transpose.h"
#include "qsort.h"
#include "inv.h"
#include "meman.h"
#include "rangerand.h"
#include "extremes.h"

#include <stdlib.h>
#include <math.h>

static void
mtx_fmt_err(const char *fun_name)
{
  leprintf("%s: Format error.\n", fun_name);
}

static void
mtx_prc_err(const char *fun_name)
{
  leprintf("%s: Precision error.\n", fun_name);
}

static void
mtx_fltp64_err(const char *fun_name)
{
  leprintf("%s: Not fltp64 error.\n", fun_name);
}

static void
mtx_dim_err(const char *fun_name)
{
  leprintf("%s: Dimension error.\n", fun_name);
}

static void
mtx_narg_err(const char *fun_name)
{
  leprintf("%s: Invalid number of out or/and in arguments.\n", fun_name);
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_malloc_d(Mtx * a, int m, int n, MTXFMT_t fmt, int prc)
{
  switch (a->fmt) {
  case MTX_UFIXP:
    switch (a->prc) {
    case 1:
      a->d.b = bitmtx_new(m, n);
      break;
    default:
      mtx_fmt_err(__FUNCTION__);
      break;
    }
    break;
  case MTX_SFIXP:
    break;
  case MTX_FLTP:
    switch (a->prc) {
    case 64:
      a->d.d = doubleAA_malloc(m, n);
      break;
    default:
      mtx_fmt_err(__FUNCTION__);
      break;
    }
    break;
  case MTX_AP:
    break;
  default:
    mtx_prc_err(__FUNCTION__);
    break;
  }
}

Mtx *
mtx_malloc(int m, int n, MTXFMT_t fmt, int prc)
{
  Mtx *a;
  a = calloc(sizeof(Mtx), 1);
  a->m = m;
  a->n = n;
  a->fmt = fmt;
  a->prc = prc;
  mtx_malloc_d(a, m, n, fmt, prc);
  return a;
}

Mtx *
mtx_malloc_bit(int m, int n)
{
  return mtx_malloc(m, n, MTX_UFIXP, 1);
}

Mtx *
mtx_malloc_fltp64(int m, int n)
{
  return mtx_malloc(m, n, MTX_FLTP, 64);
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_free_d(Mtx * a)
{
  switch (a->fmt) {
  case MTX_UFIXP:
    switch (a->prc) {
    case 1:
      bitmtx_delete(a->d.b);
      break;
    default:
      mtx_fmt_err(__FUNCTION__);
      break;
    }
    break;
  case MTX_SFIXP:
    break;
  case MTX_FLTP:
    switch (a->prc) {
    case 64:
      dmatrix_free(a->d.d);
      break;
    default:
      mtx_fmt_err(__FUNCTION__);
      break;
    }
    break;
  case MTX_AP:
    break;
  default:
    mtx_prc_err(__FUNCTION__);
    break;
  }
}

void
mtx_free(Mtx * a)
{
  mtx_free_d(a);
  free(a);
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_reserve(Mtx * a, int m, int n, MTXFMT_t fmt, int prc)
{
  if (a->m == m && a->n == n) {
    return;
  }
  mtx_free_d(a);
  a->m = m;
  a->n = n;
  a->fmt = fmt;
  a->prc = prc;
  mtx_malloc_d(a, m, n, fmt, prc);
}

void
mtx_reserve_bit(Mtx * a, int m, int n)
{
  mtx_reserve(a, m, n, MTX_UFIXP, 1);
}

void
mtx_reserve_fltp64(Mtx * a, int m, int n)
{
  mtx_reserve(a, m, n, MTX_FLTP, 64);
}

/* ---------------------------- Group Separator ---------------------------- */

int
mtx_eq_size(const Mtx * a, const Mtx * b)
{
  return ((a->m == b->m) && (a->n == b->n));
}

int
mtx_is_scl(const Mtx * a)
{
  return (a->m == 1 && a->n == 1);
}

int
mtx_is_fltp64(const Mtx * a)
{
  return (a->fmt == MTX_FLTP && a->prc == 64);
}

int
mtx_is_row_vec(const Mtx * a)
{
  return (a->m == 1);
}

int
mtx_is_col_vec(const Mtx * a)
{
  return (a->n == 1);
}

int
mtx_is_vector(const Mtx * a)
{
  return (a->m == 1 || a->n == 1);
}

int
mtx_is_square(const Mtx * a)
{
  return (a->m == a->n);
}

int
mtx_is_affine3d(const Mtx * a)
{
  return (a->m == 4 &&
	  a->n == 4 &&
	  a->d.d[0][3] == 0 &&
	  a->d.d[1][3] == 0 && a->d.d[2][3] == 0 && a->d.d[3][3] == 1);
}

/* ---------------------------- Group Separator ---------------------------- */

double
double_convert_mtx(const Mtx * a)
{
  double d = 0;
  if (!mtx_is_scl(a)) {
    mtx_dim_err(__FUNCTION__);
  }
  switch (a->fmt) {
  case MTX_UFIXP:
    switch (a->prc) {
    case 1:
      d = bitmtx_atIJ(a->d.b, 0, 0);
      break;
    case 8:
      d = a->d.uc[0][0];
      break;
    case 16:
      d = a->d.us[0][0];
      break;
    case 32:
      d = a->d.ul[0][0];
      break;
    default:
      mtx_fmt_err(__FUNCTION__);
      break;
    }
    break;
  case MTX_SFIXP:
    switch (a->prc) {
    case 8:
      d = a->d.sc[0][0];
      break;
    case 16:
      d = a->d.ss[0][0];
      break;
    case 32:
      d = a->d.sl[0][0];
      break;
    default:
      mtx_fmt_err(__FUNCTION__);
      break;
    }
    break;
  case MTX_FLTP:
    switch (a->prc) {
    case 64:
      d = a->d.d[0][0];
      break;
    default:
      mtx_fmt_err(__FUNCTION__);
      break;
    }
    break;
  case MTX_AP:
    break;
  default:
    mtx_prc_err(__FUNCTION__);
    break;
  }
  return d;
}

float
float_convert_mtx(const Mtx * a)
{
  return double_convert_mtx(a);
}

int
int_convert_mtx(const Mtx * a)
{
  return F_ROUND(double_convert_mtx(a));
}

void
mtx_convert(Mtx * a, const Mtx * b)
{
  if (!mtx_eq_size(a, b)) {
    mtx_dim_err(__FUNCTION__);
  }

  /* Use a lookuptable to convert
   * a->m,n,fmt,prc and b->m,n,fmt,prc into a C-fun pointer:
   * and call it as a fun (a->d[0], b->d[0], m*n); */
  if (mtx_is_vector(a)) {
  } else {
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_zeros_1_1(Mtx * a, const Mtx * b)
{
  if (mtx_is_scl(b)) {
    int m, n;
    m = n = int_convert_mtx(b);
    mtx_reserve_bit(a, m, n);
    bitmtx_set0all(a->d.b);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_zeros_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (mtx_is_scl(b) && mtx_is_scl(c)) {
    int m, n;
    m = int_convert_mtx(b);
    n = int_convert_mtx(c);
    mtx_reserve_bit(a, m, n);
    bitmtx_set0all(a->d.b);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_zeros(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_zeros_1_1(outs[0], ins[0]);
  } else if (outN == 1 && inN == 2) {
    mtx_zeros_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_ones_1_1(Mtx * a, const Mtx * b)
{
  if (mtx_is_scl(b)) {
    int m, n;
    m = n = int_convert_mtx(b);
    mtx_reserve_bit(a, m, n);
    bitmtx_set1all(a->d.b);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_ones_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (mtx_is_scl(b) && mtx_is_scl(c)) {
    int m, n;
    m = int_convert_mtx(b);
    n = int_convert_mtx(c);
    mtx_reserve_bit(a, m, n);
    bitmtx_set1all(a->d.b);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_ones(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_ones_1_1(outs[0], ins[0]);
  } else if (outN == 1 && inN == 2) {
    mtx_ones_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_eye_1_1(Mtx * a, const Mtx * b)
{
  if (mtx_is_scl(b)) {
    int m, n;
    m = n = int_convert_mtx(b);
    mtx_reserve_fltp64(a, m, n);
    dmatrix_eye(a->d.d, m, n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_eye_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (mtx_is_scl(b) && mtx_is_scl(c)) {
    int m, n;
    m = int_convert_mtx(b);
    n = int_convert_mtx(c);
    mtx_reserve_fltp64(a, m, n);
    dmatrix_eye(a->d.d, m, n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_eye(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_eye_1_1(outs[0], ins[0]);
  } else if (outN == 1 && inN == 2) {
    mtx_eye_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_rand_1_1(Mtx * a, const Mtx * b)
{
  if (mtx_is_scl(b)) {
    int m, n;
    m = n = int_convert_mtx(b);
    mtx_reserve_fltp64(a, m, n);
    darray_rangerand(a->d.d[0], m * n, 0, 1);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_rand_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (mtx_is_scl(b) && mtx_is_scl(c)) {
    int m, n;
    m = int_convert_mtx(b);
    n = int_convert_mtx(c);
    mtx_reserve_fltp64(a, m, n);
    darray_rangerand(a->d.d[0], m * n, 0, 1);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_rand(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_rand_1_1(outs[0], ins[0]);
  } else if (outN == 1 && inN == 2) {
    mtx_rand_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_diag_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_is_vector(b)) {
    int m, n;
    m = n = MAX2(b->m, b->n);
    mtx_reserve_fltp64(a, m, n);
    dmatrix_diag(a->d.d, b->d.d[0], m, n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_diag(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_diag_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_ramp_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  int m, n;
  double min, max, diff, step;

  /* \TODO: Remove, create double_convert_mtx and insert in this function
   * instead. */
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }

  if (!(b->m == 1 && b->n == 1 && c->m == 1 && c->n == 1)) {
    mtx_dim_err(__FUNCTION__);
    return;
  }

  min = b->d.d[0][0];
  max = c->d.d[0][0];
  diff = max - min;
  step = diff >= 0 ? 1 : -1;

  if (diff * step < 0) {
    m = 1;			/* \TODO: Or zero, like in Octave 2.0.16? */
    n = 0;
  } else {
    m = 1;
    n = fabs(floor(diff / step)) + 1;
  }
  mtx_reserve_fltp64(a, m, n);
  darray_ramp(a->d.d[0], n, min, step);
}

void
mtx_ramp_1_3(Mtx * a, const Mtx * b, const Mtx * c, const Mtx * d)
{
  int m, n;
  double min, max, diff, step;

  /* \TODO: Remove, create double_convert_mtx and insert in this function
   * instead. */
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c) && mtx_is_fltp64(d))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }

  if (!(b->m == 1 && b->n == 1 &&
	c->m == 1 && c->n == 1 && d->m == 1 && d->n == 1)) {
    mtx_dim_err(__FUNCTION__);
    return;
  }

  min = b->d.d[0][0];
  max = d->d.d[0][0];
  diff = max - min;
  step = c->d.d[0][0];

  if (diff * step < 0) {
    m = 1;			/* \TODO: Or zero, like in Octave 2.0.16? */
    n = 0;
  } else {
    m = 1;
    n = fabs(floor(diff / step)) + 1;
  }
  mtx_reserve_fltp64(a, m, n);
  darray_ramp(a->d.d[0], n, min, step);
}

void
mtx_ramp(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_ramp_1_2(outs[0], ins[0], ins[1]);
  } else if (outN == 1 && inN == 3) {
    mtx_ramp_1_3(outs[0], ins[0], ins[1], ins[2]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_add_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_eq_size(b, c)) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_add2(a->d.d[0], b->d.d[0], c->d.d[0], b->m * b->n);
  } else if (b->m == 1 && b->n == 1) {
    mtx_reserve_fltp64(a, c->m, c->n);
    darray_add2_dbl(a->d.d[0], c->d.d[0], b->d.d[0][0], c->m * c->n);
  } else if (c->m == 1 && c->n == 1) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_add2_dbl(a->d.d[0], b->d.d[0], c->d.d[0][0], b->m * b->n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_add(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_add_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_sub_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_eq_size(b, c)) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_sub2(a->d.d[0], b->d.d[0], c->d.d[0], b->m * b->n);
  } else if (b->m == 1 && b->n == 1) {
    mtx_reserve_fltp64(a, c->m, c->n);
    darray_sub2_dbl(a->d.d[0], c->d.d[0], b->d.d[0][0], c->m * c->n);
  } else if (c->m == 1 && c->n == 1) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_sub2_dbl(a->d.d[0], b->d.d[0], c->d.d[0][0], b->m * b->n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_sub(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_sub_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_mul_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (b->n == c->m) {
    mtx_reserve_fltp64(a, b->m, c->n);
    dmatrix_mul_orig(a->d.d, b->d.d, c->d.d, b->m, b->n, c->n);
  } else if (b->m == 1 && b->n == 1) {
    mtx_reserve_fltp64(a, c->m, c->n);
    darray_mul2_dbl(a->d.d[0], c->d.d[0], b->d.d[0][0], c->m * c->n);
  } else if (c->m == 1 && c->n == 1) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_mul2_dbl(a->d.d[0], b->d.d[0], c->d.d[0][0], b->m * b->n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_mul(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_mul_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_div_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  /* \TODO: Things todo here. */
  if (c->m == 1 && c->n == 1) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_div2_dbl(a->d.d[0], b->d.d[0], c->d.d[0][0], b->m * b->n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_div(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_div_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_rdiv_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_pw_mul_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_eq_size(b, c)) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_mul2(a->d.d[0], b->d.d[0], c->d.d[0], b->m * b->n);
  } else if (b->m == 1 && b->n == 1) {
    mtx_reserve_fltp64(a, c->m, c->n);
    darray_mul2_dbl(a->d.d[0], c->d.d[0], b->d.d[0][0], c->m * c->n);
  } else if (c->m == 1 && c->n == 1) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_mul2_dbl(a->d.d[0], b->d.d[0], c->d.d[0][0], b->m * b->n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_pw_mul(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_pw_mul_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_pw_div_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_eq_size(b, c)) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_div2(a->d.d[0], b->d.d[0], c->d.d[0], b->m * b->n);
  } else if (b->m == 1 && b->n == 1) {
    mtx_reserve_fltp64(a, c->m, c->n);
    darray_div2_dbl(a->d.d[0], c->d.d[0], b->d.d[0][0], c->m * c->n);
  } else if (c->m == 1 && c->n == 1) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_div2_dbl(a->d.d[0], b->d.d[0], c->d.d[0][0], b->m * b->n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_pw_div(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_pw_div_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_pw_pow_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_eq_size(b, c)) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_pow2(a->d.d[0], b->d.d[0], c->d.d[0], b->m * b->n);
  } else if (b->m == 1 && b->n == 1) {
    mtx_reserve_fltp64(a, c->m, c->n);
    darray_pow2_double_rev(a->d.d[0], c->d.d[0], b->d.d[0][0], c->m * c->n);
  } else if (c->m == 1 && c->n == 1) {
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_pow2_dbl(a->d.d[0], b->d.d[0], c->d.d[0][0], b->m * b->n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_pw_pow(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_pw_pow_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_colcat_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if ((b->m == c->m)) {
    mtx_reserve_fltp64(a, b->m, b->n + c->n);
    dmatrix_colcat(a->d.d, (const double **) b->d.d, (const double **) c->d.d,
		b->m, b->n, c->n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_colcat(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_colcat_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_rowcat_1_2(Mtx * a, const Mtx * b, const Mtx * c)
{
  if (!(mtx_is_fltp64(b) && mtx_is_fltp64(c))) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if ((b->n == c->n)) {
    mtx_reserve_fltp64(a, b->m + c->m, b->n);
    dmatrix_rowcat(a->d.d, (const double **) b->d.d, (const double **) c->d.d,
		b->m, c->m, b->n);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_rowcat(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 2) {
    mtx_rowcat_1_2(outs[0], ins[0], ins[1]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_neg_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  mtx_reserve_fltp64(a, b->m, b->n);
  darray_neg(a->d.d[0], b->d.d[0], b->m * b->n);
}

void
mtx_neg(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_neg_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_cos_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  mtx_reserve_fltp64(a, b->m, b->n);
  darray_cos(a->d.d[0], b->d.d[0], b->m * b->n);
}

void
mtx_cos(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_cos_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

void
mtx_sin_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  mtx_reserve_fltp64(a, b->m, b->n);
  darray_sin(a->d.d[0], b->d.d[0], b->m * b->n);
}

void
mtx_sin(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_sin_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

void
mtx_tan_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  mtx_reserve_fltp64(a, b->m, b->n);
  darray_tan(a->d.d[0], b->d.d[0], b->m * b->n);
}

void
mtx_tan(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_tan_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_acos_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  mtx_reserve_fltp64(a, b->m, b->n);
  darray_acos(a->d.d[0], b->d.d[0], b->m * b->n);
}

void
mtx_acos(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_acos_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

void
mtx_asin_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  mtx_reserve_fltp64(a, b->m, b->n);
  darray_asin(a->d.d[0], b->d.d[0], b->m * b->n);
}

void
mtx_asin(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_asin_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

void
mtx_atan_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  mtx_reserve_fltp64(a, b->m, b->n);
  darray_atan(a->d.d[0], b->d.d[0], b->m * b->n);
}

void
mtx_atan(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_atan_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_nctransp_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  mtx_reserve_fltp64(a, b->n, b->m);
  dmatrix_transpose(a->d.d, b->d.d, b->m, b->n);
}

void
mtx_nctransp(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_nctransp_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_sort_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_is_vector(b)) {
    int l;
    l = MAX2(b->m, b->n);
    mtx_reserve_fltp64(a, b->m, b->n);
    darray_copy(a->d.d[0], b->d.d[0], l);
    darray_qsort(a->d.d[0], l);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_sort(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_sort_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_inv_affine4x4_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  mtx_reserve_fltp64(a, b->n, b->m);
  darray_aff4x4inv((double*) a->d.d[0], (const double*) b->d.d[0]);
}

void
mtx_inv_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_is_square(b)) {
    mtx_reserve_fltp64(a, b->n, b->m);
    dmatrix_gjinv(a->d.d, b->d.d, b->m);
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_inv(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    if (mtx_is_affine3d(ins[0])) {
      mtx_inv_affine4x4_1_1(outs[0], ins[0]);
    } else {
      mtx_inv_1_1(outs[0], ins[0]);
    }
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_fft_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_is_vector(b)) {
    mtx_reserve_fltp64(a, b->n, b->m);
/*          dmatrix_fft (a->d.d[0], b->d.d[0], b->m); */
  } else {
    mtx_dim_err(__FUNCTION__);
  }
}

void
mtx_fft(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_fft_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_sum_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_is_vector(b)) {
    mtx_reserve_fltp64(a, 1, 1);
    a->d.d[0][0] = darray_sum(b->d.d[0], b->m * b->n);
  } else {
    int i, j;
    mtx_reserve_fltp64(a, 1, b->n);
    /* \TODO: Move elsewhere? */
    for (j = 0; j < b->n; j++) {
      double sum;
      sum = 0;
      for (i = 0; i < b->m; i++) {
	sum += b->d.d[i][j];
      }
      a->d.d[0][j] = sum;
    }
  }
}

void
mtx_sum(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_sum_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_prod_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_is_vector(b)) {
    mtx_reserve_fltp64(a, 1, 1);
    a->d.d[0][0] = darray_prod(b->d.d[0], b->m * b->n);
  } else {
    int i, j;
    mtx_reserve_fltp64(a, 1, b->n);
    /* \TODO: Move elsewhere? */
    for (j = 0; j < b->n; j++) {
      double prod;
      prod = 1;
      for (i = 0; i < b->m; i++) {
	prod *= b->d.d[i][j];
      }
      a->d.d[0][j] = prod;
    }
  }
}

void
mtx_prod(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_prod_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_min_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_is_vector(b)) {
    mtx_reserve_fltp64(a, 1, 1);
    a->d.d[0][0] = darray_min(b->d.d[0], b->m * b->n);
  } else {
    int i, j;
    mtx_reserve_fltp64(a, 1, b->n);
    /* \TODO: Move elsewhere? */
    for (j = 0; j < b->n; j++) {
      double min;
      min = b->d.d[0][j];
      for (i = 0; i < b->m; i++) {
	min = MIN2(min, b->d.d[i][j]);
      }
      a->d.d[0][j] = min;
    }
  }
}

void
mtx_min(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_min_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_max_1_1(Mtx * a, const Mtx * b)
{
  if (!mtx_is_fltp64(b)) {
    mtx_fltp64_err(__FUNCTION__);
    return;
  }
  if (mtx_is_vector(b)) {
    mtx_reserve_fltp64(a, 1, 1);
    a->d.d[0][0] = darray_max(b->d.d[0], b->m * b->n);
  } else {
    int i, j;
    mtx_reserve_fltp64(a, 1, b->n);
    /* \TODO: Move elsewhere? */
    for (j = 0; j < b->n; j++) {
      double max;
      max = b->d.d[0][j];
      for (i = 0; i < b->m; i++) {
	max = MAX2(max, b->d.d[i][j]);
      }
      a->d.d[0][j] = max;
    }
  }
}

void
mtx_max(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 1) {
    mtx_max_1_1(outs[0], ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
mtx_fprint(FILE * stream, Mtx * a)
{
  const char *fmt_names[] = { "unsigned fixed point",
    "signed fixed point",
    "floating point",
    "arbitrary precision",
  };

  if (a->fmt > MTX_AP) {
    leprintf("unknown format\n");
  }
  fprintf(stream, "ans (%d x %d of %s with precision %d) = \n",
	  a->m, a->n, fmt_names[a->fmt], a->prc);

  switch (a->fmt) {
  case MTX_UFIXP:
    switch (a->prc) {
    case 1:
      bitmtx_fprint(stream, (Bitmtx * const) a->d.b);
      break;
    default:
      mtx_fmt_err(__FUNCTION__);
      break;
    }
    break;
  case MTX_SFIXP:
    break;
  case MTX_FLTP:
    switch (a->prc) {
    case 64:
      dmatrix_fprint(stream, (const double **const) a->d.d, a->m, a->n);
      break;
    default:
      mtx_fmt_err(__FUNCTION__);
      break;
    }
    break;
  case MTX_AP:
    break;
  default:
    mtx_prc_err(__FUNCTION__);
    break;
  }

}

void
mtx_print_0_1(Mtx * a)
{
  mtx_fprint(stdout, a);
}

void
mtx_print(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 0 && inN == 1) {
    mtx_print_0_1(ins[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/* \TODO: Not threading safe. */
static pTimer tmr;

void
mtx_tic_0_0(void)
{
  ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);
  ptimer_tic(&tmr);
}

void
mtx_tic(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 0 && inN == 0) {
    mtx_tic_0_0();
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}

void
mtx_toc_1_0(Mtx * a)
{
  ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);
  ptimer_toc(&tmr);
  mtx_reserve_fltp64(a, 1, 1);
  a->d.d[0][0] = ptimer_get_sec(&tmr);
}

void
mtx_toc(Mtx ** outs, int outN, Mtx ** ins, int inN)
{
  if (outN == 1 && inN == 0) {
    mtx_toc_1_0(outs[0]);
  } else {
    mtx_narg_err(__FUNCTION__);
  }
}
