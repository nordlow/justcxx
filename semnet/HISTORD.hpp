/*!
 * \file HISTORD.hpp
 * \brief History Order and History Order Increase.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "../utils.h"
#include "../pnw_types.h"

/* ========================================================================= */

/*!
 * History Order
 */
typedef uint32_t HORD_t;

#define HORD_undefined (UINT32_MAX)

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * History Order Increase.
 *
 * Specifies the increase of the history order (hord) of the log after a
 * history entry has been added to the history.
 */
typedef uint8_t HORD_INC_t;

/* ---------------------------- Group Separator ---------------------------- */
