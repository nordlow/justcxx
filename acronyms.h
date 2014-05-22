/*!
 * \file acronyms.h
 * \brief Find acronym matches.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Print the words found in path that can be acronymed to acr.
 */
void find_acronyms(const char *path, char *acr, int force_first);

#ifdef __cplusplus
}
#endif
