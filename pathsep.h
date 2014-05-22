/*! \file pathsep.h
 * \brief Path Separator.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-04-23 19:49
 */

#pragma once
#if defined(HAVE_CONFIG_H)
#  include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*! Path Directory Separator */
#ifdef WIN32
#  define PATH_SEP ('\\')	/**< Windows */
#else
#  define PATH_SEP ('/')	/**< UNIX/Linux */
#endif

/*! Filesystem Root C String */
#ifdef WIN32
#  define PATH_ROOT_CSTR ("\\")	/**< Windows */
#else
#  define PATH_ROOT_CSTR ("/")	/**< UNIX/Linux */
#endif

/*! Environment Variable Separator */
#ifdef WIN32
#  define ENV_SEP (";")	/**< Windows */
#else
#  define ENV_SEP (":")	/**< UNIX/Linux */
#endif

#ifdef __cplusplus
}
#endif
