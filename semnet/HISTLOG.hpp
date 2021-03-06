/*!
 * \file HISTLOG.hpp
 * \brief History Logging Type Code.
 * \author Copyright (C) 2012 Per Nlogl�w (per.nloglow@gmail.com)
 */

#pragma once
#include "../utils.h"

/* ========================================================================= */

/*!
 * History Logging Type Code.
 */
typedef enum {
    HISTLOG_SKIP,               ///< Skip logging to history.
    HISTLOG_KEEP,               ///< Log but keep history order.
    HISTLOG_POSTINC,            ///< Log and then forward history order.
    HISTLOG_PREINC              ///< Forward history order and then Log.
} __attribute__ ((packed)) HISTLOG_t;
