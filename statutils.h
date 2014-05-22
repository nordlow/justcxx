/*! \file statutils.h
 * \brief Helper Functions to decode file stats.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-12-04 18:49
 */

#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uid_t g_uid; /**< \em Cached return from \c getuid() for \em performance. */
extern gid_t g_gid; /**< \em Cached return from \c getgid() for \em performance. */

/*! Initialize the values of \c g_uid and \c g_gid.
 *
 * IMPORTANT: MUST BE CALLED BEFORE USING ANY OTHER FUNCTIONS IN THIS
 * HEADER!
 */
void statutils_init_uid_gid(void);

/*! Check whether stat \p a says readable by me.
 */
static inline int stat_is_readable(const struct stat *a) {
  int ret = 0;
  if ((a->st_mode & S_IROTH) != 0) {
    ret = 1;
  } else if (a->st_uid == g_uid) {  /* matches user id */
    ret = (a->st_mode & S_IRUSR) != 0;
  } else if (a->st_gid == g_gid) { /* matches group id */
    ret = (a->st_mode & S_IRGRP) != 0;
  }
  return ret;
}

/*! Check whether stat \p a says writable by me.
 */
static inline int stat_is_writable(const struct stat *a) {
  int ret = 0;
  if ((a->st_mode & S_IWOTH) != 0) {
    ret = 1;
  } else if (a->st_uid == g_uid) {  /* matches user id */
    ret = (a->st_mode & S_IWUSR) != 0;
  } else if (a->st_gid == g_gid) { /* matches group id */
    ret = (a->st_mode & S_IWGRP) != 0;
  }
  return ret;
}

/*! Check whether stat \p a says executable by me.
 */
static inline int stat_is_executable(const struct stat *a) {
  int ret = 0;
  if ((a->st_mode & S_IXOTH) != 0) {
    ret = 1;
  } else if (a->st_uid == g_uid) {  /* matches user id */
    ret = (a->st_mode & S_IXUSR) != 0;
  } else if (a->st_gid == g_gid) { /* matches group id */
    ret = (a->st_mode & S_IXGRP) != 0;
  }
  return ret;
}

/*! Check whether stat \p a says \em link file.
 */
static inline int stat_is_symbolic_link(const struct stat *a) {
  return S_ISLNK(a->st_mode);
}

/*! Check whether stat \p a says \em regular file.
 */
static inline int stat_is_regular(const struct stat *a) {
  return S_ISREG(a->st_mode);
}

#ifdef __cplusplus
}
#endif
