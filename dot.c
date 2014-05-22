#include "dot.h"

float
farray_dot(const float *a, const float *b, size_t n)
{
  float c = 0;
  for (size_t i = 0; i < n; i++) {
    c += a[i] * b[i];
  }
  return c;
}

float
farray_dot_unrolled16(const float *a, const float *b)
{
  return (a[ 0]*b[ 0] + a[ 1]*b[ 1] + a[ 2]*b[ 2] + a[ 3]*b[ 3] +
	  a[ 4]*b[ 4] + a[ 5]*b[ 5] + a[ 6]*b[ 6] + a[ 7]*b[ 7] +
	  a[ 8]*b[ 8] + a[ 9]*b[ 9] + a[10]*b[10] + a[11]*b[11] +
	  a[12]*b[12] + a[13]*b[13] + a[14]*b[14] + a[15]*b[15]);
}

float
farray_dot_unrolled128(const float *a, const float *b)
{
  return (farray_dot_unrolled16(a +   0, b +   0) +
	  farray_dot_unrolled16(a +  16, b +  16) +
	  farray_dot_unrolled16(a +  32, b +  32) +
	  farray_dot_unrolled16(a +  48, b +  48) +
	  farray_dot_unrolled16(a +  64, b +  64) +
	  farray_dot_unrolled16(a +  80, b +  80) +
	  farray_dot_unrolled16(a +  96, b +  96) +
	  farray_dot_unrolled16(a + 112, b + 112));
}

float
farray_dot_8stretched_plainC(const float *a, const float *b, size_t n)
{
  float ans = 0.0f;
  size_t i;
  if (n >= 8) {
    for (i = 0; i < ( n >> 3 ); ++i, a += 8, b += 8)
      ans += (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3] +
	      a[4] * b[4] + a[5] * b[5] + a[6] * b[6] + a[7] * b[7]);
    n -= i << 3;
  }
  for (i = 0; i < n; ++i) {
    ans += a[i] * b[i];
  }
  return ans;
}

float
farray_dot_shv_8stretched(const float *a, const short *b, size_t n)
{
  float ans = 0.0f;
  size_t i;
  if (n >= 8) {
    for (i = 0; i < ( n >> 3 ); ++i, a += 8, b += 8)
      ans += (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3] +
	      a[4] * b[4] + a[5] * b[5] + a[6] * b[6] + a[7] * b[7]);
    n -= i << 3;
  }
  for (i = 0; i < n; ++i) {
    ans += a[i] * b[i];
  }
  return ans;
}

float
farray_dot_8stretched_aligned16(farray_aligned16 a,
				    farray_aligned16 b,
				    size_t n)
{
#ifdef HAVE_CPU_SSE
  float ans[4] __attribute__ ((aligned(16)));
  size_t i;
  if (n >= 8) {
    __asm__ volatile
      ("xorps      %%xmm0, %%xmm0" /* zero xmm0 */
       : /* outputs */
       : /* inputs */
       : /* clobbered */ "xmm0" );
    for (i = 0; i < ( n >> 3 ); ++i ) {
      __asm__ volatile
	("movaps     (%0), %%xmm1\n\t"	 /* load 4 aligned float */
	 "movaps     16(%0), %%xmm2\n\t" /* load 4 aligned float */
	 "movaps     (%1), %%xmm3\n\t"	 /* load 4 aligned float */
	 "movaps     16(%1), %%xmm4\n\t" /* load 4 aligned float */
	 "add        $32,%0\n\t"	 /* increase pointer a.dat */
	 "add        $32,%1\n\t"	 /* increase pointer b.dat */
	 "mulps      %%xmm3, %%xmm1\n\t"
	 "mulps      %%xmm4, %%xmm2\n\t"
	 "addps      %%xmm2, %%xmm1\n\t"
	 "addps      %%xmm1, %%xmm0"
	 : /* outputs */ "+r" ( a.dat ), "+r" ( b.dat )
	 : /* inputs */
	 : /* clobbered */ "xmm0", "xmm1", "xmm2", "xmm3", "xmm4" );
    }
    __asm__ volatile
      ("movaps     %%xmm0, %0"	/* copy result to memory */
       : /* outputs */ "=m" ( ans )
       : /* inputs */
       : /* clobbered */ "xmm0", "memory" );
    n -= i << 3;
    ans[0] += ans[1] + ans[2] + ans[3];
  } else {
    ans[0] = 0.0;
  }
  for (i = 0; i < n; ++i) {
    ans[0] += a.dat[i] * b.dat[i];
  }
  return ans[0];
#else
  return farray_dot_8stretched_plainC(a.dat, b.dat, n);
#endif
}

float
farray_dot_8stretched_unaligned(const float * a,
				    const float * b,
				    size_t n)
{
#ifdef HAVE_CPU_SSE
  float ans[4] __attribute__ ((aligned(16)));
  size_t i;
  if (n >= 8) {
    __asm__ volatile
      ("xorps      %%xmm0, %%xmm0" /* zero xmm0 */
       : /* outputs */
       : /* inputs */
       : /* clobbered */ "xmm0" );
    for (i = 0; i < ( n >> 3 ); ++i ) {
      __asm__ volatile
	("movups     (%0), %%xmm1\n\t"	 /* load 4 unaligned float */
	 "movups     16(%0), %%xmm2\n\t" /* load 4 unaligned float */
	 "movups     (%1), %%xmm3\n\t"	 /* load 4 unaligned float */
	 "movups     16(%1), %%xmm4\n\t" /* load 4 unaligned float */
	 "add        $32,%0\n\t"	 /* increase pointer a.dat */
	 "add        $32,%1\n\t"	 /* increase pointer b.dat */
	 "mulps      %%xmm3, %%xmm1\n\t"
	 "mulps      %%xmm4, %%xmm2\n\t"
	 "addps      %%xmm2, %%xmm1\n\t"
	 "addps      %%xmm1, %%xmm0"
	 : /* outputs */ "+r" ( a ), "+r" ( b )
	 : /* inputs */
	 : /* clobbered */ "xmm0", "xmm1", "xmm2", "xmm3", "xmm4" );
    }
    __asm__ volatile
      ("movups     %%xmm0, %0"	/* copy result to memory */
       : /* outputs */ "=m" ( ans )
       : /* inputs */
       : /* clobbered */ "xmm0", "memory" );
    n -= i << 3;
    ans[0] += ans[1] + ans[2] + ans[3];
  } else {
    ans[0] = 0.0;
  }
  for (i = 0; i < n; ++i) {
    ans[0] += a[i] * b[i];
  }
  return ans[0];
#else
  return farray_dot_8stretched_plainC(a, b, n);
#endif
}

float
farray_dot_8stretched_anyaligned(const float * a,
				     const float * b,
				     size_t n)
{
  if (is_aligned16(a) &&
      is_aligned16(b)) {
    farray_aligned16 aa, ab;
    aa.dat = (float*)a;
    ab.dat = (float*)b;
    /*     lprintf("ALIGNED\n"); */
    return farray_dot_8stretched_aligned16(aa, ab, n);
  } else {
    /*     lprintf("UNALIGNED %p %p\n", a, b); */
    return farray_dot_8stretched_unaligned(a, b, n);
  }
}
