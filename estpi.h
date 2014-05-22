/*!
 * \file estpi.h
 * \brief Estimate pi.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#ifdef HAVE_GMP_H
#include <gmp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_LIBGMP
/*!
 * Estimate pi.
 */
int mpf_estimate_pi(mpf_t pi);
#endif

#ifdef __cplusplus
}
#endif
