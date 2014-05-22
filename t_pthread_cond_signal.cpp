#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
#include "check.h"

/* For safe condition variable usage, must use a boolean predicate and  */
/* a g_mutex with the condition.                                          */
int                 workToDo = 0;
pthread_cond_t      g_cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     g_mutex = PTHREAD_MUTEX_INITIALIZER;

#define NTHREADS      2

void *threadfunc(void *parm)
{
  int           rc;

  while (1) {
    /* Usually worker threads will loop on these operations */
    rc = pthread_mutex_lock(&g_mutex);
    checkResults("pthread_mutex_lock()\n", rc);

    while (not workToDo) {
      printf("Thread blocked\n");
      rc = pthread_cond_wait(&g_cond, &g_mutex);
      checkResults("pthread_cond_wait()\n", rc);
    }
    printf("Thread awake, finish work!\n");
    /* Under protection of the lock, complete or remove the work     */
    /* from whatever worker queue we have. Here its simply a flag    */
    workToDo = 0;

    rc = pthread_mutex_unlock(&g_mutex);
    checkResults("pthread_mutex_lock()\n", rc);
  }
  return NULL;
}

int main(int argc, char **argv)
{
  int                   rc=0;
  int                   i;
  pthread_t             threadid[NTHREADS];

  printf("Enter Testcase - %s\n", argv[0]);

  printf("Create %d threads\n", NTHREADS);
  for(i=0; i<NTHREADS; ++i) {
    rc = pthread_create(&threadid[i], NULL, threadfunc, NULL);
    checkResults("pthread_create()\n", rc);
  }

  sleep(5);  /* Sleep isn't a very robust way to serialize threads */

  for(i=0; i<5; ++i) {
    printf("Wake up a worker, work to do...\n");

    rc = pthread_mutex_lock(&g_mutex);
    checkResults("pthread_mutex_lock()\n", rc);

    /* In the real world, all the threads might be busy, and       */
    /* we would add work to a queue instead of simply using a flag */
    /* In that case the boolean predicate might be some boolean    */
    /* statement like: if (the-queue-contains-work)                */
    if (workToDo) {
      printf("Work already present, likely threads are busy\n");
    }
    workToDo = 1;
    rc = pthread_cond_signal(&g_cond);
    checkResults("pthread_cond_broadcast()\n", rc);

    rc = pthread_mutex_unlock(&g_mutex);
    checkResults("pthread_mutex_unlock()\n", rc);
    sleep(5);  /* Sleep isn't a very robust way to serialize threads */
  }

  printf("Main completed\n");
  exit(0);
  return 0;
}
