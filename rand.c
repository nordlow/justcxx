#include "rand.h"
#include <stdio.h>

int g_fastrand16_seed = 0;

/* Global static variables: */
unsigned long g_randstate_z=362436069, g_randstate_w=521288629, g_randstate_jsr=123456789, g_randstate_jcong=380116160;
unsigned long g_randstate_a=224466889, g_randstate_b=7584631, g_randstate_t[256];
/* Use random seeds to reset g_randstate_z,g_randstate_w,g_randstate_jsr,g_randstate_jcong,g_randstate_a,g_randstate_b, and the table
   g_randstate_t[256]*/
unsigned long g_randstate_x=0,g_randstate_y=0,g_randstate_bro; static unsigned char g_randstate_c=0;

void settable(unsigned long i1,
              unsigned long i2,
              unsigned long i3,
              unsigned long i4,
              unsigned long i5,
              unsigned long i6)
{
  int i;
  g_randstate_z=i1;
  g_randstate_w=i2;
  g_randstate_jsr=i3;
  g_randstate_jcong=i4;
  g_randstate_a=i5;
  g_randstate_b=i6;
  for (i=0;i<256;i=i+1)
    g_randstate_t[i] = KISS;
}

void test(void)
{
  int i; unsigned long k;
  settable(12345,65435,34221,12345,9983651,95746118);
  for(i=1;i<1000001;i++){k=LFIB4;} printf("%lu\n", k-1064612766U);
  for(i=1;i<1000001;i++){k=SWB ;} printf("%lu\n", k- 627749721U);
  for(i=1;i<1000001;i++){k=KISS ;} printf("%lu\n", k-1372460312U);
  for(i=1;i<1000001;i++){k=CONG ;} printf("%lu\n", k-1529210297U);
  for(i=1;i<1000001;i++){k=SHR3 ;} printf("%lu\n", k-2642725982U);
  for(i=1;i<1000001;i++){k=MWC ;} printf("%lu\n", k- 904977562U);
  for(i=1;i<1000001;i++){k=FIB ;} printf("%lu\n", k-3519793928U);
}
