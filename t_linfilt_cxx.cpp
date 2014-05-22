#include <iostream>

#include "timing.h"
#include "dot.h"
#include "stdio_x.h"
#include "linfilt.hpp"

using namespace std;

void test_affined2d()
{
  float x[2] = { 2, 4 };
  float a[4] = { 3, 0,
		 0, 6 };
  float b[2] = { 3, 7 };
  cout << "x: [" << x[0] << "," << x[1] << "] ";
  cout << "a: ["
       << a[0] << "," << a[1] << ","
       << a[2] << "," << a[3] << "] ";
  cout << "b: [" << b[0] << "," << b[1] << "] ";
  affine2d(x, a, b);
  cout << "x: [" << x[0] << "," << x[1] << "] " << endl;
  cout << endl;
}

void test_pwss2d()
{
  const float a[4] = { 3, 2 };
  const float b[2] = { 13, 17 };
  {
    float x[2] = { 2, 5 };
    cout << "x: [" << x[0] << "," << x[1] << "] ";
    cout << "a: [" << a[0] << "," << a[1] << "] ";
    cout << "b: [" << b[0] << "," << b[1] << "] ";
    fa2_ss(x, a, b);
    cout << "x: [" << x[0] << "," << x[1] << "] " << endl;
  }
  {
    const float x[2] = { 2, 5 };
    int y[2];
    cout << "x: [" << x[0] << "," << x[1] << "] ";
    cout << "a: [" << a[0] << "," << a[1] << "] ";
    cout << "b: [" << b[0] << "," << b[1] << "] ";
    fa2_ss_i(y, x, a, b);
    cout << "y: [" << y[0] << "," << y[1] << "] " << endl;
  }
  {
    const float x[2] = { 2, 5 };
    short y[2];
    cout << "x: [" << x[0] << "," << x[1] << "] ";
    cout << "a: [" << a[0] << "," << a[1] << "] ";
    cout << "b: [" << b[0] << "," << b[1] << "] ";
    fa2_ss_s(y, x, a, b);
    cout << "y: [" << y[0] << "," << y[1] << "] " << endl;
  }
  cout << endl;
  {
    const float x[2] = { 2, 5 };
    const short s[2] = { 7, 9 };
    short y[2];
    cout << "x: [" << x[0] << "," << x[1] << "] ";
    cout << "a: [" << a[0] << "," << a[1] << "] ";
    cout << "b: [" << b[0] << "," << b[1] << "] ";
    cout << "s: [" << s[0] << "," << s[1] << "] ";
    fa2_ss_sc(y, x, a, b, s);
    cout << "y: [" << y[0] << "," << y[1] << "] " << endl;
  }
}

#define FILTLEN (128)

void test_linfilt()
{
  float a[FILTLEN] __attribute__ ((aligned(16)));
  float b[FILTLEN] __attribute__ ((aligned(16)));
  farray_aligned16 aw = { a }, bw = { b };
  float sum;

  pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);

  int niters = 65536 * 16;

  for (int i = 0; i < FILTLEN; i++) {
    a[i] = 1;
    b[i] = 1;
  }

  sum = 0;
  ptimer_tic(&tmr);
  for (int i = 0; i < niters; i++) {
    sum += farray_dot_8stretched_plainC(a, b, FILTLEN);
  }
  ptimer_toc(&tmr);
  printf("farray_dot_8stretched_plainC:     sum: %f took ", sum);
  ptimer_print_sec_usec9(tmr); endline();

  sum = 0;
  ptimer_tic(&tmr);
  for (int i = 0; i < niters; i++) {
    sum += farray_dot_unrolled128(a, b);
  }
  ptimer_toc(&tmr);
  printf("farray_dot_unrolled128:           sum: %f took ", sum);
  ptimer_print_sec_usec9(tmr); endline();

  sum = 0;
  ptimer_tic(&tmr);
  for (int i = 0; i < niters; i++) {
    sum += farray_dot_8stretched_aligned16(aw, bw, FILTLEN);
  }
  ptimer_toc(&tmr);
  printf("farray_dot_8stretched_aligned16:  sum: %f took ", sum);
  ptimer_print_sec_usec9(tmr); endline();

  sum = 0;
  ptimer_tic(&tmr);
  for (int i = 0; i < niters; i++) {
    sum += farray_dot_8stretched_unaligned(a, b, FILTLEN);
  }
  ptimer_toc(&tmr);
  printf("farray_dot_8stretched_unaligned:  sum: %f took ", sum);
  ptimer_print_sec_usec9(tmr); endline();

  sum = 0;
  ptimer_tic(&tmr);
  for (int i = 0; i < niters; i++) {
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
