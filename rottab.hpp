/*! \file rottab.hpp
 * \brief Rotation Lookup Table for Cosinus and Sinus.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "geometry/box.hpp"

/*! Radial Precision (in Binary Bits) of Animation Rotation. */
#define ROTTAB_Q_BITS (6)
#define ROTTAB_Q (1 << ROTTAB_Q_BITS)
#define ROTTAB_SZ (ROTTAB_Q * 4)
#define ROTANG_MIN (0)
#define ROTANG_MAX (ROTTAB_SZ-1)

/*! Angular Precision for Object Shape Rotations with range [0, ROTTAB_SZ-1].
 * \NOTE IMPORTANT: Make sure this type matches
 * ROTTAB_SZ and VATT_UINT8_ROTANG.
 */
typedef uint8_t ROTANG_t;

extern mat2f g_rottab[ROTTAB_SZ];

void rottab_init(void);

/*! Lookup the angle \p i.
 * \param[in] i can take negative or positive angles which are wrapped
 * to [0..N] before lookup is performed.
 */
inline pure mat2f rottab_cyclic_lookup(uint i) { return g_rottab[i % ROTTAB_SZ]; }

/*! Matrix-Vector (Self) Multiply, \p y = \p A * \p y;
 * \complexity 4 MUL, 2 ADD
 */
inline pure vec2f vec2f_rot_ROTANG(const vec2f& y, ROTANG_t rotang) { return vec2f_mvmul_mat2f(y, &g_rottab[rotang]); }
