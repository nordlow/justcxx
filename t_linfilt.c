#include <stdio.h>

#include "../linfilt.h"
#include "../vec2f.h"
#include "../vec2s.h"
#include "../vec2i.h"
#include "../box2f.hpp"
#include "../stdio_x.h"
#include "../timing.h"
#include "../dot.h"

void test_affined2d(void)
{
  vec2f x = { 2, 4 };
  mat2f a = { { 3, 0 },
	      { 0, 6 } };
  vec2f b = { 3, 7 };
  printf("x: "); vec2f_print(&x); putchar(' ');
  printf("a: "); box2f_print(&a); putchar(' ');
  printf("b: "); vec2f_print(&b); putchar(' ');
  float_affine2d(&x.x, &a.l.x, &b.x);
  printf("x: "); vec2f_print(&x); putchar(' '); endline();
  endline();
}

void test_pwss2d(void)
{
  const vec2f a = { 3, 2 };
  const vec2f b = { 13, 17 };
  {
    vec2f x = { 2, 5 };
    printf("x: "); vec2f_print(&x); putchar(' ');
    printf("a: "); vec2f_print(&a); putchar(' ');
    printf("b: "); vec2f_print(&b); putchar(' ');
    fa2_ss(&x.x, &a.x, &b.x);
    printf("x: "); vec2f_print(&x); putchar(' '); endline();
  }
  {
    const vec2f x = { 2, 5 };
    printf("x: "); vec2f_print(&x); putchar(' ');
    printf("a: "); vec2f_print(&a); putchar(' ');
    printf("b: "); vec2f_print(&b); putchar(' ');
    vec2i y;
    fa2_ss_i(&y.x, &x.x, &a.x, &b.x);
    printf("y: "); vec2i_print(&y); putchar(' '); endline();
  }
  {
    const vec2f x = { 2, 5 };
    vec2s y;
    printf("x: "); vec2f_print(&x); putchar(' ');
    printf("a: "); vec2f_print(&a); putchar(' ');
    printf("b: "); vec2f_print(&b); putchar(' ');
    fa2_ss_s(&y.x, &x.x, &a.x, &b.x);
    printf("y: "); vec2s_print(&y); putchar(' '); endline();
  }
  endline();
  {
    const vec2f x = { 2, 5 };
    const vec2s s = { 7, 9 };
    printf("x: "); vec2f_print(&x); putchar(' ');
    printf("a: "); vec2f_print(&a); putchar(' ');
    printf("b: "); vec2f_print(&b); putchar(' ');
    printf("s: "); vec2s_print(&s); putchar(' ');
    vec2s y;
    fa2_ss_sc(&y.x, &x.x, &a.x, &b.x, &s.x);
    printf("y: "); vec2s_print(&y); putchar(' '); endline();
  }
}

#define FILTLEN (128)

void test_linfilt(void)
{
  float a[FILTLEN] __attribute__ ((aligned(16)));
  float b[FILTLEN] __attribute__ ((aligned(16)));
  farray_aligned16 aw = { a }, bw = { b };
  float sum;
  int i;
  pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);
  int niters = 65536 * 16;

  for (i = 0; i < FILTLEN; i++) {
    a[i] = 1;
    b[i] = 1;
  }

  sum = 0;
  ptimer_tic(&tmr);
  for (i = 0; i < niters; i++) {
    sum += farray_dot_8stretched_plainC(a, b, FILTLEN);
  }
  ptimer_toc(&tmr);
  printf("farray_dot_8stretched_plainC:     sum: %f took ", sum);
  ptimer_print_sec_usec9(tmr); endline();

  sum = 0;
  ptimer_tic(&tmr);
  for (i = 0; i < niters; i++) {
    sum += farray_dot_unrolled128(a, b);
  }
  ptimer_toc(&tmr);
  printf("farray_dot_unrolled128:           sum: %f took ", sum);
  ptimer_print_sec_usec9(tmr); endline();

  sum = 0;
  ptimer_tic(&tmr);
  for (i = 0; i < niters; i++) {
    sum += farray_dot_8stretched_aligned16(aw, bw, FILTLEN);
  }
  ptimer_toc(&tmr);
  printf("farray_dot_8stretched_aligned16:  sum: %f took ", sum);
  ptimer_print_sec_usec9(tmr); endline();

  sum = 0;
  ptimer_tic(&tmr);
  for (i = 0; i < niters; i++) {
    sum += farray_dot_8stretched_unaligned(a, b, FILTLEN);
  }
  ptimer_toc(&tmr);
  printf("farray_dot_8stretched_unaligned:  sum: %f took ", sum);
  ptimer_print_sec_usec9(tmr); endline();

  sum = 0;
  ptimer_tic(&tmr);
  for (i = 0; i < niters; i++) {
    sum += farray_dot_8stretched_anyaligned(a, b, FILTLEN);
  }
  ptimer_toc(&tmr);
  printf("farray_dot_8stretched_anyaligned: sum: %f took ", sum);
  ptimer_print_sec_usec9(tmr); endline();
}

int main(int argc, char * argv[])
{
  test_affined2d();
  test_pwss2d();
  test_linfilt();
  return 0;
}
