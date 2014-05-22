#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <asm/msr.h>
#include "../timing.h"

int time_sec(void)
{
    time_t t;
    struct tm tm;
    if (time(&t) == (time_t)1)
    {
        fprintf(stderr, "%s: ", __FUNCTION__);
        perror("time");
        return -1;
    }
    localtime_r(&t, &tm);

    return tm.tm_sec;
}

void test_time1(void)
{
    time_t t;
    struct tm tm;
    char * ct;

    if (time(&t) == (time_t) 1)
    {
        perror("time");
        return;
    }

    ct = ctime(&t);
    printf("%s", ct);

    localtime_r(&t, &tm);
    printf(
        "sec:%d min:%d hour:%d\n"
        "mday:%d mon:%d year:%d\n"
        "wday:%d yday:%d isdst:%d\n",
        tm.tm_sec,
        tm.tm_min,
        tm.tm_hour,
        tm.tm_mday,
        tm.tm_mon,
        1900 + tm.tm_year,
        tm.tm_wday,
        tm.tm_yday,
        tm.tm_isdst);
}

void test_time2(void)
{
    uint64_t a, b;

    a = rdtsc();
    b = rdtsc();
    printf("rdtsc minimum cycles: %llu\n", b - a);

    a = rdtsc();
    sleep(1);
    b = rdtsc();
    printf("sleep 1 cycles: %llu\n", b - a);
}

int main(int argc, char ** argv)
{
    test_time1();
    test_time2();
    return 0;
}
