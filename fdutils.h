/*!
 * \file fdutils.h
 * \brief Various helper functions for operations on file descriptors.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#include <unistd.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Set file descriptor fd in blocking mode.
 */
static inline int
blockfd(int fd)
{
  long arg;
#ifdef VXWORKS
  arg = 0;
  if ((arg = ioctl(fd, FIONBIO, (int) &arg)) < 0) { /* VxWorks use ioctl */
    lperror("ioctl FIONBIO");
    return -1;
  }
#else
  if ((arg = fcntl(fd, F_GETFL, 0)) < 0) { /* get all bits */
    lperror("fcntl F_GETFL");
    return -1;
  }
  arg &= ~O_NONBLOCK;		/* clear bit */
  if (fcntl(fd, F_SETFL, arg) < 0) {  /* put back all bits */
    lperror("fcntl F_SETFL, O_NONBLOCK");
    return -1;
  }
#endif
  return 0;
}

/*!
 * Set file descriptor fd in non-blocking mode.
 */
static inline int
unblockfd(int fd)
{
  long arg;
#ifdef VXWORKS
  arg = 1;
  if ((arg = ioctl(fd, FIONBIO, (int) &arg)) < 0) { /* VxWorks use ioctl */
    lperror("ioctl FIONBIO");
    return -1;
  }
#else
  if ((arg = fcntl(fd, F_GETFL, 0)) < 0) { /* get all bits */
    lperror("fcntl F_GETFL");
    return -1;
  }
  arg |= O_NONBLOCK;		/* set bit */
  if (fcntl(fd, F_SETFL, arg) < 0) { /* put back all bits */
    lperror("fcntl F_SETFL, O_NONBLOCK");
    return -1;
  }
#endif
  /* Is this enough? */
  /*      fcntl(fd, F_SETFL, O_NONBLOCK); */
  return 0;
}

/*!
 * Return non-zero if file descriptor fd is in blocking mode.
 */
static inline int is_blocking(int fd)
{
  int x;
  if ((x = fcntl(fd, F_GETFL, 0)) < 0) {
    lperror("fcntl F_GETFL");
  }
  return ((x & O_NONBLOCK) == 0);
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
