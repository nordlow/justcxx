/*!
 * \file fhsutils.h
 * \brief File System Hierarchy Standard (FHS) Utils
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-12-11 17:21
 */

#pragma once

#include "xstr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Constants Mapping to standard FHS Linux/UNIX paths.
 */
typedef enum {
  FHS_PATH___,
  FHS_PATH_bin,
  FHS_PATH_boot,
  FHS_PATH_dev,
  FHS_PATH_etc,
  FHS_PATH_etc_opt,
  FHS_PATH_etc_X11,
  FHS_PATH_etc_sgml,
  FHS_PATH_etc_xml,
  FHS_PATH_home,
  FHS_PATH_lib,
  FHS_PATH_mnt,
  FHS_PATH_media,
  FHS_PATH_opt,
  FHS_PATH_proc,
  FHS_PATH_root,
  FHS_PATH_sbin,
  FHS_PATH_tmp,
  FHS_PATH_srv,
  FHS_PATH_usr,
  FHS_PATH_usr_bin,
  FHS_PATH_usr_include,
  FHS_PATH_usr_lib,
  FHS_PATH_usr_sbin,
  FHS_PATH_usr_share,
  FHS_PATH_usr_src,
  FHS_PATH_usr_X11R6,
  FHS_PATH_usr_local,
  FHS_PATH_var,
  FHS_PATH_undefined_
} __attribute__((packed)) FHS_PATH_t;

/*!
 * Copied from FHS.
 */
const char * FHS_PATH_lookupDoc(FHS_PATH_t vpath);

FHS_PATH_t cbuf_lookup_FHS_PATH(const char * pathG, size_t lenG);
FHS_PATH_t cstr_lookup_FHS_PATH(const char * pathG);
FHS_PATH_t csc_lookup_FHS_PATH(const csc & pathG);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
