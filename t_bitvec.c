#include "../bitvec.h"
#include "../binlog.h"
#include "../stdio_x.h"

int
test_1_bitvec(void)
{
  Bitvec a, b, c;

  bitvec_init(&a, 64);
  bitvec_init(&b, 64);
  bitvec_init(&c, 64);

  bitvec_rand(&b);
  fprintf(stdout, "b: ");
  bitvec_fprint(stdout, &b);
  puts("");

  bitvec_rand(&c);
  fprintf(stdout, "c: ");
  bitvec_fprint(stdout, &c);
  puts("");

  bitvec_uniform_crossover(&a, &b, &c);
  fprintf(stdout, "a: ");
  bitvec_fprint(stdout, &a);
  puts("");

  bitvec_rev(&a, &a);
  fprintf(stdout, "a: ");
  bitvec_fprint(stdout, &a);
  puts("");

  bitvec_rev(&a, &a);
  fprintf(stdout, "a: ");
  bitvec_fprint(stdout, &a);
  puts("");

  bitvec_clear(&a);
  bitvec_clear(&b);
  bitvec_clear(&c);

  return 0;
}

int
test_bitvec_none_all(char ch)
{
  Bitvec a;
  bitvec_initbytes(&a, 1, &ch);
  fprintf(stdout, "a: ");
  bitvec_fprint(stdout, &a);
  fprintf(stdout, "none1:%d all1:%d", bitvec_none1(&a), bitvec_all1(&a));
  endline();
  bitvec_clear(&a);
  return 0;
}

int
test_3_bitvec(void)
{
  Bitvec a;
  size_t n = 128;
  bitvec_initZ(&a, n);
  for (size_t i = 0; i < n; i++) {
    if (bitvec_get(&a, i) != 0) { PWARN("Failed for a[i:%zd] != 0\n", i); }
    bitvec_set1(&a, i);
    if (bitvec_get(&a, i) != 1) { PWARN("Failed for a[i:%zd] != 1\n", i); }
  }
  bitvec_print(&a); printf("\n");
  return 0;
}

int
main(int argc, char *argv[])
{
  test_1_bitvec();
  test_bitvec_none_all(0x0);
  test_bitvec_none_all(0xf);
  test_bitvec_none_all(0xff);
  test_3_bitvec();

  test_binlog();

  return 0;
}
