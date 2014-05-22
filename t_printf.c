#include <stdio.h>
#include <stdarg.h>

#include "../utils.h"
#include "../timing.h"
#include "../bitwise.h"
#include "../stdio_x.h"
#include "../binlog.h"
#include "../pathops.h"
#include "../extremes.h"

double
sum(int cnt, ...)
{
  int i;
  double sum = 0;
  va_list ap;
  va_start(ap, cnt);
  for (i = 0; i < cnt; i++) {
    sum += va_arg(ap, double);
  }
  va_end(ap);
  return sum;
}

int
pnw_print2(const char *format, ...)
{
  va_list ap;
  char buf[1000], *bufp = buf;
  int len;

  va_start(ap, format);
  bufp += vsprintf(bufp, format, ap);
  va_end(ap);

  /* send buf, bufp somewhere else... */

  len = bufp - buf;
  printf("%.*s", len, buf);

  return len;
}

#undef printf
#define printf(format, args...) pnw_print2(format , ##args)

/*!
 * This tests function illustrates the contra-intuitive syntax of C when
 * typedefining C-arrays of contstant size.
 */
void
test_carray_typedef(void)
{
  typedef int intpair_t[2];
  intpair_t ip = { 3, 4 };
  printf("intpair_t is [%d, %d] and has size: %d\n",
	 ip[0], ip[1], sizeof(intpair_t));
}

void
test_sscanf(const char * ipstr)
{
  int q[4];			/* quad of integers */
  int ret;
  ret = sscanf(ipstr, "%03d.%03d.%03d.%03d", &q[0], &q[1], &q[2], &q[3]);
  if (ret == 4) {
    printf("ret:%d IP:%03d.%03d.%03d.%03d\n", ret, q[0], q[1], q[2], q[3]);
  } else {
    printf("incorrect format\n");
  }
}

void test_typesizes(void)
{
  printf("int: %d\n"
	 "long: %d\n"
	 "long long: %d\n",
	 sizeof(int), sizeof(long), sizeof(long long));
}

void
test_print(void)
{
  const char * s = "ABC";
  printf("%.1s\n", s);
  printf("%.2s\n", s);
  printf("%.3s\n", s);
}

#include <float.h>

void
test_ieee754(void)
{
  int i;
  float x;

  printf("mantissa-digits: %d\n", FLT_MANT_DIG);

  printf("enter x:");
  int ret = scanf("%f", &x);
  if (ret == 1) {
    float_binfprint(stdout, x); endline();
  } else {
    lperror("scanf");
  }

  for (i = 0; i < 32; i++) {
    uint d = 1 << i;
    printf("d=%08x x=", d);
    float_binfprint(stdout, d); endline();
  }
}

void
test_fd(void)
{
  printf("stdout=%d stdin=%d\n", stdout, stdin);
}

void
test_stackpp(void)
{
  char buf[4];
  printf("buf=%p &buf=%p\n", buf, &buf);
}

void
test_signs(void)
{
  ushort us;
  signed short ss;

  us = 65535;
  ss = us;
  printf("us=%d ss=%d\n", us, ss);

  ss = -1;
  us = ss;
  printf("us=%d ss=%d\n", us, ss);
}

void
test_no_branching(void)
{
  int n;

  for (n = 16; n < 1024*1024*64; n *= 2) {
    int i;
    int std_sum, nob_sum;
    pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);

    printf("n:%d ", n);

    ptimer_tic(&tmr);
    std_sum = 0;
    for (i = 0; i < n; i++) {
      std_sum ^= int_min2(i, n - i);
    }
    ptimer_toc(&tmr);
    printf("std:"); ptimer_print_sec_usec9(tmr);

    ptimer_tic(&tmr);
    nob_sum = 0;
    for (i = 0; i < n; i++) {
      nob_sum ^= int_min_no_branching(i, n - i);
    }
    ptimer_toc(&tmr);
    printf(" nob:"); ptimer_print_sec_usec9(tmr);

    if (std_sum != nob_sum) {
      printf(" NOT EQUAL!");
    } else {
      printf(" sum:%d", std_sum);
    }

    endline();
  }
}

void test_binpow(void)
{
  for (uint32_t i = 0; i <= 1<<24; i++) {
    if (int32_is_binpow(i)) {
      lprintf("%d is a power of two\n", i);
    }
#if 0
    if (uint32_binlog_x86(i) != int_binlog(i)) {
      lprintf("i:%d uint32_binlog_x86! differse from int_binlog\n");
    }
#endif
  }
}

void test_guarded_memcpy(void)
{
  size_t len = 16;
  char buf[len];
  memcpy(buf, buf, len);
}

int
main(void)
{
  /* test_guarded_memcpy(); */
  test_typesizes();
  test_carray_typedef();
  printf("per: %d %d file\n", 3, 45);
  test_sscanf("010.001.001.008");
  test_sscanf("10.1.1.8");
  test_print();
  test_fd();
  test_ieee754();
  test_stackpp();
  test_signs();

  test_pathparseSRCV_cstr("gimp-2.0.tar.bz2");
  test_pathparseSRCV_cstr("gimp-2.0.1.tar.bz2");
  test_pathparseSRCV_cstr("gimp-2.0.1beta1.tar.bz2");
  test_pathparseSRCV_cstr("jpegsrc.v6b.tar.bz2");

  test_no_branching();

  test_binpow();

  return 0;
}
