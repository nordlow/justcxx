#include "reorg.h"
#include "utils.h"
#include "qsort.h"
#include "sortn.h"
#include "timing.h"
#include "bitwise.h"
#include "binlog.h"
#include "perm.h"
#include "meman.h"
#include "rangerand.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

void
intarray_flip(int *a, size_t n)
{
  for (size_t i = 0; i < n / 2; i++) {
    int_swap(&a[i], &a[n - 1 - i]);
  }
}

void
sizearray_flip(size_t *a, size_t n)
{
  for (size_t i = 0; i < n / 2; i++) {
    size_swap(&a[i], &a[n - 1 - i]);
  }
}

void
darray_flip(double *a, size_t n)
{
  for (size_t i = 0; i < n / 2; i++) {
    double_swap(&a[i], &a[n - 1 - i]);
  }
}

void
intarray_rsflip(int *a, size_t n)
{
  int len, off;			/* sub region length and offset */
  len = rand() % (n + 1);
  off = rand() % (n - len + 1);
  intarray_flip(a + off, len);
}

void
intarray_crsflip(int *a, size_t n)
{
  /* \TODO: Should be more symmetric for genetic algorithm permutations. */
}

void
intarray_crsswap(int *a, size_t n)
{
  size_t rlen, i0, sep, i1;

  rlen = rand() % (n / 2);
  i0 = rand() % n;
  sep = rand() % (n - 2 * rlen + 1);
  i1 = (i0 + rlen + sep) % n;
/*      printf("rlen:%d i0:%d sep:%d i1:%d rlen+sep:%d\n", */
/*              rlen, i0, sep, i1, rlen+sep); */

  for (size_t i = 0; i < rlen; i++) {
/*          printf("i0:%d i1:%d\n", i0, i1); */
/*          intarray_print(a, n); */
    SWAP(a[i0], a[i1]);
/*          intarray_print("a", a, n); */
    i0++;
    if (i0 >= n)
      i0 = 0;
    i1++;
    if (i1 >= n)
      i1 = 0;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
intarray_shuffle(int *a, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    int tmp;
    int j;
    j = rand() % n;
    tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
  }
}

void
intarray_partial_shuffle(int *a, size_t n, size_t nswaps)
{
  for (size_t i = 0; i < nswaps; i++) {
    int tmp;
    int j, k;
    j = rand() % n;
    k = rand() % n;
    tmp = a[j];
    a[j] = a[k];
    a[k] = tmp;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Generate a \em Random Permutation
 * \param[out] a Random permutation of [0 ... n-1] of length \p n.
 */
void
intarray_random_permutation(int *a, size_t n)
{
  intarray_ramp(a, n);
  intarray_shuffle(a, n);
}

/*! Generate a Fast Pseudo Random Permutation (fast)
 * \param[out] a Random permutation of [0 ... n-1] of length \p n.
 */
void
intarray_fast_random_permutation(int *a, size_t n)
{
  int x = 1;
  for (size_t i = 0; i < n; i++) {
    x = (x * (n - 1)) % n;
    a[i] = x;
  }
}

/*! Check if argument is a permutation.
 * In: a is an integer vector of length \p n.
 * Out:
 * \return true if a is a permutation of [0 ... n-1], false otherwise.
 */
int
intarray_is_permutation(const int *a, size_t n)
{
  int *h;			/* histogram */

  h = calloc(n, sizeof(int));

  for (size_t i = 0; i < n; i++) {
    if ((ssize_t)a[i] < 0 ||
	(ssize_t)a[i] >= (ssize_t)n) {
      return false;
    }
    if (++h[a[i]] > 1) {
      return false;
    }
  }

  free(h);
  return true;
}

/*! Check if argument is a permutation.
 * In: a is an integer vector of length \p n.
 * Out:
 * \return true if a is a permutation of [0 ... n-1], otherwise false.
 */
int
sizearray_is_permutation(const size_t *a, size_t n)
{
  int *h;			/* histogram */

  h = calloc(n, sizeof(int));

  for (size_t i = 0; i < n; i++) {
    if (a[i] >= n) {
      return false;
    }
    if (++h[a[i]] > 1) {
      return false;
    }
  }

  free(h);
  return true;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Function: Quad-Tree Indexing (in 2 Dimensions).
 * In:
 * - x and y are the 2D-coordinate arrays, both of length \p n.
 * - idx is a permutation of [0 ... n-1], indicating the current ordering
 *   of x and y.
 * Out: Replaces idx, x and y with their reordered variants.
 *
 * \complexity O(n*(log(n))^2)
 */
void
darray_qtidx(size_t *idx, const double *x, const double *y, size_t n)
{
  size_t i, j;
  size_t nloops;
  int *lens, *tlens;		/* lengths of sub regions */
  size_t nlens, nlensmax;
  int pos;
  double *tx, *ty;

  nloops = int_binlog(n);

  nlensmax = (1 << nloops) / 2;	/* \TODO: Change to (1 << nloops) / 2. */
  lens = intarray_malloc(nlensmax);
  tlens = intarray_malloc(nlensmax);
  tx = darray_malloc(n);
  ty = darray_malloc(n);

  lens[0] = n;
  nlens = 1;
/*      printf ("nlensmax: %d\n", nlensmax); */

  for (i = 0; i < nloops; i++, nlens *= 2) {
/*          int l, rj, wj; */
    /* len, lens read and write indexes */
/*          printf ("loop i: %d\n", i); */

    pos = 0;
    if (i % 2 == 0) {
      darray_perm(tx, x, idx, n);
      for (j = 0; j < nlens; j++) {
/*                  printf ("x loop j: %d  lens[j]:%d\n", j, lens[j]); */
	if (lens[j] > 1) {
	  darray_iqsort(tx + pos, idx + pos, lens[j]);
	  /* Reverse every other to improve locality. */
	  if (rand() % 2 == 0) {
	    darray_flip(tx + pos, lens[j]);
	    sizearray_flip(idx + pos, lens[j]);
	  }
	}
	pos += lens[j];

	/* \TODO: Fix this later. */
/*                  if (nlens < nlensmax) { */
/*                      rj = nlens-1 - j; */
/*                      wj = nlensmax-2 - 2*j; */
/*                      printf ("rj: %d  wj: %d\n", rj, wj); */
/*                      l = lens[rj]; */
/*                      lens[wj + 0] = l / 2; */
/*                      lens[wj + 1] = l - l / 2; */
/*                  } */
      }
      darray_perm(ty, y, idx, n);
    } else {
      darray_perm(ty, y, idx, n);
      for (j = 0; j < nlens; j++) {
/*                  printf ("y loop j: %d  lens[j]:%d\n", j, lens[j]); */
	if (lens[j] > 1) {
	  darray_iqsort(ty + pos, idx + pos, lens[j]);
	  if (rand() % 2 == 0) {
	    darray_flip(ty + pos, lens[j]);
	    sizearray_flip(idx + pos, lens[j]);
	  }
	}
	pos += lens[j];

	/* \TODO: Fix this later. */
/*                  if (nlens < nlensmax) { */
/*                      rj = nlensmax-nlens + j; */
/*                      wj = 2*j; */
/*                      printf ("rj: %d  wj: %d\n", rj, wj); */
/*                      l = lens[rj]; */
/*                      lens[wj + 0] = l / 2; */
/*                      lens[wj + 1] = l - l / 2; */
/*                  } */
      }
      darray_perm(tx, x, idx, n);
    }

    if (i < nloops - 1) {
      for (j = 0; j < nlens; j++) {
/*              printf ("loop j: %d  lens[j]:%d\n", j, lens[j]); */
	tlens[2 * j + 0] = lens[j] / 2;
	tlens[2 * j + 1] = lens[j] - lens[j] / 2;
      }
      intarray_copy(lens, tlens, 2 * nlens);
    }

  }

  free(lens);
  free(tlens);
  free(tx);
  free(ty);
}

void
test_qtidx8(void)
{
  size_t i[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
  double x[8] = { 14, 21, 16, 7, 11, 6, 4, 0 };
  double y[8] = { 9, 15, 23, 7, 13, 0, 19, 4 };
  size_t n = 8;
  darray_qtidx(i, x, y, n);
  darray_print(x, n);
  darray_print(y, n);
  sizearray_print("i", i, n);
}

void
test_qtidx(size_t n, int show)
{
  size_t *i;
  double *x;
  double *y;
  pTimer t; ptimer_init(&t, CLOCK_PROCESS_CPUTIME_ID);
  double secs;

  i = sizearray_malloc(n);
  x = darray_malloc(n);
  y = darray_malloc(n);

  sizearray_ramp(i, n);
  darray_rangerand(x, n, 0.0, 100.0);
  darray_rangerand(y, n, 0.0, 100.0);

  ptimer_tic(&t);
  darray_qtidx(i, x, y, n);
  ptimer_toc(&t);
  printf("darray_qtidx: ");
  secs = ptimer_get_sec(&t);
  printf("secs:%f  ratio: %f\n", secs, (n * SQR(log(n))) / secs);

  if (show) { sizearray_print("i", i, n); }

  free(i);
  free(x);
  free(y);
}

void
test_qtidxLoop(void)
{
  int i, n;
  n = 1024 * 1024;
  for (i = 1; i <= n; i *= 2) {
    printf("i: %d\n", i);
    test_qtidx(i, false);
  }
}

/*! Remainder Stochastic Sampling (RSS) most efficiently implemented
 * using Stochastic Uniform Sampling (SUS).
 *
 * (p. 32 A Genetic Algorithm Tutorial, Darrell Whitley).
 * In: idxnum, fits, fits_sum
 * Out: idx
 * \return -1 if fits_sum is smaller than the sum of the fits.
 */
int
darray_sus(int *idx, int idxnum, const double *fits, int fits_sum)
{
  int i, j;
  double pos, step;		/* sampling position and step */
  double pfs;			/* partial fit sum */

  step = fits_sum / idxnum;
  pos = double_rangerand(0, step);
  pfs = 0;
  i = j = 0;

  while (i < idxnum) {
    while (pfs + fits[j] < pos) {
      if (j >= idxnum) {
	/* \TODO: Should not happen. */
	leprintf("Error: j >= idxnum !\n");
	return -1;
      }
      pfs += fits[j];
      j++;
    }
    idx[i] = j;
    pos += step;
    i++;
  }

  return 0;
}
