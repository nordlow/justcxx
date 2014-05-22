/*!
 * \file ptrarray_grid.h
 * \brief Creation and Destruction of arrays of pointers to shapes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

/* ========================================================================= */

#pragma once

#include "vis.hpp"

#ifdef __cplusplus
extern "C" {
#endif

vec2f ** ptrarrayvec2f_newGrid(float step, float dim, size_t m, size_t n, int random);
box2f ** ptrarraybox2f_newGrid(float step, float dim, size_t m, size_t n, int random);
vis_t ** ptrarrayvis_newGrid(float step, float dim, size_t m, size_t n, int random);
void ptrarrayvis_clrGrid(vis_t ** grid, size_t m, size_t n);

#ifdef __cplusplus
}
#endif
