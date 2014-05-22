#pragma once

#include "utils.h"
#include "bitmtx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  MTX_UFIXP,			/* unsigned fixed point */
  MTX_SFIXP,			/* signed fixed point */
  MTX_FLTP,			/* floating point */
  MTX_AP			/* arbitrary precision integer */
} MTXFMT_t;

typedef struct
{
  int m, n;

  union
  {
    Bitmtx *b;			/* \TODO: Remove redundant m and n. */

    uchar **uc;
    signed char **sc;

    ushort **us;
    signed short **ss;

    ulong **ul;
    signed long **sl;

    float **f;
    double **d;

  } d;				/* data */

  MTXFMT_t fmt:8;		/* format */
  int prc:8;			/* precision */

/*      int max_prc; */
/*      double min, max; */
} Mtx;

void mtx_malloc_d(Mtx * a, int m, int n, MTXFMT_t fmt, int prc);
Mtx *mtx_malloc(int m, int n, MTXFMT_t fmt, int prc);
Mtx *mtx_malloc_bit(int m, int n);
Mtx *mtx_malloc_fltp64(int m, int n);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_free_d(Mtx * a);
void mtx_free(Mtx * a);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_reserve(Mtx * a, int m, int n, MTXFMT_t fmt, int prc);
void mtx_reserve_bit(Mtx * a, int m, int n);
void mtx_reserve_fltp64(Mtx * a, int m, int n);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_convert(Mtx * a, const Mtx * b);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_zeros_1_1(Mtx * a, const Mtx * b);
void mtx_zeros_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_zeros(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_ones_1_1(Mtx * a, const Mtx * b);
void mtx_ones(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_eye_1_1(Mtx * a, const Mtx * b);
void mtx_eye(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_rand_1_1(Mtx * a, const Mtx * b);
void mtx_rand(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_diag_1_1(Mtx * a, const Mtx * b);
void mtx_diag(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_ramp_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_ramp_1_3(Mtx * a, const Mtx * b, const Mtx * c, const Mtx * d);
void mtx_ramp(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_add_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_add(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_sub_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_sub(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_mul_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_mul(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_div_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_div(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_rdiv_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_rdiv(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_pow_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_pow(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_pw_mul_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_pw_mul(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_pw_div_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_pw_div(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_pw_pow_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_pw_pow(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_colcat_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_colcat(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_rowcat_1_2(Mtx * a, const Mtx * b, const Mtx * c);
void mtx_rowcat(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_neg_1_1(Mtx * a, const Mtx * b);
void mtx_neg(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_cos_1_1(Mtx * a, const Mtx * b);
void mtx_cos(Mtx ** outs, int outN, Mtx ** ins, int inN);

void mtx_sin_1_1(Mtx * a, const Mtx * b);
void mtx_sin(Mtx ** outs, int outN, Mtx ** ins, int inN);

void mtx_tan_1_1(Mtx * a, const Mtx * b);
void mtx_tan(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_acos_1_1(Mtx * a, const Mtx * b);
void mtx_acos(Mtx ** outs, int outN, Mtx ** ins, int inN);

void mtx_asin_1_1(Mtx * a, const Mtx * b);
void mtx_asin(Mtx ** outs, int outN, Mtx ** ins, int inN);

void mtx_atan_1_1(Mtx * a, const Mtx * b);
void mtx_atan(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_nctransp_1_1(Mtx * a, const Mtx * b);
void mtx_nctransp(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_sort_1_1(Mtx * a, const Mtx * b);
void mtx_sort(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_inv_1_1(Mtx * a, const Mtx * b);
void mtx_inv(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_fft_1_1(Mtx * a, const Mtx * b);
void mtx_fft(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_sum_1_1(Mtx * a, const Mtx * b);
void mtx_sum(Mtx ** outs, int outN, Mtx ** ins, int inN);

void mtx_prod_1_1(Mtx * a, const Mtx * b);
void mtx_prod(Mtx ** outs, int outN, Mtx ** ins, int inN);

void mtx_min_1_1(Mtx * a, const Mtx * b);
void mtx_min(Mtx ** outs, int outN, Mtx ** ins, int inN);

void mtx_max_1_1(Mtx * a, const Mtx * b);
void mtx_max(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_fprint(FILE * stream, Mtx * a);

void mtx_print_0_1(Mtx * a);
void mtx_print(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ---------------------------- Group Separator ---------------------------- */

void mtx_tic_0_0(void);
void mtx_tic(Mtx ** outs, int outN, Mtx ** ins, int inN);

void mtx_toc_1_0(Mtx * a);
void mtx_toc(Mtx ** outs, int outN, Mtx ** ins, int inN);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
