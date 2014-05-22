/*! \file render.hpp
 * \brief Rendering of Shapes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "geometry/shape.hpp"

template<class T> int shape2<T>::render(const box2<T>& win, gr2d::GImage& gimg) const
{
    if (not shdata)
    {
        return 0;
    }

    switch (shtype.get_value())
    {
    case RECT:
    {
        PNOTE("xXXXXXXXXXXXXXXXX\n");

        const box2<T>& rect = get_dataref<box2<T> >();

        cout << "shape: " << *this << endl;
        cout << "win: " << win << endl;

        const vec2<T>& wd = win.dim();
        cout << "wd: " << wd << endl;

        const vec2<T> id(gimg.get_width(), gimg.get_height());
        cout << "id: " << id << endl;

        // relative box in "texture" coordinates
        const box2<T>& rb = box2<T>((rect.l() - win.l()) / wd * id,
                                    (rect.u() - win.l()) / wd * id);

        cout << "rb: " << rb << endl;

        // set whole pixel values
        int i0 = static_cast<int>(std::ceil(rb.l(0)));
        int i1 = static_cast<int>(std::ceil(rb.l(1)));

        int j0 = static_cast<int>(std::floor(rb.u(0)));
        int j1 = static_cast<int>(std::floor(rb.u(1)));

        printf("shape:%p, region is i0:%d i1:%d j0:%d j1:%d\n",
               this, i0, i1, j0, j1);

        if (i0 < j0 and
            i1 < j1)
        {
            int n0 = j0 - i0;
            int n1 = j1 - i1;

            printf("region is n0:%d n1:%d\n", n0, n1);

            gimg.set_region(i0, i1, n0, n1, shcol);
        }

        cout << endl;
        // set partial pixel values

        break;
    }
    case ARC:
        break;
    case CIRCLE:
        break;
    case LINE:
        break;
    case POLY:
        break;
    default:
        break;
    }

    return 0;
}
