/*!
 * \file do_file.h
 * \brief File Utilities.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-04-04 12:48
 */

#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Show File \p filename using default file handler.
 * \param through_shell flags that we should execute through shell environment.
 * If \c do_file returns, execve  failed.
 * \return \c execve return value.
 */
static inline int do_file(const char * filename, char *envp[], const char * display, bool through_shell)
{
#ifdef WIN32
  HINSTANCE ret = 0;
  HWND hwnd = 0;              // handle to owner window.
  LPCTSTR operation = "";     /* verb to do, e.g., edit, open, print. */
  /* TODO: Use this */
  ret = ShellExecute(hwnd,
                     operation,
                     filename,
                     NULL,     // parameters if executable, else NULL.
                     NULL,     // working directory or NULL for current directory.
                     SW_SHOWNORMAL // window mode e.g., SW_HIDE, SW_SHOWNORMAL.
                     );
  return hit;
#else
  int ret = 0;
  // TODO: Check for precence of gnome-open in path.
  //const char * path = getenv("FILENAME");
  const char * opener = "/usr/bin/chromium-browser"; /* program name */

  /* environment */
#if 0
  const char * home = getenv("HOME"); /* home directory */
  if (!display) {                   /* Linux browsers requires display to be set */
    display = ":0.0";               /* default display */
  }
  const char * const envp[] = { "HOME", home,
                                "DISPLAY", display,  NULL }; /* environment */
#endif

  /* check environment */
  if (false) {
    for (size_t ei = 0; envp[ei]; ++ei) {
      printf("%s\n", envp[ei]);
    }
  }

  if (through_shell) {
    const char * const argv[] = { opener, opener, filename, NULL }; /* arguments */
    ret = execve("/bin/sh", (char * const*)argv, (char * const*)envp);
  } else {
    const char * const argv[] = { opener, filename, NULL }; /* arguments */
    ret = execve(opener, (char * const*)argv, (char * const*)envp);
  }
#endif
  return ret;
}

#ifdef __cplusplus
}
#endif
