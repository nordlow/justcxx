/*! \file t_gjsolve.c
 * \brief Tesing Gauss-Jordan matrix inversion.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include "utils.h"
#include "inv.h"
#include "meman.h"
#include "rangerand.h"

int
main(int argc, char *argv[])
{
  int len = 4;

  double **a = doubleAA_malloc(len, len);
  double **b = doubleAA_malloc(len, len);

  double *vec = darray_malloc(len);

  darray_rangerand(vec, len, 0, 1);
  dmatrix_diag(a, vec, len, len);

  darray_rangerand(vec, len, 0, 1);
  dmatrix_diag(b, vec, len, len);

  dmatrix_print((const double **) a, len, len);
  printf("\n");
  dmatrix_print((const double **) b, len, len);
  printf("\n");

  dmatrix_gjsolve(a, b, len, len);

  dmatrix_print((const double **) a, len, len);
  printf("\n");
  dmatrix_print((const double **) b, len, len);
  printf("\n");

  dmatrix_free(a);
  dmatrix_free(b);

  return 0;
}
