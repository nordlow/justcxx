/*! \file CORNMASK_enum.hpp
 * \brief Box Corner Mask Type.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/*! Box Corner Mask.
 */
typedef enum {
  CORNMASK_BOTTOM_RIGHT = (1<<0), /**< Bottom-Right Corner. */
  CORNMASK_BOTTOM_LEFT  = (1<<1), /**< Bottom-Left Corner. */
  CORNMASK_TOP_LEFT     = (1<<2), /**< Top-Left Corner. */
  CORNMASK_TOP_RIGHT    = (1<<3), /**< Top-Right Corner. */

  CORNMASK_BOTTOM = (1<<0) | (1<<1), /**< Bottom-Left and -Right Corner. */
  CORNMASK_TOP    = (1<<2) | (1<<3), /**< Top-Left and -Right Corner. */
  CORNMASK_LEFT   = (1<<1) | (1<<2), /**< Left-Bottom and -Top Corner. */
  CORNMASK_RIGHT  = (1<<0) | (1<<3), /**< Right-Bottom and -Top Corner. */

  CORNMASK_FULL =   0xf,	/**< All four corners. */
} __attribute__ ((packed)) CORNMASK_t;
