/*!
 * \file sysio.h
 * \brief Handling of System Related Stuff.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * xsysinfo uses
 * - /proc/meminfo
 * - /proc/loadavg
 * - /proc/stat
 * - /proc/swaps
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef linux

/*!
 * Determine the Load Average.
 *
 * \return load average as a floating point between 0 and 1.
 *
 * Found this from the source of xload (rxload-yymmdd.tar.gz)
 */
static inline double
get_loadavg(void)
{
  double val;			/* load average value */
  static int fd = -1;
  int n;
  char buf[10];

  if (fd < 0) {
    if (fd == -2 || (fd = open("/proc/loadavg", O_RDONLY)) < 0) {
      fd = -2;
      val = -1.0;
      return;
    }
  } else
    lseek(fd, 0, 0);

  if ((n = read(fd, buf, sizeof(buf) - 1)) > 0 &&
      sscanf(buf, "%lf", &val) == 1) {
    return val;
  }

  return -1.0;
}

#endif

#ifdef __cplusplus
}
#endif
