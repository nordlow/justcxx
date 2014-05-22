#include "reg.h"
#include "mtx.h"

#include <stdlib.h>
#include <string.h>

void
reg_init(GReg * reg)
{
  reg->r = NULL;
  reg->l = 0;
}

void
reg_clear(GReg * reg)
{
  free(reg->r);
  reg->l = 0;
}

void
reg_add_mtxfun(GReg * reg, const char *name, void *fun, int outN, int inN)
{
  reg->l++;
  reg->r = realloc(reg->r, reg->l * sizeof(FInfo));
  reg->r[reg->l - 1].name = strdup(name);
  reg->r[reg->l - 1].fun = fun;
  reg->r[reg->l - 1].outN = outN;
  reg->r[reg->l - 1].inN = inN;
  reg->r[reg->l - 1].cat = NOI_MTXFUN;
}

void
reg_add_std(GReg * reg)
{
  reg_add_mtxfun(reg, "pi", NULL, 1, 1);

  reg_add_mtxfun(reg, "neg", mtx_neg, 1, 1);

  reg_add_mtxfun(reg, "cos", mtx_cos, 1, 1);
  reg_add_mtxfun(reg, "sin", mtx_sin, 1, 1);
  reg_add_mtxfun(reg, "tan", mtx_tan, 1, 1);

  reg_add_mtxfun(reg, "acos", mtx_acos, 1, 1);
  reg_add_mtxfun(reg, "asin", mtx_asin, 1, 1);
  reg_add_mtxfun(reg, "atan", mtx_atan, 1, 1);

  reg_add_mtxfun(reg, "nctransp", mtx_nctransp, 1, 1);
  reg_add_mtxfun(reg, "transp", mtx_nctransp, 1, 1);

  reg_add_mtxfun(reg, "sort", mtx_sort, 1, 1);
  reg_add_mtxfun(reg, "inv", mtx_inv, 1, 1);

  reg_add_mtxfun(reg, "sum", mtx_sum, 1, 1);
  reg_add_mtxfun(reg, "prod", mtx_prod, 1, 1);

  reg_add_mtxfun(reg, "min", mtx_min, 1, 1);
  reg_add_mtxfun(reg, "max", mtx_max, 1, 1);

  reg_add_mtxfun(reg, "add", mtx_add, 1, 2);
  reg_add_mtxfun(reg, "sub", mtx_sub, 1, 2);

  reg_add_mtxfun(reg, "mul", mtx_mul, 1, 2);
  reg_add_mtxfun(reg, "div", mtx_div, 1, 2);

  reg_add_mtxfun(reg, "rowcat", mtx_rowcat, 1, 2);
  reg_add_mtxfun(reg, "colcat", mtx_colcat, 1, 2);

  reg_add_mtxfun(reg, "pw_mul", mtx_pw_mul, 1, 2);
  reg_add_mtxfun(reg, "pw_div", mtx_pw_div, 1, 2);
  reg_add_mtxfun(reg, "pw_pow", mtx_pw_pow, 1, 2);

  reg_add_mtxfun(reg, "ramp", mtx_ramp, 1, 3);

  reg_add_mtxfun(reg, "zeros", mtx_zeros, 1, 1);
  reg_add_mtxfun(reg, "ones", mtx_ones, 1, 1);
  reg_add_mtxfun(reg, "eye", mtx_eye, 1, 1);
  reg_add_mtxfun(reg, "rand", mtx_rand, 1, 1);
  reg_add_mtxfun(reg, "diag", mtx_diag, 1, 1);

  reg_add_mtxfun(reg, "print", mtx_print, 0, 1);

  reg_add_mtxfun(reg, "tic", mtx_tic, 0, 0);
  reg_add_mtxfun(reg, "toc", mtx_toc, 1, 0);

  /* \TODO: Fix what follows: ... */

  reg_add_mtxfun(reg, "progn", NULL, 0, 0);
  reg_add_mtxfun(reg, "concurrent", NULL, 0, 0);

}

/*!
 * \NOTE Used
 * the GNU MP test program "primes" to get these numbers and
 * the sh-utils program "factor" to verify them.
 */
const static size_t pow2_primes[sizeof(size_t) * 8] = {

/*  2, 4, 8, 16, 32, 64, 128, 256, */
  2, 3, 7, 13, 31, 61, 127, 251,

/*  512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, */
  509, 1021, 2039, 4093, 8191, 16381, 32749, 65521,

/*  131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, */
  131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593, 16777213,

/*  33554432, 67108864, 134217728, 268435456, */
  33554393, 67108859, 134217689, 268435399,

/*  536870912, 1073741824, 2147483648, 4294967296, */
  536870909, 1073741789, 2147483647, 4294967291UL
};

FInfo *
reg_hash_lookup_nfi(const GReg * reg, char *name)
{
  uint p = pow2_primes[10];
  uint key = strlen(name);

  uint i = 0;
  do {
    i = (i + key) % p;
    /* Check for hit (return it), miss (return NULL) or next (loop on). */
  } while (i != 0);

  return NULL;
}

FInfo *
reg_lookup_nfi(const GReg * reg, char *name)
{
  for (int i = 0; i < reg->l; i++) {
    if (strcmp(reg->r[i].name, name) == 0) {
      return reg->r + i;
    }
  }
  return NULL;
}

void
greg_print(const GReg * reg)
{
  for (int i = 0; i < reg->l; i++) {
    printf("name: %s  fun: %p  outN: %d  inN: %d"
	   "  cat: %d"
	   "  row_nbh: %d  col_nbh: %d\n",
	   reg->r[i].name,
	   reg->r[i].fun,
	   reg->r[i].outN,
	   reg->r[i].inN,
	   reg->r[i].cat, reg->r[i].row_nbh, reg->r[i].col_nbh);
  }
}

#if 0
void
nfi_exe1(FInfo * nfi, Mtx * a)
{
  void (*fun) (Mtx *);
  if (!(nfi && nfi->fun && a)) {
    leprintf("Some arguments where NULL!\n");
    return;
  }
  printf("%s: Calling nref %s\n", __FUNCTION__, nfi->name);
  fun = nfi->fun;
  fun(a);
}

void
nfi_exe2(FInfo * nfi, Mtx * a, Mtx * b)
{
  void (*fun) (Mtx *, Mtx *);
  if (!(nfi && nfi->fun && a && b)) {
    leprintf("Some arguments where NULL!\n");
    return;
  }
  printf("%s: Calling nref %s\n", __FUNCTION__, nfi->name);
  fun = nfi->fun;
  fun(a, b);
}

void
nfi_exe3(FInfo * nfi, Mtx * a, Mtx * b, Mtx * c)
{
  void (*fun) (Mtx *, Mtx *, Mtx *);
  if (!(nfi && nfi->fun && a && b && c)) {
    leprintf("Some arguments where NULL!\n");
    return;
  }
  printf("%s: Calling nref %s\n", __FUNCTION__, nfi->name);
  fun = nfi->fun;
  fun(a, b, c);
}

void
nfi_exe4(FInfo * nfi, Mtx * a, Mtx * b, Mtx * c, Mtx * d)
{
  void (*fun) (Mtx *, Mtx *, Mtx *, Mtx *);
  if (!(nfi && nfi->fun && a && b && c && d)) {
    leprintf("Some arguments where NULL!\n");
    return;
  }
  printf("%s: Calling nref %s\n", __FUNCTION__, nfi->name);
  fun = nfi->fun;
  fun(a, b, c, d);
}
#endif
