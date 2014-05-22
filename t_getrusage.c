#include <sys/time.h>
#include <sys/resource.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*!
 * \brief Get Pages allocated by process in bytes.
 * \return pagese allocated by process in bytes
 */
static inline long
rusage_page_bytes(void)
{
  long ret = -1;
  struct rusage ru;
  int retR = getrusage(RUSAGE_SELF, &ru);
  if (retR == 0) {
    ret = ru.ru_minflt*sysconf(_SC_PAGESIZE);
  } else {
#ifdef DEBUG
    perror("getrusage()");
#endif
  }
  return ret;
}

/*!
 * \brief Print \em Resource \em Usage Statistics.
 *
 * \param[in] who can be either \c RUSAGE_SELF or \c RUSAGE_CHILDREN.
 * The former asks for resources used by the current process, the
 * latter for resources used by those of its children that have
 * terminated and have been waited for.
 *
 * \return value returned from \c getrusage()
 */
int print_rusage(int who)
{
  switch (who) {
  case RUSAGE_SELF: printf("RUSAGE_SELF:\n"); break;
  case RUSAGE_CHILDREN: printf("RUSAGE_CHILDREN:\n"); break;
  }
  struct rusage ru;
  int retR = getrusage(who, &ru);
  if (retR == 0) {
    printf("ru_utime    xxx (user time used)\n");
    printf("ru_stime    xxx (system time used)\n");
    printf("ru_maxrss   %ld (maximum resident set size)\n", ru.ru_maxrss);
    printf("ru_ixrss    %ld (integral shared memory size)\n", ru.ru_ixrss);
    printf("ru_idrss    %ld (integral unshared data size)\n", ru.ru_idrss);
    printf("ru_isrss    %ld (integral unshared stack size)\n", ru.ru_isrss);
    printf("ru_minflt   %ld (page reclaims) (%ld bytes)\n", ru.ru_minflt, ru.ru_minflt*sysconf(_SC_PAGESIZE));
    printf("ru_majflt   %ld (page faults)\n", ru.ru_majflt);
    printf("ru_nswap    %ld (swaps)\n", ru.ru_nswap);
    printf("ru_inblock  %ld (block input operations)\n", ru.ru_inblock);
    printf("ru_oublock  %ld (block output operations)\n", ru.ru_oublock);
    printf("ru_msgsnd   %ld (messages sent)\n", ru.ru_msgsnd);
    printf("ru_msgrcv   %ld (messages received)\n", ru.ru_msgrcv);
    printf("ru_nsignals %ld (signals received)\n", ru.ru_nsignals);
    printf("ru_nvcsw    %ld (voluntary context switches)\n", ru.ru_nvcsw);
    printf("ru_nivcsw   %ld (involuntary context switches)\n", ru.ru_nivcsw);
    printf("pages:%ld bytes\n", rusage_page_bytes());
  } else {
#ifdef DEBUG
    perror("getrusage()");
#endif
  }
  return retR;
}

int
main(int argc, char *argv[])
{
  print_rusage(RUSAGE_SELF);
  size_t num = 1024*1024;
  size_t len = 1;
  char * x[num];                /* array of pointers to arrays */

  /* allocate */
  printf("allocating...\n");
  for (size_t i = 0; i < num; i++) { x[i] = malloc(len); }

  print_rusage(RUSAGE_SELF);

  /* free */
  printf("freeing...\n");
  for (size_t i = 0; i < num; i++) { free(x[i]); }

  print_rusage(RUSAGE_SELF);

  return 0;
}
