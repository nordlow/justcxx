#include "bitmtx.h"

#include <stdlib.h>

void
bitmtx_init(Bitmtx * a, size_t m, size_t n)
{
  size_t l = m * n;
  if (l) { a->d = malloc_aligned(16, bitvec_get_data_bytesize(l)); } else { a->d = NULL; }
  a->m = m;
  a->n = n;
}

void
bitmtx_initZ(Bitmtx * a, size_t m, size_t n)
{
  size_t l = m * n;
  if (l) { a->d = calloc_aligned(16, bitvec_get_data_bytesize(l)); } else { a->d = NULL; }
  a->m = m;
  a->n = n;
}

Bitmtx *
bitmtx_new(size_t m, size_t n)
{
  Bitmtx *a = (Bitmtx*)malloc(sizeof(Bitmtx));
  bitmtx_init(a, m, n);
  return a;
}

Bitmtx *
bitmtx_newZ(size_t m, size_t n)
{
  Bitmtx *a = (Bitmtx*)malloc(sizeof(Bitmtx));
  bitmtx_initZ(a, m, n);
  return a;
}

void
bitmtx_clear(Bitmtx * a)
{
  size_t l = a->m * a->n;
  if (l) { free(a->d); a->d = NULL; }
}

void
bitmtx_delete(Bitmtx * a)
{
  bitmtx_clear(a);
  free(a);
}

void
bitmtx_resize(Bitmtx * a, size_t m, size_t n)
{
  if (a->m * a->n != m * n) {
    leprintf("unmatched dimensions %d %d\n", (int)m, (int)n);
  }
  a->m = m;
  a->n = n;
}

uint
bitmtx_atIJ(Bitmtx * a, size_t i, size_t j)
{
  PTEST("This looks fishy!...\n");
  return bitsblock_get(a->d, i * a->n + j);
}

void
bitmtx_set0all(Bitmtx * a)
{
  size_t i;
  size_t l = a->m * a->n;
  for (i = 0; i < bitvec_get_data_blocknum(l); i++) {
    a->d[i] = 0;
  }
}

void
bitmtx_set1all(Bitmtx * a)
{
  size_t i;
  size_t l = a->m * a->n;
  for (i = 0; i < bitvec_get_data_blocknum(l); i++) {
    a->d[i] = BITVEC_BLOCK_ONES;
  }
}

void
bitmtx_rand(Bitmtx * a)
{
  size_t i;
  size_t l = a->m * a->n;
  for (i = 0; i < bitvec_get_data_blocknum(l); i++) {
    a->d[i] = bitsblock_rand();
  }
}

void
bitmtx_randadj(Bitmtx * a)
{
}

void
bitmtx_fprint(FILE * stream, Bitmtx * a)
{
  size_t i, j;
  const char *sepstr = "  ";
  for (i = 0; i < a->m; i++) {
    for (j = 0; j < a->n; j++) {
      putc(bitmtx_atIJ(a, i, j) ? '1' : '0', stream);
      fprintf(stream, "%s", sepstr);
    }
    putc('\n', stream);
  }
}

void
bitmtx_print(Bitmtx * a)
{
  bitmtx_fprint(stdout, a);
}

void
bitmtx_test(void)
{
  Bitmtx a;
  while (1) {
    getc(stdin);
    bitmtx_init(&a, 11, 7);

    bitmtx_rand(&a);
    bitmtx_print(&a);

    bitmtx_set0all(&a);
    bitmtx_print(&a);

    bitmtx_set1all(&a);
    bitmtx_print(&a);

    bitmtx_clear(&a);
  }
}
