/*! \file layout.hpp
 * \brief Layouts.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <vector>
#include "box.hpp"
#include "crarc.hpp"
#include "../enforce.hpp"
#include "SLAY_enum.hpp"
#include "../cc_features.h"

/*! \em Horizontal Uniform Grid Layout \n Number of Boxes inside \p b with relative padding \p p. */
template<class T, std::size_t N> inline pure std::vector<box<T,N> > uhg_layout(const box<T,N>& b, size_t n, T p = 0.0)
{
    std::vector<box<T,N> > a(n);
    if (n == 0) { return a; }
    const T d = b.dim(0) / ((T)n + n*p + p); // dimension
    const T s = d * (1 + p);                 // step
    const T f = b.l(0) + d * p;             // first
    size_t c = 0;                           // counter
    for (auto& i : a) {
        i = box<T,N>(vec<T,N>{f + c*s,     b.l(1)},
                     vec<T,N>{f + c*s + d, b.u(1)});
        c++;
    }
    return a;
}

/*! \em Vertical Uniform Grid Layout \n Number of Boxes inside \p b with relative padding \p p. */
template<class T, std::size_t N>
inline pure std::vector<box<T,N> > uvg_layout(const box<T,N>& b, size_t n, T p = 0.0)
{
    std::vector<box<T,N> > a(n);
    if (n == 0) { return a; }
    const T d = b.dim(1) / ((T)n + n*p + p); // dimension
    const T s = d * (1 + p);                 // step
    const T f = b.l(1) + d * p;             // first
    size_t c = 0;                           // counter
    for (auto& i : a) {
        i = box<T,N>(vec<T,N>{b.l(0), f + c*s},
                     vec<T,N>{b.u(0), f + c*s + d});
        c++;
    }
    return a;
}

/*! Uniform \em Rectangular Column-Major-Order Grid Layout \n Number of Boxes inside \p b with relative padding \p p.
 * \param n0 defines the number columns.
 * \param n1 defines the number rows.
 */
template<class T, std::size_t N> inline pure std::vector<box<T,N> > urg_layout(const box<T,N>& b,
                                                                               size_t n, size_t n0, size_t n1, T p = 0.0) // FIXME: Add support for relative padding \p p.
{
    std::vector<box<T,N> > a(n);
    if (n == 0) { return a; }
    enforce_lt(n0 * n1, a.size());
    // iteration constants for calculating subrectangles
    const auto d = b.dim() / vec<T,N>{static_cast<T>(n0),
                                      static_cast<T>(n1)}; // dimension
    const auto pd = p * d;	// calculate padding from relative paddingding
    const auto s = d;		// step
    const auto f = b.l();	// first
    auto ci = begin(a);
    for (size_t i = 0; i < n1 and ci != end(a); i++) {
        for (size_t j = 0; j < n0 and ci != end(a); j++) {
            const auto cpos = f + vec<T,N>{j * s(0),
                                           i * s(1)};
            *ci = box<T,N>(cpos + pd,
                           cpos + d - pd);
            ci++;
        }
    }
    return a;
}

/*! \em Square Uniform Column-Major-Order Grid Layout \n Number of Boxes inside \p b with relative padding \p p */
template<class T, std::size_t N> inline pure std::vector<box<T,N> > usg_layout(const box<T,N>& b, size_t n, T p = 0.0)
{
    std::vector<box<T,N> > a(n);
    if (n == 0) { return a; }
    const size_t sn = static_cast<size_t>(std::ceil(std::sqrt((T) n))); // side number of items
    // dimension, step, first
    const auto d = b.dim() / (sn + sn*p + p);
    const auto s = d * (1 + p);
    const auto f = b.l() + d * p;
    auto ci = begin(a);
    for (size_t i = 0; i < sn and ci != end(a); i++) {
        for (size_t j = 0; j < sn and ci != end(a); j++) {
            const auto cpos = f + vec<T,N>{j * s(0),
                                           i * s(1)};
            *ci = box<T,N>(cpos,
                           cpos + d);
            ci++;
        }
    }
    return a;
}

#if HAVE_CXX11_EXTERN_TEMPLATES
extern template pure std::vector<box<float,2> > uhg_layout(const box<float,2>& b, size_t n, float p = 0.0);
extern template pure std::vector<box<float,2> > uvg_layout(const box<float,2>& b, size_t n, float p = 0.0);
extern template pure std::vector<box<float,2> > urg_layout(const box<float,2>& b, size_t n, size_t n0, size_t n1, float p = 0.0);
extern template pure std::vector<box<float,2> > usg_layout(const box<float,2>& b, size_t n, float p = 0.0);
extern template pure std::vector<box<double,2> > uhg_layout(const box<double,2>& b, size_t n, double p = 0.0);
extern template pure std::vector<box<double,2> > uvg_layout(const box<double,2>& b, size_t n, double p = 0.0);
extern template pure std::vector<box<double,2> > urg_layout(const box<double,2>& b, size_t n, size_t n0, size_t n1, double p = 0.0);
extern template pure std::vector<box<double,2> > usg_layout(const box<double,2>& b, size_t n, double p = 0.0);

extern template pure std::vector<box<float,3> > uhg_layout(const box<float,3>& b, size_t n, float p = 0.0);
extern template pure std::vector<box<float,3> > uvg_layout(const box<float,3>& b, size_t n, float p = 0.0);
extern template pure std::vector<box<float,3> > urg_layout(const box<float,3>& b, size_t n, size_t n0, size_t n1, float p = 0.0);
extern template pure std::vector<box<float,3> > usg_layout(const box<float,3>& b, size_t n, float p = 0.0);
extern template pure std::vector<box<double,3> > uhg_layout(const box<double,3>& b, size_t n, double p = 0.0);
extern template pure std::vector<box<double,3> > uvg_layout(const box<double,3>& b, size_t n, double p = 0.0);
extern template pure std::vector<box<double,3> > urg_layout(const box<double,3>& b, size_t n, size_t n0, size_t n1, double p = 0.0);
extern template pure std::vector<box<double,3> > usg_layout(const box<double,3>& b, size_t n, double p = 0.0);
#endif

/*! Layout the element \p idx of a total of \p num elements
 * in a ring around the parent \p master.
 */
template<class T> inline pure box2<T> box2f_lay1_RING(const box2<T>& master, size_t idx, size_t num, T radsz)
{
    const T fac = 1.5f / num;
    const T ang = (T)M_2PI * idx / num;
    auto dim = master.dim();
    auto sep = 0.75 * dim * cossin(ang); // separation
    auto srad = radsz * dim * fac;       // sub radius
    auto scen = master.cen() + sep;      // sub center
    return box2<T>(scen - srad,
                   scen + srad);
}

/*! Circular Layout of Ring-Segment "outside of" Circle \p master.
 * \param[in] aoff Angular Offset.
 * \param[in] alen Angular Length.
 * \param[in] apad Angular Padding.
 * \param[in] rpad Relative Padding to next slave ring.
 * \param[in] rrad Relative (Thickness) Radius of the next slave ring.
 */
template<class T> inline pure crarc<T> layOutside(const circle<T>& master,
                                                  T aoff, T alen, T apad,
                                                  T rpad, T rrad)
{
    return crarc_setCrraa(master.c(),
                          master.r * (1 + rpad),
                          master.r * (1 + rpad + rrad),
                          (aoff)            * M_2PI,
                          (aoff+alen - apad) * M_2PI);
}

/*! Circular Layout of Ring-Segment "outside of" Ring-Segment \p master.
 * \param[in] aoff Angular Offset.
 * \param[in] alen Angular Length.
 * \param[in] apad Angular Padding.
 * \param[in] rpad Relative Padding to next slave ring.
 * \param[in] rrad Relative (Thickness) radius of the next slave ring.
 */
template<class T> inline pure crarc<T> layOutside(const crarc<T>& master,
                                                  T aoff, T alen, T apad,
                                                  T rpad, T rrad)
{
    const T mthick = master.r.ro() - master.r.ri(); /* master thickness */
    const T aspan = master.q() - master.p();
    return crarc<T>(master.r.c(),
                    master.r.ro() * (1 + rpad),
                    master.r.ro() * (1 + rpad) + mthick * rrad,
                    master.p() + (aoff)             * aspan,
                    master.p() + (aoff+alen - apad) * aspan);
}

/*! Layout the element \p i of a total of \p num elements in a
 * \em Horizontal Grid at \p master.
 */
template<class T> inline pure box2<T> layHGrid(const box2<T>& master, size_t i, size_t num, T rpad)
{
    // if (num == 0) { PWARN("num is 0\n"); return; }
    T d = master.dim(0) / (static_cast<T>(num) + num * rpad + rpad); /* sub dimension */
    T s = d * (1 + rpad);       /* step */
    T f = master.l(0) + d * rpad; /* first */
    return box2<T>(f + i * s,
                   master.l(1),
                   f + i * s + d,
                   master.u(1));
}

/*! Layout the element \p i of a total of \p num elements in a
 * \em Vertical Grid at \p master.
 */
template<class T> inline pure box2<T> layVGrid(const box2<T>& master, size_t i, size_t num, T rpad)
{
    // if (num == 0) { PWARN("num is 0\n"); return; }
    T d = master.dim(1) / (static_cast<T>(num) + num * rpad + rpad); /* sub dimension */
    T s = d * (1 + rpad);			      /* step */
    T f = master.l(1) + d * rpad;		      /* first */
    return box2<T>(master.l(0),
                   f + i * s,
                   master.u(0),
                   f + i * s + d);
}

/*! Layout the element \p idxY, \p idxX of a total of \p num elements
 * in a \em Square Rectangular Grid at \p master.
 * \param[in] rpad relative padding along both x and y-dimension.
 */
template<class T> inline pure box2<T> laySGrid(const box2<T>& master, size_t idx, size_t num, T rpad)
{
    // if (num == 0) { PWARN("num is 0\n"); return; }

    auto dim = master.dim();    /* dimension */

    size_t sn = (size_t)ceil(sqrt((double)num)); /* side number of items */
    size_t idxY = idx / sn;
    size_t idxX = idx - idxY * sn;

    T dX = dim(0) / ((T)sn + sn * rpad + rpad); /* sub dimension */
    T pX = dX * rpad;				     /* padding */
    T sX = dX + pX;				     /* step */
    T fX = master.l(0) + pX;			     /* first */

    T dY = dim(1) / ((T)sn + sn * rpad + rpad); /* sub dimension */
    T pY = dY * rpad;				     /* padding */
    T sY = dY + pY;				     /* step */
    T fY = master.l(1) + pY;			     /* first */

    return box2<T>(fX + idxX * sX,
                   fY + idxY * sY,
                   fX + idxX * sX + dX,
                   fY + idxY * sY + dY);
}

/*! Return Element indexed by \p idxX, \p idxY of a total of \p masterN
 * elements in \p master in a rectangular grid at \p master.
 * \param[in] rpad Relative Padding along both X and Y-dimension.
 */
template<class T, std::size_t N>
inline pure box<T,N> layRGrid(const box<T,N>& master,
                              size_t masterN,
                              vec<size_t,N> idx,
                              vec<size_t,N> num,
                              T rpad)
{
    // if (masterN == 0) { PWARN("masterN is 0\n"); return; }
    const vec<T,N> d = master.dim(0) / (static_cast<T>(num) + num * rpad + rpad); /* sub dimension */
    const vec<T,N> p = d * rpad;                              /* padding */
    const vec<T,N> s = d + p;                                 /* step */
    const vec<T,N> f = master.l() + p;                        /* first */
    return box<T,N>(f + idx * s,
                    f + idx * s + d);
}

/*! Linearly Layout the element at index \p i starting at \p master.
 * \param[in] offR Relative Offset.
 * \param[in] stepR Relative Step.
 * \param[in] dimR Relative Dimension.
 */
template<class T, std::size_t N>
inline pure box<T,N> layLin(const box<T,N>& master, size_t i,
                            const vec<T,N>& offR,
                            const vec<T,N>& stepR,
                            const vec<T,N>& dimR)
{
    const auto mdimA = master.dim();   // absolute master dimension
    const auto offA = mdimA * offR;  // absolute offset
    const auto stepA = mdimA * stepR; // absolute step
    const auto dimA = mdimA * dimR; // absolute dimension
    const auto pos = master.l() + offA + i * stepA;
    return box<T,N>(pos, pos + dimA);
}

/*! Layout the element \p idx of a total of \p num elements in a \c SLAY-layout at \p master.
 */
template<class T> inline pure box2<T> layAny(const box2<T>& master,
                                             size_t idx,
                                             size_t num,
                                             T rpad, SLAY_t slay)
{
    switch (slay) {
    case SLAY_RING: return lay1_RING(master, idx, num, 1.0); break;
        // case SLAY_SUNBURST: PTODO("handle SLAY_SUNBURST\n");
        //     /* layCRArc(master, idx, num, 1.0, slave); */
        //     break;
    case SLAY_HGRID: return lay1_HGRID(master, idx, num, rpad); break;
    case SLAY_VGRID: return lay1_VGRID(master, idx, num, rpad); break;
    default: return *master; break;
    }
}
