/*!
 * \file w2s.h
 * \brief World-to-Screen (W2S) Mapping Transformations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "vec2f.h"
#include "vec2s.h"
#include "box2f.hpp"
#include "box2s.h"
#include "linfilt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * \name Shifts to be added before conversion from float to short to specify
 * the type of truncation.
 */
#define TRUNC_DOWN (0.0f)	/**< Truncate down (floor). */
#define TRUNC_UP (0.5f)		/**< Truncate up (ceil). */

/*!
 * Map World Position posW to Screen Position posS.
 *
 * \param[in] wrp World Reference Point
 * \param[in] mapf Mapping Factor
 * \param[in] srp Screen Reference Point, which is set to NULL if at origo,
 *        or if non-NULL means an offset into pixmap or window we are currently
 *        drawing onto.
 * \param[in] trunc is truncation direction: zero means floor, non-zero means ceil.
 *
 * \TODO: Use \c fa2_ss (linfilt.h) instead to enable 3DNow! when possible.
 */
static inline pure
vec2s map_w2sPos(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
                 const vec2f * posW,
                 float trunc)
{
  const vec2f adj = { trunc,
                      trunc };	/* truncation adjustment */
  vec2s posS;
  if (srp == 0) {
    fa2_sst(&posS->x, &posW->x, &wrp->x, &mapf->x, &adj.x);
    /*     posS->x = (posW->x - wrp->x) * mapf->x + adj.x; */
    /*     posS->y = (posW->y - wrp->y) * mapf->y + adj.y; */
  } else {
    fa2_sst_sc(&posS->x, &posW->x, &wrp->x, &mapf->x, &adj.x, &srp->x);
    /*     posS->x = (posW->x - wrp->x) * mapf->x + adj.x; */
    /*     posS->y = (posW->y - wrp->y) * mapf->y + adj.y; */
    /*     posS->x += srp->x; */
    /*     posS->y += srp->y; */
  }
  return posS;
}

/*!
 * Map World Dimension \p dimW to Screen Dimension \p dimS.
 *
 * \param[in] dimW World Dimension
 * \param[in] mapf Mapping Factor
 * \param[out] dimS screen dimension to be calculate
 *
 * \TODO: Use \c fa2_ss (linfilt.h) instead to enable 3DNow! when possible.
 */
static inline pure vec2d
map_w2sDimf(const vec2f * dimW, const vec2f * mapf)
{
  vec2f dimS;
  dimS->x = (dimW->x) * mapf->x;
  dimS->y = (dimW->y) * mapf->y;
  return dimS;
}

/*!
 * Map World Box \p wb to <b>Exclusive</b> Screen Box \p sbox.
 *
 * \param[in] wrp World Reference Point
 * \param[in] mapf Mapping Factor
 * \param[in] srp Screen Reference Point, which is set to NULL if at origo
 * \return non-zero if screen box has a non-zero area.
 */
static inline int
map_w2sBoxEXL(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
              const box2f * wb, box2s * sbox)
{
  map_w2sPos(wrp, mapf, srp, &wb->l, &sbox->l, TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &wb->u, &sbox->u, TRUNC_DOWN);
  return (sbox->l.x < sbox->u.x &&
          sbox->l.y < sbox->u.y);
}

/*!
 * Map World Box \p wb to <b>Inclusive</b> Screen Box \p sbox.
 *
 * \param[in] wrp World Reference Point
 * \param[in] mapf Mapping Factor
 * \param[in] srp Screen Reference Point, which is set to NULL if at origo
 * \return non-zero if screen box has a non-zero area.
 */
static inline int
map_w2sBoxINC(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
              const box2f * wb, box2s * sbox)
{
  map_w2sPos(wrp, mapf, srp, &wb->l, &sbox->l, TRUNC_DOWN);
  map_w2sPos(wrp, mapf, srp, &wb->u, &sbox->u, TRUNC_UP);
  return (sbox->l.x < sbox->u.x &&
          sbox->l.y < sbox->u.y);
}

/*!
 * Map World Box \p wb to <b>Inclusive</b>-<b>Exclusive</b> Screen Box \p sbox.
 *
 * \param[in] wrp World Reference Point
 * \param[in] mapf Mapping Factor
 * \param[in] srp Screen Reference Point, which is set to NULL if at origo
 * \return non-zero if screen box has a non-zero area.
 */
static inline int
map_w2sBoxIX(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
             const box2f * wb, box2s * sbox)
{
  map_w2sPos(wrp, mapf, srp, &wb->l, &sbox->l, TRUNC_DOWN);
  map_w2sPos(wrp, mapf, srp, &wb->u, &sbox->u, TRUNC_DOWN);
  return (sbox->l.x < sbox->u.x &&
          sbox->l.y < sbox->u.y);
}

/*!
 * Map 3-edged World Polygon \p posW to Screen Polygon \p posS.
 */
static inline void
map_w2sPoly3(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
             const vec2f posW[3], vec2s posS[3])
{
  map_w2sPos(wrp, mapf, srp, &posW[0], &posS[0], TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &posW[1], &posS[1], TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &posW[2], &posS[2], TRUNC_DOWN);
}

/*!
 * Map 4-edged World Polygon \p posW to Screen Polygon \p posS.
 */
static inline void
map_w2sPoly4EXL(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
                const vec2f posW[4], vec2s posS[4])
{
  map_w2sPos(wrp, mapf, srp, &posW[0], &posS[0], TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &posW[1], &posS[1], TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &posW[2], &posS[2], TRUNC_DOWN);
  map_w2sPos(wrp, mapf, srp, &posW[3], &posS[3], TRUNC_DOWN);
}

/*!
 * Map 4-edged World Polygon \p posW to Screen Polygon \p posS.
 */
static inline void
map_w2sPoly4INC(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
                const vec2f posW[4], vec2s posS[4])
{
  map_w2sPos(wrp, mapf, srp, &posW[0], &posS[0], TRUNC_DOWN);
  map_w2sPos(wrp, mapf, srp, &posW[1], &posS[1], TRUNC_DOWN);
  map_w2sPos(wrp, mapf, srp, &posW[2], &posS[2], TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &posW[3], &posS[3], TRUNC_UP);
}

/*!
 * Map 4-edged World Polygon \p posW to Screen Polygon \p posS.
 */
static inline void
map_w2sPoly4up(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
               const vec2f posW[4], vec2s posS[4])
{
  map_w2sPos(wrp, mapf, srp, &posW[0], &posS[0], TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &posW[1], &posS[1], TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &posW[2], &posS[2], TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &posW[3], &posS[3], TRUNC_UP);
}

/*!
 * Map World Polygon \p posW to Screen Polygon \p posS.
 */
static inline void
map_w2sPoly5INC(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
                const vec2f posW[5], vec2s posS[5])
{
  map_w2sPos(wrp, mapf, srp, &posW[0], &posS[0], TRUNC_DOWN);
  map_w2sPos(wrp, mapf, srp, &posW[1], &posS[1], TRUNC_DOWN);
  map_w2sPos(wrp, mapf, srp, &posW[2], &posS[2], TRUNC_DOWN);
  map_w2sPos(wrp, mapf, srp, &posW[3], &posS[3], TRUNC_UP);
  map_w2sPos(wrp, mapf, srp, &posW[4], &posS[4], TRUNC_UP);
}

/*!
 * Map n-edged World Polygon \p posW to Screen Polygon \p posS.
 */
static inline void
map_w2sPolyNdown(const vec2f * wrp, const vec2f * mapf, const vec2s * srp,
                 const vec2f posW[], vec2s posS[], size_t n)
{
  for (size_t i = 0; i < n; i++) {
    map_w2sPos(wrp, mapf, srp, &posW[i], &posS[i], TRUNC_DOWN);
  }
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
