/*!
 * \file HISTNAV_enum.hpp
 * \brief History Navigation Type Code.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

typedef enum {
  HISTNAV_PREV,
  HISTNAV_NEXT,
  HISTNAV_FIRST,
  HISTNAV_LAST,
} __attribute__ ((packed)) HISTNAV_t;
