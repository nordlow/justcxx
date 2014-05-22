#include <stdio.h>
#include <stdlib.h>

int ptpool_init(ptpool_t * ppool, int num_threads)
{
  ppool->total_tasks = 0;
  ppool->request_errors = 0;
  ppool->task_queue_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
  ppool->active_task = PTHREAD_COND_INITIALIZER;
  ppool->thr_id = NULL;
  ppool->threads = NULL;

  ppool->first_tasks = NULL;
  ppool->last_task = NULL;

  for (int iterator = 0; iterator < num_threads; iterator++) {
    thr_id[iterator] = iterator;
    pthread_create(&threads[iterator], NULL, ptpool_handle_requests_loop,
                   (void *)&thr_id[iterator]);
  }

  return 0;
}

void *ptpool_add_task(int task_num, pthread_mutex_t* mutex, pthread_cond_t* cond, void (*funp)(void))
{
  int ret = 0;

  ptask_t* enqueue_task = (ptask_t*)malloc(sizeof(ptask_t));
  if (!enqueue_task) {
    request_errors++;
    return;
  }

  enqueue_task->task_id = task_num;
  enqueue_task->funp = funp;
  enqueue_task->next = NULL;

  ret = pthread_mutex_lock(mutex);

  if (total_tasks == 0) {
    first_task = enqueue_task;
    last_task = enqueue_task;
  } else {
    last_task->next = enqueue_task;
    last_task = enqueue_task;
  }

  total_tasks++;

  ret = pthread_mutex_unlock(mutex);

  ret = pthread_cond_signal(cond);
}

ptask_t* ptpool_get_task(pthread_mutex_t* mutex)
{
  ptask_t* task = NULL;
  int ret = pthread_mutex_lock(mutex);
  if (total_tasks > 0) {
    task = first_task;
    first_task = task->next;

    if (first_task == NULL) {
      last_task = NULL;
    }
    total_tasks--;
  } else {
    task = NULL;
  }
  ret = pthread_mutex_unlock(mutex);
  return task;
}

void ptpool_execute_task(ptask_t* task, int thread_id)
{
  if (task) {
  }
}

void* ptpool_handle_requests_loop(void* data)
{
  int thread_id = *((int*)data);

  int ret = pthread_mutex_lock(&task_queue_mutex);
  while (1) {
    if (total_tasks > 0) {
      ptask_t* task = ptpool_get_task(&task_queue_mutex);
      if (task) {
        ret = pthread_mutex_unlock(&task_queue_mutex);
        ptpool_execute_task(task, thread_id);
        free(task);
        ret = pthread_mutex_lock(&task_queue_mutex);
      } else {
        ret = pthread_cond_wait(&active_task, &task_queue_mutex);
      }
    }
  }
  return NULL;
}

void ptpool_test_thread_function()
{
  printf("Hello\n");
}

int ptpool_test()
{
  ptpool_init(8);
  void (*x)(void) = ptpool_test_thread_function;
  for (int i = 0; i < 11; i++) {
    ptpool_add_task(i, &task_queue_mutex, &active_task, *x);
  }
  return 0;
}
