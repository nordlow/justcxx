/*!
 * \file GODIR_enum.hpp
 * \brief Go Direction.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/*!
 * Go (Navigation) Direction Type Code.
 */
typedef enum {
    GODIR_LEFT,			///< Go left.
    GODIR_RIGHT,			///< Go right.

    GODIR_UP,			///< Go up.
    GODIR_DOWN,			///< Go down.

    GODIR_PREV,                   ///< Go to (tree-traverse) previous (Forward). 
    GODIR_NEXT,			///< Go to (tree-traverse) next (Backward).
} __attribute__ ((packed)) GODIR_t;
