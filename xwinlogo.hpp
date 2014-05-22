/*!
 * \file xwinlogo.hpp
 * \brief Generate an X Window System Logo.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "vec.hpp"
#include "line.hpp"

template<class T>
inline T
compute_inverse_slope (line2<T> & l)
{
    return ((l.b(0) - l.a(0)) /
            (l.b(1) - l.a(1)));
}

template<class T>
inline T
compute_x_intercept(line2<T> & l, T inverse_slope)
{
    return (l.a(0)) - inverse_slope * l.a(1);
}

template<class T>
static void
xlogo_intersect(line2<T> & l1, line2<T> & l2, vec2<T> & intersection)
{
    T check;
    /*
     * x = m1y + b1
     * x = m2y + b2
     * m1y + b1 = m2y + b2
     * y * (m1 - m2) = b2 - b1
     * y = (b2 - b1) / (m1 - m2)
     */
    T  m1 = compute_inverse_slope (l1);
    T  b1 = compute_x_intercept (l1, m1);
    T  m2 = compute_inverse_slope (l2);
    T  b2 = compute_x_intercept (l2, m2);

    intersection(1) = (b2 - b1) / (m1 - m2);
    intersection(0) = m1 * intersection(1) + b1;

    check = m2 * intersection(1) + b2;

    if (check >= intersection(0))
    {
        check -= intersection(0);
    }
    else
    {
        check = intersection(0) - check;
    }

    if (check > (1/(T)(1<<16)))
    {
        fprintf(stderr, "intersect: intersection is off by %f\n", check);
    }
}

/*!
 * Draw the "official" X Window System Logo, designed by Danny Chong
 *
 * Written by Ollie Jones, Apollo Computer
 *
 * Does some fancy stuff to make the logo look acceptable even
 * if it is tiny.  Also makes the various linear elements of
 * the logo line up as well as possible considering rasterization.
 *
 * Munged to draw anti-aliased logo using Render extension.
 * Carl Worth, 2002-05-16
 */
template<!!class T>
void
gen_xlogo(T x, T y, T width, T height,
          vec2<T> poly_left[6],
          vec2<T> poly_right[6])
{
    T side;
    T thin, thick, gap, d31;
    line2<T> thick_left, thick_right, thin_left, thin_right, gap_left, gap_right;

    /* for now, do a centered even-sized square, at least for now */
    side = width;
    if (height < width)
    {
        side = height;
    }

    x += (width - side) / 2;
    y += (height - side) / 2;

    thin = (side / 11.0);	/* width of the thin line */
    thick = (side / 4.0);	/* width of the thick line */
    gap = thin / 4.0;		/* the gap between the lines */
    d31 = thin + thin + gap;

    thick_left.a(0) = x;		thick_left.a(1) = y;
    thick_left.b(0) = x + side - thick;	thick_left.b(1) = y + side;

    thick_right.a(0) = x + thick;	thick_right.a(1) = y;
    thick_right.b(0) = x + side;	thick_right.b(1) = y + side;

    thin_left.a(0) = x + side-d31;	thin_left.a(1) = y;
    thin_left.b(0) = x + 0;		thin_left.b(1) = y + side;

    thin_right.a(0) = x + side;		thin_right.a(1) = y;
    thin_right.b(0) = x + d31;		thin_right.b(1) = y + side;

    gap_left.a(0) = x + side-( thin+gap);	gap_left.a(1) = y;
    gap_left.b(0) = x + thin;			gap_left.b(1) = y + side;

    gap_right.a(0) = x + side- thin;	gap_right.a(1) = y;
    gap_right.b(0) = x + thin + gap;	gap_right.b(1) = y + side;

    // left polygon
    poly_left[0] = thick_left.a;
    poly_left[1] = thick_right.a;
    xlogo_intersect(thick_right, gap_left, poly_left[2]);
    poly_left[3] = gap_left.b;
    poly_left[4] = thin_left.b;
    xlogo_intersect(thick_left, thin_left, poly_left[5]);

    // right polygon
    poly_right[0] = thin_right.a;
    poly_right[1] = gap_right.a;
    xlogo_intersect(thick_left, gap_right, poly_right[2]);
    poly_right[3] = thick_left.b;
    poly_right[4] = thick_right.b;
    xlogo_intersect(thick_right, thin_right, poly_right[5]);
}
