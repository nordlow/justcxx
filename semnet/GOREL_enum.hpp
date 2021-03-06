/*!
 * \file GOREL_enum.hpp
 * \brief Navigation (Go) Relation Type Code.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/*!
 * Navigation (Go) Relation Type Code.
 */
typedef enum {
    GOREL_BEG,			///< Absolute (to beginning) indexing.
    GOREL_END,			///< Absolute (to end) indexing.
    GOREL_REL,			///< Relative indexing.
    GOREL_CYC			///< Cyclic (Relative) indexing.
} __attribute__ ((packed)) GOREL_t;
