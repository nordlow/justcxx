/*!
 * \file pthreadpool.h
 * \brief Threadpool builtin using POSIX Threads (pthreads) C API.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2010-02-17 09:20
 */

#pragma once

/*!
 * \brief Threadpool \em Task (linked list element).
 */
typedef struct task
{
  int task_id;
  void (*funp)(void); /**< Thread function. */
  struct task* next;  /**< Next task. */
} ptask_t;

/*!
 * \brief \em Threadpool.
 */
typedef struct
{
  int             total_tasks;  /**< Total \em number of tasks. */
  int             request_errors; /**< Request Errors. */

  pthread_mutex_t task_queue_mutex; /**< Mutex protectecting task queue. */
  pthread_cond_t  active_task;

  int             thr_id[];     /**< Thread IDs. */
  pthread_t       threads[];  /**< Thread Array. */

  ptask_t*        first_tasks;  /**< First Task in linked list of tasks. */
  ptask_t*        last_task;    /**< Last Task in linked list of tasks. */
} ptpool_t;
