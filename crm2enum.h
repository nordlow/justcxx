/*!
 * \file crm2enum.h
 * \brief Cyclic Random Mix of Two Enumerations
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Cyclic Random Mix of Two Enumerations
 *
 * In: bb and cc are permutations of [0 ... n-1].
 * Out: a is the resulting mixed permutation
 *
 * \return -1 if either bb or/and cc is not a permutation
 */
int intarray_crm2enum(int *a, const int *bb, const int *cc, size_t n);

#ifdef __cplusplus
}
#endif
