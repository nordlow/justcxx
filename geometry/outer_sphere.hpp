/*! \file outer_sphere.hpp
 * \brief An Efficient Bounding Sphere.
 * \author Jack Ritter
 * from "Graphics Gems", Academic Press, 1990
 * Routine to Calculate near-optimal Bounding Sphere over a set of points in 3D.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Generalize to N Dimensions.
 */

#pragma once
#include <algorithm>
#include <limits>
#include <cmath>
#include "../mean.hpp"
#include "sphere.hpp"

/*! Near-Optimal Outer Bounding Sphere over a set of \p num points at \p points in 3D.
 * \param[in] points Points
 * \param[in] num Number of Points
 * \return Bounding Sphere
 */
template<class T, std::size_t N>
inline pure
sphere<T,N> outer_sphere(const vec<T,N>* points, size_t num)
{
    vec<T,N> xmin, xmax;         // TODO: xbnd
    vec<T,N> ymin, ymax;         // TODO: ybnd
    vec<T,N> zmin, zmax;         // TODO: zbnd

    /* first pass: find 6 minima / maxima points */
    /* ===================================================================== */

    /* initialize for min/max compare */
    auto lmax = std::numeric_limits<T>::max();
    xmin(0) = ymin(1) = zmin(2) = +lmax;
    xmax(0) = ymax(1) = zmax(2) = -lmax;

    for (size_t i = 0; i < num; i++) {
        auto point = points[i];
        if (point(0) < xmin(0)) xmin = point;	/* new xminimum point */
        if (point(0) > xmax(0)) xmax = point;
        if (point(1) < ymin(1)) ymin = point;
        if (point(1) > ymax(1)) ymax = point;
        if (point(2) < zmin(2)) zmin = point;
        if (point(2) > zmax(2)) zmax = point;
    }

    /* Set span = distance between the 2 points xmin & xmax (squared) */
    T xspan = sqrnorm(xmax-xmin);
    T yspan = sqrnorm(ymax-ymin);
    T zspan = sqrnorm(zmax-zmin);

    /* set points dia1 & dia2 to the maximally separated pair */
    auto dia1 = xmin;
    auto dia2 = xmax;		/* assume xspan biggest */
    T maxspan = xspan;
    if (yspan > maxspan) {
        maxspan = yspan;
        dia1 = ymin;
        dia2 = ymax;
    }
    if (zspan > maxspan) {
        dia1 = zmin;
        dia2 = zmax;
    }

    auto cen = pnw::mean(dia1, dia2); /* initial center, dia1,dia2 is a diameter of initial sphere */

    /* calculate initial radius ** 2 and radius */
    auto diff = dia2-cen;
    auto rad_sq = sqrnorm(diff);
    auto rad = std::sqrt(rad_sq);

    /* second pass: increment current sphere */
    std::for_each(points, points+num, [&cen,&rad,&rad_sq](const vec<T,N> & point) {
            auto diff_ = point-cen;
            auto old_to_p_sq = sqrnorm(diff_);

            if (old_to_p_sq > rad_sq) {	/* do r**2 test first *//* this point is outside of current sphere */
                auto old_to_p = std::sqrt(old_to_p_sq);

                /* update radius */
                rad = (rad + old_to_p) / 2.0;
                rad_sq = rad * rad;      /* for next r**2 compare */

                auto old_to_new = old_to_p - rad;
                cen = (rad * cen + old_to_new * point) / old_to_p; /* update center */
            }
        });
    return sphere<T,N>(cen, rad);
}
