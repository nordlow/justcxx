/*!
 * \file FILE_SHOW_enum.h
 * \brief File Presentation Type
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-08-20 01:15
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Type of File Visualization.
 */
typedef enum {
  FILE_SHOW_AUTO,		/**< Visualize automatically. */
  FILE_SHOW_TEXT,		/**< Visualize as Text. */
  FILE_SHOW_TEXTURE,		/**< Visualize as Texture. */
  FILE_SHOW_HIST		/**< Visualize as Histogram. */
} __attribute__ ((packed)) FILE_SHOW_t;

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
