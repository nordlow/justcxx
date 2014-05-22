/*! \file pathops.h
 * \brief Operations on UNIX path patterns in \c xStr.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <stdlib.h>
#include "pathsep.h"
#include "xstr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PATH_LEN_CHECK(path, len) if (path != NULL && len == 0) { len = strlen(path); }
#define PATH_LEN_CHECK_XSTR(path) if (path->p.ch != NULL && path->l == 0) { ((xStr*)path)->l = strlen(path->p.ch); }

void pathops_xstr_init(void);

static inline int xstr_is_root_path(const xStr * path)
{
  if (path->wcF) {
    return (path->l == 1 &&
            path->p.wch[0] == PATH_SEP);
  } else {
    return (path->l == 1 &&
            path->p.ch[0] == PATH_SEP);
  }
}

static inline int xstr_is_absolute_path(const xStr * path)
{
  if (path->wcF) {
    return (path->l >= 1 && path->p.wch[0] == PATH_SEP);
  } else {
    return (path->l >= 1 && path->p.ch[0] == PATH_SEP);
  }
}

static inline int xstr_is_relative_path(const xStr * path)
{
  if (path->wcF) {
    return (path->l >= 1 && path->p.wch[0] == '.');
  } else {
    return (path->l >= 1 && path->p.ch[0] == '.');
  }
}

static inline int xstr_is_local_path(const xStr * path)
{
  return !(xstr_is_absolute_path(path) ||
           xstr_is_relative_path(path));
}

/*!
 * Convert a Relative path \p in to an Absolute path \p out.
 * \return 1 upon success, 0 otherwise.
 */
int xstr_is_realpath(const xStr * in, xStr * out);

/*!
 * Allocate and Create new path = \p cdir + '/' + \p sfil and return it.
 * \param[out] full
 * \param[in] cdir
 * \param[in] sfil
 */
void xstr_init_pathadd(xStr * full, const xStr * cdir, const xStr * sfil);

/* ---------------------------- Group Separator ---------------------------- */

void xstr_init_pathadd_atHOME(xStr * full,
                              const char * dsub, size_t slen,
                              const char * sfil, size_t flen);

#ifdef __cplusplus
}
#endif
