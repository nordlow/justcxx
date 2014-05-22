/*!
 * \file median.c
 * \brief Median
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include "utils.h"

void
intarray_median(int *a, int n)
{
  int i, mi = 0;			/* median index */
  for (i = 1; i < n; i++) {
    if (a[i] < a[mi]) {
      mi--;
    } else {
      mi++;
    }
  }
}
