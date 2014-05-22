#include "../utils.h"
#include "../reorg.h"
#include "../crm2enum.h"
#include "../timing.h"

void
t_crm2enum(void)
{
  int *i;
  int *a;
  int *b;
  int *c;
  int j, n;
  int disp;
  pTimer t; ptimer_init(&t, CLOCK_PROCESS_CPUTIME_ID);

  disp = TRUE;

  for (n = 16;; n *= 1) {

    printf("\nn: %d\n", n);

    i = malloc(n * sizeof(n));
    a = malloc(n * sizeof(n));
    b = malloc(n * sizeof(n));
    c = malloc(n * sizeof(n));

    for (j = 0; j < n; j++) { i[j] = j; }

    intarray_random_permutation(b, n);
    intarray_random_permutation(c, n);

    if (disp) {
      intarray_print(i, n);
      intarray_print(b, n);
      intarray_print(c, n);
    }

    if (!intarray_is_permutation(b, n)) { printf("b is not a permutation!\n"); }

    if (!intarray_is_permutation(c, n)) { printf("c is not a permutation!\n"); }

    ptimer_tic(&t);
    intarray_crm2enum(a, b, c, n);
    ptimer_toc(&t);
    printf("intarray_crm2enum: %g million elements per second\n",
	   (double) n / 1000000 / ptimer_get_sec(&t));

    if (!intarray_is_permutation(a, n)) { printf("a is not a permutation!\n"); }

    if (disp) { intarray_print(a, n); }

    free(i);
    free(a);
    free(b);
    free(c);
  }
}

int main(void)
{
  t_crm2enum();
  return 0;
}
