/*!
 * \file t_mul.c
 * \brief Tests multiplication algorithms.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include "../utils.h"
#include "../mul.h"
#include "../timing.h"
#include "../meman.h"

void
test_dmatrix_power_uint(int n, uint p, int print)
{
  double **out, **in;
  pTimer t; ptimer_init(&t, CLOCK_PROCESS_CPUTIME_ID);

  in = doubleAA_malloc(n, n);
  dmatrix_ramp(in, n, n, 0.0, 2 * n, 1.0);

  if (print) {
    dmatrix_print((const double **) in, n, n);
    puts("");
  }

  out = doubleAA_malloc(n, n);

  ptimer_tic(&t);
  dmatrix_mmul(out,
		    (const double **) in, (const double **) in,
		    n, n, n);
  ptimer_toc(&t);
  printf("dmatrix_mmul: ");
  siunit_show(ptimer_get_sec(&t));
  puts("\n");

  if (print) {
    dmatrix_print((const double **) out, n, n);
    puts("");
  }

  ptimer_tic(&t);
  dmatrix_mmul_strassen(out,
			     (const double **) in, (const double **) in, n, n,
			     n);
  ptimer_toc(&t);
  printf("dmatrix_mmul_strassen: ");
  siunit_show(ptimer_get_sec(&t));
  puts("\n");

  if (print) {
    dmatrix_print((const double **) out, n, n);
  }

  dmatrix_free(in);
  dmatrix_free(out);
}

int
main(int argc, char *argv[])
{
  test_dmatrix_power_uint(8, 8, 1);
  return 0;
}
