#include "../utils.h"
#include "../timing.h"

#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int
thread_run(pthread_t * t, void *(*start_routine) (void *), void *arg)
{
  pthread_attr_t *attr = NULL;	/* Use default attributes. */
  int code = pthread_create(t, attr, start_routine, arg);
  if (code < 0) {
    lperror("pthread_create");
  }
  return code;
}

void *
thread_join(pthread_t * t)
{
  void **ret = NULL;		/* thread_return */
  pthread_join(*t, ret);
  return *ret;
}

int
thread_kill(pthread_t * t)
{
  int sig = SIGKILL;		/* signal number */
  return pthread_kill(*t, sig);
}

void *
fun(void *arg)
{
  int i = *((int *) arg);
  printf("pthread_t %d created\n", i);
  sleep(i);
  free(arg);
  printf("pthread_t %d completed\n", i);
  return NULL;
}

void
test_pthreads(void)
{
  int n = 16;
  pthread_t t[n];

  for (int i = 0; i < n; i++) {
    int *dyn_i = malloc(sizeof(int));
    *dyn_i = i + 1;
    thread_run(t + i, fun, (void *) dyn_i);
  }

  sleep(n);
}

const int x = 1;

void
test_mutex_lock_unlock(void)
{
  pTimer pt; ptimer_init(&pt, CLOCK_REALTIME);

  ptimer_tic(&pt);

  pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
  int n = 8*1024*1024;
  for (int i = 0; i < n; i++) {
    pthread_mutex_lock(&mut);
    pthread_mutex_unlock(&mut);
  }
  ptimer_toc(&pt);

  printf("%d locks took ", n); ptimer_print_sec_usec9(pt); printf("[s]\n");
  printf("one lock took %g[ns]\n", (double)ptimer_get_nsec(&pt) / n);
}

int
main(int argc, char *argv[])
{
  test_mutex_lock_unlock();
  test_pthreads();
  return 0;
}
