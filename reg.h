#pragma once

#include "utils.h"
/*  #include "node.h" */
#include "mtx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/* Native Object Information */

enum
{
  NOI_MTX,
  NOI_MTXFUN,
  NOI_EXECUTOR
};

typedef struct
{
  int type;
  void *value;
} noi_t;

/* ---------------------------- Group Separator ---------------------------- */

/* Native Function Information */

typedef struct
{
  char *name;
/*      void (*cfun)(const void *, const void *)); */
/*      void (*fun)(); */
  void (*fun) (Mtx **, int, Mtx **, int);
  int outN, inN;
  int cat;
  int row_nbh, col_nbh;		/* row, column neighbourhood */
} FInfo;

void nfi_exe0(FInfo * nfi);
void nfi_exe1(FInfo * nfi, Mtx * a);
void nfi_exe2(FInfo * nfi, Mtx * a, Mtx * b);
void nfi_exe3(FInfo * nfi, Mtx * a, Mtx * b, Mtx * c);
void nfi_exe4(FInfo * nfi, Mtx * a, Mtx * b, Mtx * c, Mtx * d);

/* ---------------------------- Group Separator ---------------------------- */

/* Native Object Register */

typedef struct
{
  FInfo *r;
  int l;
} GReg;

void reg_init(GReg * reg);
void reg_clear(GReg * reg);

void reg_add_mtxfun(GReg * reg, const char *name, void *fun, int outN, int inN);

void reg_add_std(GReg * reg);

FInfo *reg_lookup_nfi(const GReg * reg, char *name);

void greg_print(const GReg * reg);

/* ---------------------------- Group Separator ---------------------------- */

/* Native Function Reference */

typedef struct
{
  FInfo *nfi;
  char *name;			/* used before lookup */
  int cnt;			/* reference count */
} nref_t;

/* ---------------------------- Group Separator ---------------------------- */

/* Super Function Information */

typedef struct
{
  int dummy;
/*     fun_t *fun; */
} sfi_t;

/* ---------------------------- Group Separator ---------------------------- */

/* Super Function Reference */

typedef struct
{
  sfi_t *ref;
  int cnt;			/* reference count */
} sfiref_t;

/* ---------------------------- Group Separator ---------------------------- */

/* MaTrix Information */

typedef struct
{
  int mod_tag;			/* modification tag */
} mti_t;

/* ---------------------------- Group Separator ---------------------------- */

/* MaTrix Reference */

typedef struct
{
  mti_t *ref;

  int rd:1;			/* does read */
  int wr:1;			/* does write */

  int mod_tag;			/* modification tag */
} mtxref_t;

/* ---------------------------- Group Separator ---------------------------- */

#ifdef __cplusplus
}
#endif
