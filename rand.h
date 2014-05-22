/*! \file rand.h
 * \brief Generate Random Numbers.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
 * \see http://www.cse.yorku.ca/~oz/marsaglia-rng.html
 */

#pragma once
#include <inttypes.h>
#include "pnw_types.h"
#include "cc_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
  Write your own calling program and try one or more of
  the above, singly or in combination, when you run a
  simulation. You may want to change the simple 1-letter
  names, to avoid conflict with your own choices. */

/* All that follows is comment, mostly from the initial post. You may want to remove it */

/* Any one of KISS, MWC, FIB, LFIB4, SWB, SHR3, or CONG can be used in an
   expression to provide a random 32-bit integer.

   The KISS generator, (Keep It Simple Stupid), is designed to combine the two
   multiply-with-carry generators in MWC with the 3-shift register SHR3 and the
   congruential generator CONG, using addition and exclusive-or. Period about
   2^123. It is one of my favorite generators.

   The MWC generator concatenates two 16-bit multiply- with-carry generators,
   x(n)=36969x(n-1)+carry, y(n)=18000y(n-1)+carry mod 2^16, has period about
   2^60 and seems to pass all tests of randomness. A favorite stand-alone
   generator---faster than KISS, which contains it.

   FIB is the classical Fibonacci sequence x(n)=x(n-1)+x(n-2),but taken modulo
   2^32. Its period is 3*2^31 if one of its two seeds is odd and not 1 mod 8. It
   has little worth as a RNG by itself, but provides a simple and fast component
   for use in combination generators.

   SHR3 is a 3-shift-register generator with period 2^32-1. It uses
   y(n)=y(n-1)(I+L^17)(I+R^13)(I+L^5), with the y's viewed as binary vectors, L
   the 32x32 binary matrix that shifts a vector left 1, and R its
   transpose. SHR3 seems to pass all except those related to the binary rank
   test, since 32 successive values, as binary vectors, must be linearly
   independent, while 32 successive truly random 32-bit integers, viewed as
   binary vectors, will be linearly independent only about 29% of the time.

   CONG is a congruential generator with the widely used 69069 multiplier:
   x(n)=69069x(n-1)+1234567. It has period 2^32. The leading half of its 32 bits
   seem to pass tests, but bits in the last half are too regular.

   LFIB4 is an extension of what I have previously defined as a lagged Fibonacci
   generator: x(n)=x(n-r) op x(n-s), with the x's in a finite set over which
   there is a binary operation op, such as +,- on integers mod 2^32, * on odd
   such integers, exclusive-or(xor) on binary vectors. Except for those using
   multiplication, lagged Fibonacci generators fail various tests of randomness,
   unless the lags are very long. (See SWB below). To see if more than two lags
   would serve to overcome the problems of 2-lag generators using +,- or xor, I
   have developed the 4-lag generator LFIB4 using addition:
   x(n)=x(n-256)+x(n-179)+x(n-119)+x(n-55) mod 2^32. Its period is
   2^31*(2^256-1), about 2^287, and it seems to pass all tests---in particular,
   those of the kind for which 2-lag generators using +,-,xor seem to fail. For
   even more confidence in its suitability, LFIB4 can be combined with KISS,
   with a resulting period of about 2^410: just use (KISS+LFIB4) in any C
   expression.

   SWB is a subtract-with-borrow generator that I developed to give a simple
   method for producing extremely long periods: x(n)=x(n-222)-x(n-237)- borrow
   mod 2^32. The 'borrow' is 0, or set to 1 if computing x(n-1) caused overflow
   in 32-bit integer arithmetic. This generator has a very long period,
   2^7098(2^480-1), about 2^7578. It seems to pass all tests of randomness,
   except for the Birthday Spacings test, which it fails badly, as do all lagged
   Fibonacci generators using +,- or xor. I would suggest combining SWB with
   KISS, MWC, SHR3, or CONG. KISS+SWB has period >2^7700 and is highly
   recommended. Subtract-with-borrow has the same local behaviour as lagged
   Fibonacci using +,-,xor---the borrow merely provides a much longer
   period. SWB fails the birthday spacings test, as do all lagged Fibonacci and
   other generators that merely combine two previous values by means of =,- or
   xor. Those failures are for a particular case: m=512 birthdays in a year of
   n=2^24 days. There are choices of m and n for which lags >1000 will also fail
   the test. A reasonable precaution is to always combine a 2-lag Fibonacci or
   SWB generator with another kind of generator, unless the generator uses *,
   for which a very satisfactory sequence of odd 32-bit integers results.

   The classical Fibonacci sequence mod 2^32 from FIB fails several tests. It is
   not suitable for use by itself, but is quite suitable for combining with
   other generators.

   The last half of the bits of CONG are too regular, and it fails tests for
   which those bits play a significant role. CONG+FIB will also have too much
   regularity in trailing bits, as each does. But keep in mind that it is a rare
   application for which the trailing bits play a significant role. CONG is one
   of the most widely used generators of the last 30 years, as it was the system
   generator for VAX and was incorporated in several popular software packages,
   all seemingly without complaint.

   Finally, because many simulations call for uniform random variables in 0<x<1
   or -1<x<1, I use #define statements that permit inclusion of such variates
   directly in expressions: using UNI will provide a uniform random real (float)
   in (0,1), while VNI will provide one in (-1,1).

   All of these: MWC, SHR3, CONG, KISS, LFIB4, SWB, FIB UNI and VNI, permit
   direct insertion of the desired random quantity into an expression, avoiding
   the time and space costs of a function call. I call these in-line-define
   functions. To use them, static variables z,w,jsr,jcong,a and b should be
   assigned seed values other than their initial values. If LFIB4 or SWB are
   used, the static table t[256] must be initialized.

   A note on timing: It is difficult to provide exact time costs for inclusion
   of one of these in-line- define functions in an expression. Times may differ
   widely for different compilers, as the C operations may be deeply nested and
   tricky. I suggest these rough comparisons, based on averaging ten runs of a
   routine that is essentially a long loop: for(i=1;i<10000000;i++) L=KISS; then
   with KISS replaced with SHR3, CONG,... or KISS+SWB, etc. The times on my home
   PC, a Pentium 300MHz, in nanoseconds: FIB 49;LFIB4 77;SWB 80;CONG 80;SHR3
   84;MWC 93;KISS 157; VNI 417;UNI 450;
*/

#define ZNEW (g_randstate_z=36969*(g_randstate_z&65535)+(g_randstate_z>>16))
#define WNEW (g_randstate_w=18000*(g_randstate_w&65535)+(g_randstate_w>>16))
#define MWC ((ZNEW<<16)+WNEW )
#define SHR3 (g_randstate_jsr^=(g_randstate_jsr<<17), g_randstate_jsr^=(g_randstate_jsr>>13), g_randstate_jsr^=(g_randstate_jsr<<5))
#define CONG (g_randstate_jcong=69069*g_randstate_jcong+1234567)
#define FIB ((g_randstate_b=g_randstate_a+g_randstate_b),(g_randstate_a=g_randstate_b-g_randstate_a))
#define KISS ((MWC^CONG)+SHR3)
#define LFIB4 (g_randstate_c++,g_randstate_t[g_randstate_c]=g_randstate_t[g_randstate_c]+g_randstate_t[(unsigned char)(g_randstate_c+58)]+g_randstate_t[(unsigned char)(g_randstate_c+119)]+g_randstate_t[(unsigned char)(g_randstate_c+178)])
#define SWB (g_randstate_c++,g_randstate_bro=(g_randstate_x<g_randstate_y),g_randstate_t[g_randstate_c]=(g_randstate_x=g_randstate_t[(unsigned char)(g_randstate_c+34)])-(g_randstate_y=g_randstate_t[(unsigned char)(g_randstate_c+19)]+g_randstate_bro))
#define UNI (KISS*2.328306e-10)
#define VNI ((long) KISS)*4.656613e-10

/* Global static variables: */
extern unsigned long g_randstate_z, g_randstate_w, g_randstate_jsr, g_randstate_jcong;
extern unsigned long g_randstate_a, g_randstate_b, g_randstate_t[256];
/* Use random seeds to reset g_randstate_z,g_randstate_w,g_randstate_jsr,g_randstate_jcong,g_randstate_a,g_randstate_b, and the table
   g_randstate_t[256]*/
extern unsigned long g_randstate_x, g_randstate_y, g_randstate_bro;
//static unsigned char g_randstate_c;

/* Example procedure to set the table, using \c KISS. */
void settable(unsigned long i1,
              unsigned long i2,
              unsigned long i3,
              unsigned long i4,
              unsigned long i5,
              unsigned long i6);

/*! This is a test main program. It should compile and print 7 0's. */
void rand_test(void);

static inline wuur bool    bool_rand(void) { return (rand() >> 30) & 1; }
static inline wuur int8_t  int8_rand(void) { return (rand() >> 16) & 0xff; }
static inline wuur uint8_t uint8_rand(void) { return (rand() >> 16) & 0xff; }
static inline wuur int16_t int16_rand(void) { return (rand() >> 16) & 0xffff; }
static inline wuur int32_t int32_rand(void) {
  return ((((int32_t)int16_rand() << 0) & 0x0000ffff) |
	  (((int32_t)int16_rand() << 16) & 0xffff0000));
  /* return (rand()); */
}

static inline wuur int64_t int64_rand(void) {
  return ((((int64_t) int16_rand() <<  0) & 0x000000000000ffffULL) |
          (((int64_t) int16_rand() << 16) & 0x00000000ffff0000ULL) |
          (((int64_t) int16_rand() << 32) & 0x0000ffff00000000ULL) |
          (((int64_t) int16_rand() << 48) & 0xffff000000000000ULL));
}

extern int g_fastrand16_seed;
static inline void fast_srand(int seed) { g_fastrand16_seed = seed; }
static inline int fastrand_iter(void) {  return (g_fastrand16_seed = (214013 * g_fastrand16_seed + 2531011));}

/*! \em Fast Pseudo-Random Generator in range [0 0xffff].
 * \see http://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c/3747462#3747462
 * \see http://software.intel.com/en-us/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor/
 */
static inline wuur int fastrand16(void) { return (fastrand_iter() >> 16);  }

/*! \em Fast Pseudo-Random Generator in range [0 0xffffffff]. */
static inline wuur int fastrand32(void) { return (fastrand16() | (fastrand16() << 16)); }

#ifdef __cplusplus
}
#endif
