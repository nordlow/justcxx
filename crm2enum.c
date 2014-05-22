/*!
 * \file crm2enum.c
 * \brief Cyclic Random Mix of Two Enumerations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include "crm2enum.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int *b, *c;
static int ai, bi, ci;		/* indexes */
static int *btab, *ctab;
static int next_cnt, lookup_cnt;

static int
make_tabs(size_t n)
{
  btab = malloc(n * sizeof(int));
  ctab = malloc(n * sizeof(int));

  for (size_t i = 0; i < n; i++) {
    btab[i] = -1;
    ctab[i] = -1;
  }

  for (size_t i = 0; i < n; i++) {
    int val;

    val = b[i];
    if (val < 0 || val >= (int)n || btab[val] >= 0) {
      return -1;
    }
    btab[val] = i;

    val = c[i];
    if (val < 0 || val >= (int)n || ctab[val] >= 0) {
      return -2;
    }
    ctab[val] = i;
  }

  return 0;
}

static void
free_tabs(void)
{
  free(btab);
  free(ctab);
}

/* next and find takes the majority of the CPU time when few subseq's in
 * bb and cc match. */

static int
next(int *d, int di, size_t n)
{
  next_cnt++;
  while (d[di] == -1) {
    di++;
    if (di >= (int)n) {
      di = 0;
    }
  }
  return di;
}

static int
lookup(int *tab, int val)
{
  lookup_cnt++;
  return tab[val];
}

static void
match(int *a, size_t n)
{
  /* \TODO: Add optimizing for long copying cases. */

  while (b[bi] >= 0 &&		/* Check for (c[ci] >= 0) is not needed. */
	 b[bi] == c[ci]) {

    a[ai] = b[bi];
    b[bi] = -1;
    c[ci] = -1;

    ai++;			/* Wraparound check no needed here. */
    bi++;
    if (bi >= (int)n) {
      bi = 0;
    }
    ci++;
    if (ci >= (int)n) {
      ci = 0;
    }
  }
}

int
intarray_crm2enum(int *a, const int *bb, const int *cc, size_t n)
{
  int ret;
  b = malloc(n * sizeof(int));
  c = malloc(n * sizeof(int));

  memcpy(b, bb, n * sizeof(int));
  memcpy(c, cc, n * sizeof(int));

  if ((ret = make_tabs(n)) < 0) {
    free_tabs();
    fprintf(stderr, "%s: error: argument %d is not a permutation\n",
	    __FUNCTION__, 1 - ret);
    return -1;
  }

  next_cnt = 0;
  lookup_cnt = 0;

  ai = bi = ci = 0;
  while (ai < (int)n) {
    int r, val;

    /* Try one at random. */
    r = (rand() >> 8) & 1;

    if (r) {
      val = b[bi];
    } else {
      val = c[ci];
    }

    /* If occupied try the other. */
    if (val < 0) {
      r ^= 1;			/* Toggle bit. */
    }

    if (r) {
      val = b[bi];
    } else {
      val = c[ci];
    }

    if (val >= 0) {
      if (r) {
	ci = lookup(ctab, val);
      } else {
	bi = lookup(btab, val);
      }
    } else {
      bi = next(b, bi, n);
      ci = next(c, ci, n);
    }

    match(a, n);
  }

/*     printf("next_cnt: %d  lookup_cnt: %d  ratio: %g\n", */
/*             next_cnt, lookup_cnt, (double)next_cnt / lookup_cnt); */
  free(b);
  free(c);
  free_tabs();

  return 0;
}
