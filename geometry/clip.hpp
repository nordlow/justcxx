/*! \file clip.hpp
 * \brief Liang-Barsky Line Clipping.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "vec.hpp"
#include "box.hpp"
#include "line.hpp"

/*! Liang-Barsky Line Clip in 2D helping test function. */
template<class T, std::size_t N>
inline bool liang_barsky_clip_test(T p,
                                   T q,
                                   T& u1,
                                   T& u2)
{
    T r;
    bool ret = true;
    if (p < 0.0) {
        r = q / p;
        if (r > u2) {
            ret = false;
        } else {
            if (r > u1) {
                u1 = r;
            }
        }
    } else if (p > 0.0) {
        r = q / p;
        if (r < u1) {
            ret = false;
        } else if (r < u2) {
            u2 = r;
        }
    } else {       /* p == 0, so line is parallel to this clipping edge */
        if (q < 0.0) {
            /* line is outside clipping edge */
            ret = false;
        }
    }
    return ret;
}

/*! Liang-Barsky Line Clip in 2D.
 * \param[in] min Lower-Right Corner of the Clipping Box
 * \param[in] max Upper-Left Corner of the Clipping Box
 * \param[in] a and b Line to be clipped
 * \param[out] ra and rb Resulting Clipped Line upon return
 */
template<class T, std::size_t N>
bool liang_barsky_clip(vec2<T> min,
                       vec2<T> max,
                       vec2<T> a,
                       vec2<T> b,
                       vec2<T>& ra,
                       vec2<T>& rb)
{
    T u1 = 0.0, u2 = 1.0, d0 = b(0) - a(0), d1;

    vec2<T> d;
    d(0) = b(0) - a(0);
    if (liang_barsky_clip_test(-d0, a(0) - min(0), u1, u2)) {
        if (liang_barsky_clip_test(d0, max(0) - a(0), u1, u2)) {
            d1 = b(1) - a(1);
            if (liang_barsky_clip_test(-d1, a(1) - min(1), u1, u2)) {
                if (liang_barsky_clip_test(d1, max(1) - a(1), u1, u2)) {
                    if (u2 < 1.0) {
                        b(0) = a(0) + u2 * d0;
                        b(1) = a(1) + u2 * d1;
                    }
                    if (u1 > 0.0) {
                        a(0) += u1 * d0;
                        a(1) += u1 * d1;
                    }
                    ra = a;
                    rb = b;
                    return true;
                }
            }
        }
    }
    return false;
}

/*! Liang-Barsky line clip in 2D.
 * \param[in] min and max upper-left and lower-right corners of the Clipping Box.
 * \param[in] line Line to be clipped
 * \param[out] clipped_line Resulting Clipped Line upon return
 */
template<class T, std::size_t N>
inline bool liang_barsky_clip(vec2<T> min,
                              vec2<T> max,
                              line2<T> line,
                              line2<T>& clipped_line)
{
    return liang_barsky_clip(min, max,
                             line.a, line.b,
                             clipped_line.a, clipped_line.b);
}

/*! Liang-Barsky line clip in 2D.
 *
 * \param[in] box Clipping Box
 * \param[in] a and b Line to be clipped
 * \param[out] ra and rb Resulting Clipped Line upon return
 */
template<class T, std::size_t N>
inline bool liang_barsky_clip(box2<T> box,
                              vec2<T> a,
                              vec2<T> b,
                              vec2<T>& ra,
                              vec2<T>& rb)
{
    return liang_barsky_clip(box.l(), box.u(), a, b, ra, rb);
}

/*! Liang-Barsky line clip in 2D.
 *
 * \param[in] box Clipping Box
 * \param[in] line Line to be clipped
 * \param[out] clipped_line Resulting Clipped Line upon return
 */
template<class T, std::size_t N>
inline bool liang_barsky_clip(box2<T> box,
                              line2<T> line,
                              line2<T>& clipped_line)
{
    return liang_barsky_clip(box,
                             line.a, line.b,
                             clipped_line.a, clipped_line.b);
}
