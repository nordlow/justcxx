/*! \file VREC_enum.hpp
 * \brief Recursion Type Code.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/*! Network/Graph \em Recursion Type Code.
 * \c VREC_BRING and \c VREC_BOTTOM are currently only used when doing
 * recursive layouts.
 */
typedef enum {
  /*!
   * \em No recursion.
   */
  VREC_none = 0,

  /*! Bring children \em relative to parents extents.
   */
  VREC_BRING = 1,

  /*! Perform same operation (layout) all the way down.
   */
  VREC_BOTTOM = 2,

  /*!
   * Layout only top level, then preserve the relative positions to
   * the CHILDren for the other sublevels.
   */
  VREC_TOP = 3,
} __attribute__ ((packed)) VREC_t;

/*!
 * Lookup Recursion Type vrec name as a C-string.
 * \return C-string, 0 NULL if type was unknown.
 */
static inline const char *
VREC_getName(VREC_t vrec)
{
  const char * str = NULL;
  switch (vrec) {
  case VREC_none: str = "None"; break;
  case VREC_BRING: str = "Bring"; break;
  case VREC_BOTTOM: str = "Bottom"; break;
  case VREC_TOP: str = "Top"; break;
  }
  return str;
}
