#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstdbool>

#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "qsort_mt.h"
#include "stdio_x.h"

#ifndef ELEM_T
#define ELEM_T uint32_t
#endif

int
num_compare(const void *a, const void *b)
{
  return (*(ELEM_T *) a - *(ELEM_T *) b);
}

int
string_compare(const void *a, const void *b)
{
  return strcmp(*(char **) a, *(char **) b);
}

void *
xmalloc(size_t s)
{
  void *p;
  if ((p = malloc(s)) == NULL) { perror("malloc"); exit(1); }
  return (p);
}

void
usage(void)
{
  fprintf(stderr,
	  "usage: qsort_mt [-stv] [-f forkelements] [-h threads] [-n elements]\n"
	  "\t-l\tRun the libc version of qsort\n"
	  "\t-s\tTest with 20-byte strings, instead of integers\n"
	  "\t-t\tPrint timing results\n" "\t-v\tVerify the integer results\n"
	  "Defaults are 1e7 elements, 2 threads, 100 fork elements\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  bool opt_str = false;
  bool opt_time = false;
  bool opt_verify = false;
  bool opt_libc = false;
  int ch;
  size_t i;
  size_t nelem = 10000000;
  int threads = 2;
  int forkelements = 100;
  ELEM_T *int_elem = NULL;
  char *ep;
  char **str_elem;
  struct timeval start, end;
  struct rusage ru;

  gettimeofday(&start, NULL);
  while ((ch = getopt(argc, argv, "f:h:ln:stv")) != -1) {
    switch (ch) {
    case 'f':
      forkelements = (int) strtol(optarg, &ep, 10);
      if (forkelements <= 0 || *ep != '\0') {
	PWARN("illegal number, -f argument -- %s", optarg);
	usage();
      }
      break;
    case 'h':
      threads = (int) strtol(optarg, &ep, 10);
      if (threads < 0 || *ep != '\0') {
	PWARN("illegal number, -h argument -- %s", optarg);
	usage();
      }
      break;
    case 'l': opt_libc = true; break;
    case 'n':
      nelem = (size_t) strtol(optarg, &ep, 10);
      if (nelem <= 0 || *ep != '\0') {
	PWARN("illegal number, -n argument -- %s", optarg);
	usage();
      }
      break;
    case 's': opt_str = true; break;
    case 't': opt_time = true; break;
    case 'v': opt_verify = true; break;
    case '?': default: usage();
    }
  }

  if (opt_verify && opt_str)
    usage();

  argc -= optind;
  argv += optind;

  if (opt_str) {
    str_elem = (char **) xmalloc(nelem * sizeof(char *));
    for (i = 0; i < nelem; i++) {
      if (asprintf(&str_elem[i], "%d%d", rand(), rand()) == -1) {
	perror("asprintf"); exit(1);
      }
    }
  } else {
    int_elem = (ELEM_T *) xmalloc(nelem * sizeof(ELEM_T));
    for (i = 0; i < nelem; i++) { int_elem[i] = rand() % nelem; }
  }
  if (opt_str) {
    if (opt_libc) {
      qsort(str_elem, nelem, sizeof(char *), string_compare);
    } else {
      qsort_mt(str_elem, nelem, sizeof(char *),
	       string_compare, threads, forkelements);
    }
  } else {
    if (opt_libc) {
      qsort(int_elem, nelem, sizeof(ELEM_T), num_compare);
    } else {
      qsort_mt(int_elem, nelem, sizeof(ELEM_T), num_compare, threads,
	       forkelements);
    }
  }
  gettimeofday(&end, NULL);
  getrusage(RUSAGE_SELF, &ru);
#if DEBUG_SORT
  for (i = 0; i < nelem; i++) { fprintf(stderr, "%d ", int_elem[i]); }
  fprintf(stderr, "\n");
#endif
  if (opt_verify) {
    for (i = 1; i < nelem; i++) {
      if (int_elem[i - 1] > int_elem[i]) {
	fprintf(stderr, "sort error at position %d: "
		" %d > %d\n", i, int_elem[i - 1], int_elem[i]);
	exit(2);
      }
    }
  }
  if (opt_time) {
    printf("%.3g %.3g %.3g\n",
	   (end.tv_sec - start.tv_sec) +
	   (end.tv_usec - start.tv_usec) / 1e6,
	   ru.ru_utime.tv_sec + ru.ru_utime.tv_usec / 1e6,
	   ru.ru_stime.tv_sec + ru.ru_stime.tv_usec / 1e6);
  }
  return (0);
}
