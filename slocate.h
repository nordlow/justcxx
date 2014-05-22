/*!
 * \file slocate.h
 * \brief Calls (s)locate and Collects its Output from stdout.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-08-23 14:11
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Calls (s)locate with argument \p arg and Collects its Output from
 * stdout.
 * \return collected output, which should be released with free().
 */
char * slocate_call(const char * arg);

#ifdef __cplusplus
}
#endif
