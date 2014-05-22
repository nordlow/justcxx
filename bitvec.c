#include "bitvec.h"
#include "rangerand.h"
#include "extremes.h"
#include "stdio_x.h"

#include <stdlib.h>
#include <string.h>

/* accesses b8's that are not byte-aligned. 0 <= bit_offset <= 7 (x % 8) */
#define b1a16_getB8(v, bit_offset) \
    ((b8)( *((b8*) (v)                   ) >> (bit_offset    ) ) | \
     (b8)( *((b8*) (v) + (bit_offset > 0)) << (8 - bit_offset) ))
#define b1v_getB8(v, x) (b1a16_getB8((b8*) v + x / 8, x % 8))

bool bitvec_none1(const Bitvec * a)
{
  bool ret = true;
  const size_t q = a->l / BITVEC_BLOCK_BITSIZE, r = a->l % BITVEC_BLOCK_BITSIZE;
  for (size_t i = 0; i < q; i++) {
    if (a->d[i] != BITVEC_BLOCK_ZEROS) { ret = false; }
  }
  if (ret && r) {
    /* \TODO: Move this expression to bitwise.h */
    if ((a->d[q] & ((1 << r) - 1)) != /* mask out rest bits */
        BITVEC_BLOCK_ZEROS) {
      ret = false;
    }
  }
  return ret;
}
bool bitvec_all1(const Bitvec * a)
{
  bool ret = true;
  const size_t q = a->l / BITVEC_BLOCK_BITSIZE, r = a->l % BITVEC_BLOCK_BITSIZE;
  for (size_t i = 0; i < q; i++) {
    if (a->d[i] != BITVEC_BLOCK_ONES) { ret = false; }
  }
  if (ret && r) {
    /* \TODO: Move this expression to bitwise.h */
    if ((a->d[q] ^ ((1 << r) - 1)) !=
        BITVEC_BLOCK_ZEROS) {
      ret = false;
    }
  }
  return ret;
}

void
bitvec_rand(Bitvec * a)
{
  for (size_t i = 0; i < bitvec_get_data_blocknum(a->l); i++) {
    a->d[i] = bitsblock_rand();
  }
}

void
bitvec_set0all(Bitvec * a)
{
  for (size_t i = 0; i < bitvec_get_data_blocknum(a->l); i++) {
    a->d[i] = 0;
  }
}
void
bitvec_set1all(Bitvec * a)
{
  for (size_t i = 0; i < bitvec_get_data_blocknum(a->l); i++) {
    a->d[i] = BITVEC_BLOCK_ONES;
  }
}

void
bitvec_parsestr(Bitvec * b, const char *str)
{
  bitvec_reinit(b, strlen(str));
  for (size_t i = 0; i < b->l; i++) {
    switch (str[i]) {
    case '0':
      bitvec_set0(b, i);
      break;
    case '1':
      bitvec_set1(b, i);
      break;
    default:
      leprintf("parse error at position %lu\n", (ulong)i);
      break;
    };
  }
}

void
bitvec_copyall(Bitvec * a, const Bitvec * b)
{
  bitvec_reinit(a, b->l);
  const size_t blocknum = bitvec_get_data_blocknum(b->l); /* NOTE: for auto-vectorization */
  for (size_t i = 0; i < blocknum; i++) {
    a->d[i] = b->d[i];
  }
}

void
bitvec_append(Bitvec * a, const Bitvec * b)
{
  PTOWRITE("\n");
}

static void
bitvec_reinitmin2(Bitvec * a, const Bitvec * b, const Bitvec * c)
{
  if (b->l != c->l) {
    leprintf("length mismatch. shortest of b and c becomes length of a!\n");
    bitvec_reinit(a, MIN2(b->l, c->l));
  } else {
    bitvec_reinit(a, b->l);
  }
}

void
bitvec_and(Bitvec * a, const Bitvec * b, const Bitvec * c)
{
  bitvec_reinitmin2(a, b, c);
  const size_t blocknum = bitvec_get_data_blocknum(b->l); /* NOTE: for auto-vectorization */
  for (size_t i = 0; i < blocknum; i++) {
    a->d[i] = b->d[i] & c->d[i];
  }
}
void
bitvec_or(Bitvec * a, const Bitvec * b, const Bitvec * c)
{
  bitvec_reinitmin2(a, b, c);
  const size_t blocknum = bitvec_get_data_blocknum(b->l); /* NOTE: for auto-vectorization */
  for (size_t i = 0; i < blocknum; i++) {
    a->d[i] = b->d[i] | c->d[i];
  }
}
void
bitvec_xor(Bitvec * a, const Bitvec * b, const Bitvec * c)
{
  bitvec_reinitmin2(a, b, c);
  const size_t blocknum = bitvec_get_data_blocknum(b->l); /* NOTE: for auto-vectorization */
  for (size_t i = 0; i < blocknum; i++) {
    a->d[i] = b->d[i] ^ c->d[i];
  }
}
void
bitvec_not(Bitvec * a, const Bitvec * b)
{
  bitvec_reinit(a, b->l);
  const size_t blocknum = bitvec_get_data_blocknum(b->l); /* NOTE: for auto-vectorization */
  for (size_t i = 0; i < blocknum; i++) {
    a->d[i] = ~b->d[i];
  }
}

void
bitvec_rev(Bitvec * a, const Bitvec * b)
{
  const size_t l = b->l;
  bitvec_reinit(a, l);
  for (size_t i = 0; i < l / 2; i++) {
    uint x, y;
    x = bitvec_get(b, i);
    y = bitvec_get(b, l - 1 - i);
    bitvec_set(a, i, y);
    bitvec_set(a, l - 1 - i, x);
  }
}

void
bitvec_uniform_crossover(Bitvec * a, const Bitvec * b, const Bitvec * c)
{
  bitvec_reinitmin2(a, b, c);
  const size_t blocknum = bitvec_get_data_blocknum(b->l); /* NOTE: for auto-vectorization */
  for (size_t i = 0; i < blocknum; i++) {
    a->d[i] = bitsblock_uniform_crossover(b->d[i], c->d[i]);
  }
}

void
bitvec_mutate(Bitvec * a, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    bitvec_toggle(a, rand() % n);
  }
}

void
bitvec_fprint(FILE * stream, const Bitvec * b)
{
  const char *sepstr = " ";
  for (size_t i = 0; i < b->l; i++) {
    putc(bitvec_get(b, i) ? '1' : '0', stream);
    fprintf(stream, "%s", sepstr);
  }
}
