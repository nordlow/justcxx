/*!
 * \file cpumult.h
 * \brief Detect Number of CPUs/Processors/Cores Online on the System.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-09-11 22:35
 */

#pragma once

#include <unistd.h>
#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Detect the number of CPUs/Processors/Cores Online.
 * \return number of CPUs found.
 */
static inline int get_CPUmult(void)
{
  int num = 1;			/* default to one CPU */
#if defined(__APPLE__)
  size_t len = sizeof(num);
  int mib[2];
  mib[0] = CTL_HW;
  mib[1] = HW_NCPU;
  if (sysctl(mib, 2, &num, &len, 0, 0) < 0 ||
      len != sizeof(num)) {
    num = 1;
  }
#elif defined(_SC_NPROCESSORS_ONLN)
  num = sysconf(_SC_NPROCESSORS_ONLN);
#endif
  return num;
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
