#include "../packed_ops_SIMD.h"

#define BLEN_s (4)		/* block length */
#define BLEN_d (2)		/* block length */

int
main(int argc, char **argv)
{
  float datA[BLEN_s] __attribute__ ((aligned(16)));
  float datB[BLEN_s] __attribute__ ((aligned(16)));
  float datC[BLEN_s] __attribute__ ((aligned(16)));
  farray_aligned16 wA = { datA }; /* wrapper */
  farray_aligned16 wB = { datB }; /* wrapper */
  farray_aligned16 wC = { datC }; /* wrapper */

  /* init arguments */
  for (int i = 0; i < BLEN_s; i++) { datA[i] = i; datB[i] = i; datC[i] = 0; }

  if (system("cat /proc/cpuinfo | grep sse > /dev/null") != 0) {
    printf("Processor does not have SSE.\n");
    exit(1);
  }
  if (system("cat /proc/cpuinfo | grep sse2 > /dev/null") != 0) {
    printf("Processor does not have SSE2.\n");
    exit(1);
  }

  printf("wA: %f %f %f %f\n", wA.dat[0], wA.dat[1], wA.dat[2], wA.dat[3]);
  printf("wB: %f %f %f %f\n", wB.dat[0], wB.dat[1], wB.dat[2], wB.dat[3]);

  /* ---------------------------- Group Separator ---------------------------- */

  float_unaligned_SSE_add(&wA.dat[0], &wB.dat[0], &wC.dat[0]);
  printf("SSE Unaligned Adds: Result %f\n", wC.dat[0]);

  float_unaligned_SSE_mul(&wA.dat[0], &wB.dat[0], &wC.dat[0]);
  printf("SSE Unaligned Muls: Result %f\n", wC.dat[0]);

  float_aligned16_SSE_add(wA, wB, wC);
  printf("SSE Aligned Adds: Result %f %f %f %f\n",
	 wC.dat[0], wC.dat[1], wC.dat[2], wC.dat[3]);

  float_aligned16_SSE_mul(wA, wB, wC);
  printf("SSE Aligned Muls: Result %f %f %f %f\n",
	 wC.dat[0], wC.dat[1], wC.dat[2], wC.dat[3]);


  /* ---------------------------- Group Separator ---------------------------- */

  double datD[BLEN_d] __attribute__ ((aligned(16)));
  double datE[BLEN_d] __attribute__ ((aligned(16)));
  double datF[BLEN_d] __attribute__ ((aligned(16)));
  darray_aligned16 wD = { datD }; /* wrapper */
  darray_aligned16 wE = { datE }; /* wrapper */
  darray_aligned16 wF = { datF }; /* wrapper */

  /* init arguments */
  for (int i = 0; i < BLEN_d; i++) { datD[i] = i; datE[i] = i; datF[i] = 0; }

  printf("wD: %f %f\n", wD.dat[0], wD.dat[1]);
  printf("wE: %f %f\n", wE.dat[0], wE.dat[1]);

  double_unaligned_SSE2_add(&wD.dat[0], &wE.dat[0], &wF.dat[0]);
  printf("SSE2 Unaligned Adds: Result %f\n", wF.dat[0]);

  double_unaligned_SSE2_mul(&wD.dat[0], &wE.dat[0], &wF.dat[0]);
  printf("SSE2 Unaligned Muls: Result %f\n", wF.dat[0]);

  double_aligned16_SSE2_add(wD, wE, wF);
  printf("SSE2 Aligned Adds: Result %f %f %f %f\n",
	 wF.dat[0], wF.dat[1], wF.dat[2], wF.dat[3]);

  double_aligned16_SSE2_mul(wD, wE, wF);
  printf("SSE2 Aligned Muls: Result %f %f %f %f\n",
	 wF.dat[0], wF.dat[1], wF.dat[2], wF.dat[3]);

  exit(0);
}
