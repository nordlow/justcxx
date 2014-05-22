/*! \file atomic_ctypes.h
 * \brief Thread-safe Wrapper Containers for C Types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-01-25 10:20
 */

#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

typedef struct {
  float           _v;            /**< Value (private). */
  pthread_mutex_t _m;            /**< Mutex (private) */
} floatM_t;

/*!
 * Init Mutex-Protected float.
 * \param[out] mf Mutexed Float.
 * \param[in] v Value
 */
static inline void floatM_init(floatM_t * mf, float v)
{
  pthread_mutex_init(&mf->_m, 0);
  mf->_v = v;
}

/*!
 * Set Mutex-Protected float.
 * \param[in,out] mf Mutexed Float.
 * \param[in] v Value
 */
static inline int floatM_set(floatM_t * mf, float v)
{
  int ret = 0;
  if (pthread_mutex_lock(&mf->_m) == 0) {
    mf->_v = v;
    pthread_mutex_unlock(&mf->_m);
    ret = 1;
  }
  return ret;
}

/*!
 * Get Value of Mutex-Protected float.
 * \param[in,out] mf Mutexed Float.
 * \param[out] v_ret Value
 */
static inline int floatM_get(floatM_t * mf, float * v_ret)
{
  int ret = 0;
  if (pthread_mutex_lock(&mf->_m) == 0) {
    *v_ret = mf->_v;
    pthread_mutex_unlock(&mf->_m);
    ret = 1;
  }
  return ret;
}

static inline int floatM_add(floatM_t * mf, float vi)
{
  int ret = 0;
  if (pthread_mutex_lock(&mf->_m) == 0) {
    mf->_v += vi;
    pthread_mutex_unlock(&mf->_m);
    ret = 1;
  }
  return ret;
}

static inline int floatM_sub(floatM_t * mf, float vi)
{
  int ret = 0;
  if (pthread_mutex_lock(&mf->_m) == 0) {
    mf->_v -= vi;
    pthread_mutex_unlock(&mf->_m);
    ret = 1;
  }
  return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

typedef struct {
  int           _v;            /**< Value (private). */
  pthread_mutex_t _m;            /**< Mutex (private) */
} intM_t;

/*!
 * Init Mutex-Protected  int.
 * \param[in] v Value
 * \param[out] mf Mutexed Int.
 */
static inline void intM_init(intM_t * mf, int v)
{
  pthread_mutex_init(&mf->_m, 0);
  mf->_v = v;
}

static inline int intM_set(intM_t * mf, int v)
{
  int ret = 0;
  if (pthread_mutex_lock(&mf->_m) == 0) {
    mf->_v = v;
    pthread_mutex_unlock(&mf->_m);
    ret = 1;
  }
  return ret;
}

static inline int intM_add(intM_t * mf, int vi)
{
  int ret = 0;
  if (pthread_mutex_lock(&mf->_m) == 0) {
    mf->_v += vi;
    pthread_mutex_unlock(&mf->_m);
    ret = 1;
  }
  return ret;
}

static inline int intM_sub(intM_t * mf, int vi)
{
  int ret = 0;
  if (pthread_mutex_lock(&mf->_m) == 0) {
    mf->_v -= vi;
    pthread_mutex_unlock(&mf->_m);
    ret = 1;
  }
  return ret;
}

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600     /* Or: #define _BSD_SOURCE */
#endif
#include <unistd.h>

static inline int intM_inc(intM_t * mf)
{
  usleep(100000);
  return intM_add(mf, 1);
}

static inline int intM_dec(intM_t * mf)
{
  return intM_sub(mf, 1);
}

static inline int intM_get(intM_t * mf, int * v_ret)
{
  int ret = 0;
  if (pthread_mutex_lock(&mf->_m) == 0) {
    *v_ret = mf->_v;
    pthread_mutex_unlock(&mf->_m);
    ret = 1;
  }
  return ret;
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
