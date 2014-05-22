#include <stdio.h>
#include <stdlib.h>

#include "../utils.h"
#include "../timing.h"
#include "../stdio_x.h"

void
test_tm(void)
{
  int s, us;
  pTime now, dur, future, past;
  int local_flag = 1;

  ptime_read_CLOCK_REALTIME(&now);
  printf("now: "); ptime_fprint_YYYY_MM_DD_HMSN(stdout, &now, local_flag); endline();

  printf("enter s: ");
  scanf("%d", &s);
  printf("enter us: ");
  scanf("%d", &us);
  ptime_init_sec_usec(&dur, s, us);

  future = now;
  ptime_add(&future, &dur);
  printf("future: "); ptime_fprint_YYYY_MM_DD_HMSN(stdout, &future, local_flag); endline();

  past = now;
  ptime_sub(&past, &dur);
  printf("past: "); ptime_fprint_YYYY_MM_DD_HMSN(stdout, &past, local_flag); endline();
}

int
main(void)
{
  test_tm();
  return 0;
}
