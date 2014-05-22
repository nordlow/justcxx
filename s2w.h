/*!
 * \file s2w.h
 * \brief Screen-to-World (S2W) Mapping Transformations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "vec2f.h"
#include "vec2s.h"
#include "box2f.hpp"
#include "box2s.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Map Screen Position \p posS to world position.
 *
 * \param[in] wrp World Reference Point
 * \param[in] imapf Mapping Factor
 * \param[in] srp Screen Reference Point, which is set to NULL if at origo
 * \return World Position
 */
static inline vec2f
map_S2W_Pos(const vec2f * wrp, const vec2f * imapf, const vec2s * srp,
            vec2s posS)
{
  vec2f polyW;
  if (srp) {
    posS.x -= srp->x;
    posS.y -= srp->y;
  }
  polyW->x = (float)posS.x * imapf->x + wrp->x;
  polyW->y = (float)posS.y * imapf->y + wrp->y;
  return polyW;
}

/*! Map Screen Box \p sbox to world box.
 *
 * \param[in] wrp World Reference Point
 * \param[in] mapf Mapping Factor
 * \param[in] srp Screen Reference Point, which is set to NULL if at origo
 *
 * \return World Box.
 */
static inline box2f
map_S2W_Box(const vec2f * wrp, const vec2f * imapf, const vec2s * srp,
            const box2s * sbox)
{
  box2f boxW;
  map_S2W_Pos(wrp, imapf, srp, sbox->l, &boxW->l);
  map_S2W_Pos(wrp, imapf, srp, sbox->u, &boxW->u);
  return boxW;
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
