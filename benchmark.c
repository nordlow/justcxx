#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static void benchmark_fork_do_nothing(void)
{
  int i;
  i = 0;
}

/*!
 * Benchmark process forking. call with nforks set to 50000.
 */
void benchmark_fork(int nforks)
{
  int pid, status;
  for (int i = 0; i < nforks; i++) {
    if ((pid = fork()) < 0 ) {	/*** error handling ***/
      printf ("fork failed with error code= %d\n", pid);
      exit(0);
    } else if (pid == 0) {      /*** this is the child of the fork ***/
      benchmark_fork_do_nothing();
      exit(0);
    } else {		      /*** this is the parent of the fork ***/
      waitpid(pid, status, 0);
    }
  }
}

static void *benchmark_pthread_do_nothing(void *null)
{
  int i;
  i = 0;
  pthread_exit(NULL);
}

/*!
 * Benchmark fork(). call with nforks set to 50000.
 */
void benchmark_pthread(int nthreads)
{
  pthread_t tid;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for (int i = 0; i < nthreads; i++) {
    int rc = pthread_create(&tid, &attr, benchmark_pthread_do_nothing, NULL);
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }

    /* Wait for the thread */
    rc = pthread_join(tid, NULL);
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
  }

  pthread_attr_destroy(&attr);
  pthread_exit(NULL);
}
