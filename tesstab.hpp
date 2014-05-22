/*! \file tesstab.hpp
 * \brief Tesselation Lookup Table for Cosinus and Sinus.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "geometry/vec.hpp"

/*! Radial Precision (in Binary Bits) of Circle Quadrants Tessellation. */
#define TESS_Q_BITS (5)

/*! Radial Precision of Circle Quadrants Tessellation.
 * \NOTE IMPORTANT: For best performance set this to an even power of two,
 * that is 4, 8, 16, 32, 64, etc.
 */
#define TESS_Q (1 << TESS_Q_BITS)

/*! Radial Precision of all Four Quadrants Tessellation */
#define TESS_SZ_BITS (TESS_Q_BITS+2)
#define TESS_SZ (1 << TESS_SZ_BITS)

/*! Unit Circle Lookup Table. */
extern vec2f g_tesstab[TESS_SZ];

/*! Initialize internal radial lookuptable. */
void tesstab_init(void);

inline spure const vec2f tesstab_at(uint i) { return g_tesstab[i]; }
inline spure const vec2f tesstab_cat(uint i) { return g_tesstab[i % TESS_SZ]; }
