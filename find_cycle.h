/*!
 * \file find_cycle.h
 * \brief Find Minimum (Shortest) or Maximum (Longest) Cycle/Loop in C arrays.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t
intarray_find_min_cycle(const int * x, ssize_t stride, size_t n,
			ssize_t * off_ret, size_t * len_ret);

size_t
intarray_find_max_cycle(const int * x, ssize_t stride, size_t n,
			ssize_t * off_ret, size_t * len_ret);

#ifdef __cplusplus
}
#endif
