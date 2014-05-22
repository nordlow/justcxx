/*! \file LOD_tess.hpp
 * \brief Level-Of-Detail (LOD) Shape Tesselations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Lookup indexes are all positive because C/C++ is unable to handle
 */

#pragma once

#include "tesstab.hpp"
#include "geometry/gen_shapes.hpp"
#include "geometry/vec.hpp"

/*! Checks that an LOD-cosin lookup range i to j index range is correct.
 * More specifically that \p i <= j and that j < TESS_SZ.
 */
#ifdef PNW_OLD_DEBUG_CHECK_INDEX
#define PNW_OLD_DEBUG_CHECK_TESS_INDEX(i, j, mLOD)                      \
    if (i > j) {                                                        \
        PNW_OLD__ERR("index i:%d is > j:%d!\n", i, j);                  \
        return -1;                                                      \
    }                                                                   \
    if (j * mLOD > TESS_SZ) {                                           \
        PNW_OLD__ERR("index (j:%d * mLOD:%d) is > %d!\n", i, mLOD, TESS_SZ); \
        return -1;                                                      \
    }
#else
#define PNW_OLD_DEBUG_CHECK_TESS_INDEX(i, j, mLOD)
#endif

/* ---------------------------- Group Separator ---------------------------- */

/*! Tesselate Ellipse Segment at Origo with Radius \p rad.
 * \param[out] vtx contains the vertices to be tesselated
 * \return number of vertices in \p vtx tesselated (\p vN)
 */
size_t
tessEllSeg_R(vec2f * vtx,
             const vec2f& rad,
             size_t vi, size_t vN, uint mLOD);

/* ---------------------------- Group Separator ---------------------------- */

/*! Tesselate Ellipse Segment at Center \p cen with Radius \p rad.
 * \param[out] vtx contains the vertices to be tesselated
 * \return number of vertices in \p vtx tesselated (\p vN)
 */
size_t
tessEllSeg_CR(vec2f * vtx,
              const vec2f& cen, const vec2f& rad,
              size_t vi, size_t vN, uint mLOD);

/*! Tesselate Ellipse Segment at Center \p cen with Radius \p rad rotated angle rotang.
 * \param[out] vtx contains the vertices to be tesselated
 * \return number of vertices in \p vtx tesselated (\p vN)
 */
size_t
tessEllSeg_CRa(vec2f * vtx,
               const vec2f& cen,
               const vec2f& rad, ROTANG_t rotang,
               size_t vi, size_t vN, uint mLOD);

/* ---------------------------- Group Separator ---------------------------- */

/*! Tesselate Ellipse Segment Sine Flower at Center \p cen with Radius \p rad.
 * \param[out] vtx contains the vertices to be tesselated
 * \return number of vertices in \p vtx tesselated (\p vN)
 */
size_t
tessEllSegSineFlower_CR(vec2f * vtx,
                        const vec2f& cen, const vec2f& rad,
                        float amp, int frq, float phs,
                        size_t vi, size_t vN, uint mLOD);

/*! Tesselate Ellipse Segment Sine Flower at Center \p cen with Radius \p rad rotated angle rotang.
 *
 * \param[out] vtx contains the vertices to be tesselated
 * \return number of vertices in \p vtx tesselated (\p vN)
 */
size_t
tessEllSegSineFlower_CRa(vec2f * vtx,
                         const vec2f& cen, const vec2f& rad,
                         float amp, int frq, float phs,
                         ROTANG_t rotang,
                         size_t vi, size_t vN, uint mLOD);

/* ---------------------------- Group Separator ---------------------------- */

/*! Tesselate Ellipse Frame Segment at Center \p cen between Inner Radius \p radI and Outer Radius \p radO.
 *
 * \param[out] vtx contains the vertices to be tesselated
 * \return number of vertices in \p vtx tesselated (\p vN)
 */
size_t
tessEllSeg_CRR(vec2f * vtx,
               const vec2f& cen,
               const vec2f& radI, const vec2f& radO,
               size_t vi, size_t vN, uint mLOD);

/*! Tesselate Ellipse Frame Segment at Center \p cen between Inner Radius \p radI and Outer Radius \p radO.
 *
 * \param[out] vtx contains the vertices to be tesselated
 * \return number of vertices in \p vtx tesselated (\p vN)
 */
size_t
tessEllSeg_CRRa(vec2f * vtx,
                const vec2f& cen,
                const vec2f& radI, const vec2f& radO,
                ROTANG_t rotang,
                size_t vi, size_t vN, uint mLOD);

/* ---------------------------- Group Separator ---------------------------- */

/*! "Point-Tesselate" Ellipse Frame Segment at Center \p cen between Inner Radius \p radI and Outer Radius \p radO.
 *
 * \param[out] vtx contains the vertices to be tesselated
 * \return 1, for similarity with functions returning vN
 */
inline int
tesspEllSeg_CRR(vec2f * vtx,
                const vec2f& cen,
                const vec2f& radI, const vec2f& radO,
                float ang)
{
    auto udir = cossin(ang);
    vtx[0] = cen + udir * radI;
    vtx[1] = cen + udir * radO;
    return 1;
}

/*! "Point-Tesselate" Ellipse Frame Segment at Center \p cen between Inner Radius \p radI and Outer Radius \p radO.
 *
 * \param[out] vtx contains the vertices to be tesselated
 * \return 1, for similarity with functions returning vN
 */
inline int
tesspEllSeg_CRRa(vec2f * vtx,
                 const vec2f& cen,
                 const vec2f& radI, const vec2f& radO,
                 ROTANG_t rotang,
                 float ang)
{
    auto udir = cossin(ang);
    vec2f_mvmul_mat2f(&udir, &g_rottab[rotang]);
    vtx[0] = cen + udir * radI;
    vtx[1] = cen + udir * radO;
    return 1;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Tesselate Ellipse Frame Segment from
 * Center cenI with Inner Radius \p radI to
 * Center cenO with Outer Radius \p radO.
 *
 * \param[out] vtx contains the vertices to be tesselated
 * \return number of vertices in \p vtx tesselated (\p vN)
 */
size_t
tessEllSeg_CCRR(vec2f * vtx,
                const vec2f& cenI, const vec2f& cenO,
                const vec2f& radI, const vec2f& radO,
                size_t vi, size_t vN, uint mLOD);
