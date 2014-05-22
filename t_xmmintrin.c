/*!
 * \file t_xmmintrin.c
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include "../timing.h"
#include "../stdio_x.h"
#include "../config.h"
#include <stdio.h>
#include <xmmintrin.h>

int main(void)
{
#ifdef HAVE_CPU_SSE
  float inp1[4] = { 1.2, 3.5, 1.7, 2.8 };
  float inp2[4] = { -0.7, 2.6, 3.3, -4.0 };
  float out[4];

  __m128 a, b, c;

  float inp_sse1[4] __attribute__((aligned(16))) = { 1.2, 3.5, 1.7, 2.8 };
  float inp_sse2[4] __attribute__((aligned(16))) = { -0.7, 2.6, 3.3, -4.0 };
  float out_sse[4] __attribute__((aligned(16)));

  const int lnum = 32768;		/* loop number */
  pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);

  ptimer_tic(&tmr);
  for (int l = 0; l < lnum; l++) {
    for (int i = 0; i < 4; i++) {
      out[i] = inp1[i] * inp2[i] + inp2[i];
    }
  }
  ptimer_toc(&tmr);
  printf("Scalar code took: "); ptimer_print_sec_usec9(tmr); endline();

  /* for (int i = 0; i < 4; i++) { printf("Result %d: %f\n", i, out[i]); } */

  ptimer_tic(&tmr);
  for (int l = 0; l < lnum; l++) {
    a = _mm_load_ps(inp_sse1);	/* Load Packed Single precision */
    b = _mm_load_ps(inp_sse2);
    c = _mm_mul_ps(a, b);
    c = _mm_add_ps(c, b);
    _mm_store_ps(out_sse,c);
  }
  ptimer_toc(&tmr);
  printf("SSE code took: "); ptimer_print_sec_usec9(tmr); endline();

  /* for (int i = 0; i < 4; i++) { printf("Result %d: %f\n",i,out_sse[i]); } */

  /* check that results are equal */
  int ok = 1;
  for (int i = 0; i < 4; i++) {
    if (out[i] != out_sse[i]) { ok = 0; break; }
  }

  if (ok) {
    printf("Test OK\n");
    return 1;
  } else {
    printf("Test NOT OK\n");
    return -1;
  }

#else
  printf("SSE not activated or supported\n");
  return 0;
#endif
}
