/*!
 * \file pathops.h
 * \brief Operations on UNIX path patterns in C-strings.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include "pathsep.h"
#include "xstr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PATH_LEN_CHECK(path, len) if (path != NULL && len == 0) { len = strlen(path); }

static inline pure bool cstr_is_root_path(const char * path)   { return (path[0] == PATH_SEP && path[1] == '\0'); }
static inline pure bool cstr_is_absolute_path(const char* path) { return (path[0] == PATH_SEP && path[1] != '\0'); }
static inline pure bool cstr_is_relative_path(const char* path) { return (path[0] == '.'      && path[1] != '\0'); }
static inline pure bool cstr_is_local_path(const char* path)    { return ! (cstr_is_absolute_path(path) ||
                                                                                             cstr_is_relative_path(path)); }

static inline pure bool nstr_is__root_path(const char * path, size_t len) { return (len == 1 && path[0] == PATH_SEP); }

static inline pure bool cstr_is_DorDD_path(const char* path)
{
  return ((path[0] == '.' && path[1] == '\0') ||
          (path[0] == '.' && path[1] == '.' && path[2] == '\0'));
}

static inline pure bool nstr_is_DorDD_path(const char* path, size_t len)
{
  return ((len == 1 && path[0] == '.') ||
          (len == 2 && path[0] == '.' && path[1] == '.'));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Append path sfil to cdir and return the resulting cdir. */
char * cstr_pathapp(char * cdir, const char * sfil);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Allocate and Create new path = \p cdir + '/' + \p sfil and return it.
 * \param[out] full_len_ret is full return value
 * \return the resulting path, that is later released with \c free()
 */
char * cbuf_pathadd(const char * cdir, size_t clen,
                    const char * sfil, size_t flen,
                    size_t *full_len_ret);

/*! Allocate and Create new path = \p cdir + '/' + \p sfil and return it.
 * \return the resulting path, that is later released with \c free()
 */
pure char * cstr_pathadd(const char * cdir, const char * sfil);

/* ---------------------------- Group Separator ---------------------------- */

/*! Allocate and Create new path = \p cdir + '/' + \p dsub + '/' + \p sfil and return it.
 * \return the resulting path, that is later released with \c free()
 */
pure char * cbuf_pathadd3(const char * cdir, size_t clen,
                                           const char * dsub, size_t slen,
                                           const char * sfil, size_t flen);

/*! Allocate and Create new path = \p cdir + '/' + \p dsub + '/' + \p sfil and return it.
 * \return the resulting path, that is later released with \c free()
 */
pure char * cstr_pathadd3(const char * cdir, const char * dsub, const char * sfil);

/* ---------------------------- Group Separator ---------------------------- */

pure char * cbuf_pathadd2_atHOME(const char * dsub, size_t slen);

/* ---------------------------- Group Separator ---------------------------- */

/*! Allocate and Create new path = \c $HOME + '/' + \p dsub + '/' + \p sfil and return it.
 * \return the resulting path, that is later released with \c free()
 */
pure char * cstr_pathadd_atHOME(const char * dsub, const char * sfil);

pure char * cbuf_pathadd_atHOME(const char * dsub, size_t slen,
                                                 const char * sfil, size_t flen);

/* ---------------------------- Group Separator ---------------------------- */

/*! Decode Full Path \p full into Parent Directory and Local Directory/File.
 *
 * \param coff Offset to Parent Directory.
 * \param clen Length of Parent Directory.
 *
 * \param soff Offset to Local File/Directory.
 * \param slen Length of Local File/Directory.
 */
int pathparsePL(const char * bufF, size_t lenF,
                int *coff, int *clen,
                int *soff, int *slen);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Maximum Version Depth in archive filenames.
 */
#define VER_DEPTH_MAX (8)

/*!
 * Parse Local File Name of Source Code Package \p buf of length \p size into
 * basename, version, archive-type and compression type.
 *
 * \param[in] buf Contains Whole Package Name.
 * \param[in] size Length of Whole Package Name.

 * \param[out] base_off Offset to \em base part of name.
 * \param[out] base_len Length of \em base part of name.
 * \param[out] ver_offs Offsets to \em version parts of name.
 * \param[out] ver_lens Lengths of \em version parts of name.
 * \param[out] ver_depth Number of \em sub-versions of name.
 * \param[out] ar_off Offset to \em archive part of name.
 * \param[out] ar_len Length of \em archive part of name.
 * \param[out] zip_off Offset to \em compression part of name.
 * \param[out] zip_len Length of \em compression part of name.
 */
int cbuf_pathparseSRCV(const char * buf, int size,
                       int * base_off, int * base_len,
                       int ver_offs[VER_DEPTH_MAX],
                       int ver_lens[VER_DEPTH_MAX],
                       int * ver_depth,
                       int * ar_off, int * ar_len,
                       int * zip_off, int * zip_len);

void test_pathparseSRCV_cstr(const char * buf);

/* ---------------------------- Group Separator ---------------------------- */

/*! Expand ~user to the user's home directory. */
char * cstr_path_tilde_expand(const char *dirname);

#ifdef __cplusplus
}
#endif
