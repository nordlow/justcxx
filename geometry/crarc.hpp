/*! \file crarc.hpp
 * \brief N-Dimensional Circular Ring Arc.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <tuple>
#include <boost/serialization/access.hpp>
#include "vec.hpp"
#include "box.hpp"
#include "sphere.hpp"
#include "sphere_shell.hpp"
#include "../atan2_adjusted.h"

/*! Unit Circle Quadrant Extremes Points/Vectors. */
template<class T>
void make_qext(vec2<T> g_qext[4]) {
    g_qext = { { +1.0f,  0.0f },
               {  0.0f, +1.0f },
               { -1.0f,  0.0f },
               {  0.0f, -1.0f } };
}

/*! N-Dimensional Circular Ring-Segment. */
template<class T>
class crarc : public std::tuple<cring<T>, T, T>
{
public:
    /*! Construct from Circular Ring \p cr, begin angle \p p and end angle \p q. */
    crarc(const cring<T>& cr_, T p_, T q_)
        : std::tuple<cring<T>, T, T>(cr_, p_, q_) {} // \old cring_setCrr

    box2<T> outer_box() const { return r().inner_box(); }
    box2<T> inner_box() const { return r().outer_box(); }

    cring<T>  r() const { return std::get<0>(*this); } ///< Circular Ring.
    cring<T>& r()       { return std::get<0>(*this); } ///< Circular Ring.
    T  ri() const { return std::get<1>(r()); }         ///< Inner Radius.
    T& ri()       { return std::get<1>(r()); }         ///< Inner Radius.
    T  ro() const { return std::get<2>(r()); }         ///< Outer Radius.
    T& ro()       { return std::get<2>(r()); }         ///< Outer Radious.
    T  p() const { return std::get<1>(*this); }        ///< Begin Angle.
    T& p()       { return std::get<1>(*this); }        ///< Begin Angle.
    T  q() const { return std::get<2>(*this); }        ///< End Angle.
    T& q()       { return std::get<2>(*this); }        ///< End Angle.

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & r() & p() & q(); }
};


/*! Inner Begin Point of \p a. */
template<class T> inline pure vec2<T> getip(const crarc<T>& a) { return a.r().c() + cossin(a.p()) * a.r().ri(); }
/*! Inner End Point of \p a. */
template<class T> inline pure vec2<T> getiq(const crarc<T>& a) { return a.r().c() + cossin(a.q()) * a.r().ri(); }
/*! Outer Begin Point of \p a. */
template<class T> inline pure vec2<T> getop(const crarc<T>& a) { return a.r().c() + cossin(a.p()) * a.r().ro(); }
/*! Outer End Point of \p a. */
template<class T> inline pure vec2<T> getoq(const crarc<T>& a) { return a.r().c() + cossin(a.q()) * a.r().ro(); }

/* ---------------------------- Group Separator ---------------------------- */

/*! Check if \p a lies beside \p b. */
template<class T> inline pure bool beside(const crarc<T>& a,
                                          const vec2<T>& b) {
    int ret = 0;
    /* radial clipping first because dist2() is faster than atan2() */
    if (beside(a.r(), b)) {
        ret = 1;			/* radial discardal/clipping */
    } else {
        vec2<T> diff; vec2<T>_diff(b, a.r().c(), diff); /* vector from center to point */
        T ang = atan2f_0to2PI(diff(1), diff(0));
        if (ang <  a.p() or
            ang >= a.q()) {		/* b beside angular range [p, q] */
            ret = 1;			/* angular discardal/clipping */
        } else {
            /* PTEST("Angle %f inside [%f %f]\n", ang, a.p(), a.q()); */
        }
    }
    return ret;
}

/*! Check if \p a covers \p b. */
template<class T> inline pure bool includes(const crarc<T>& a,
                                            const vec2<T>& b) { return not beside(a, b); }

/*! Check if \p a lies beside \p b. */
template<class T> inline pure bool beside(const crarc<T>& a,
                                          const box2<T>& b) {
    const vec2<T> vtx[4] = { { b.l(0), b.l(1) },
                             { b.u(0), b.l(1) },
                             { b.u(0), b.u(1) },
                             { b.l(0), b.u(1) } };
    /* circle beside all box-corners => lies beside the whole box */
    return (not includes(a, vtx[0]) and
            not includes(a, vtx[1]) and
            not includes(a, vtx[2]) and
            not includes(a, vtx[3]));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Resize \p a using the user (mouse) drag from \p mpos the shift \p mshift.
 * \param[in] mask Control Point Mask to use.
 * \return 1 if extents were changed, -1 if type not supported, 0 otherwise.
 */
template<class T>
inline pure crarc<T>shiftCtrlP(const crarc<T>& a,
                               const vec2<T>& mpos,
                               const vec2<T>& mshift,
                               const vec2<T>& mindim,
                               const uint8_t * mask)
{
    a.r() = shiftCtrlP(a.r(), mpos, mshift, mindim, mask);
    return a;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Return non-zero if \p a overlaps \p b. */
template<class T>
inline pure bool overlap(const crarc<T>& a,
                         const box2<T>& b)
{
    return not beside(a, b);
}

/*! Return non-zero if \p a completely covers \p b. */
template<class T>
inline pure bool includes(const crarc<T>& a,
                          const box2<T>& b)
{
    const vec2<T> vtx[4] = { { b.l(0), b.l(1) },
                             { b.u(0), b.l(1) },
                             { b.u(0), b.u(1) },
                             { b.l(0), b.u(1) } };
    return (includes(a, vtx[0]) and
            includes(a, vtx[1]) and
            includes(a, vtx[2]) and
            includes(a, vtx[3]));
}

/*! Area of \p a. */
template<class T>
inline pure T area(const crarc<T>& a) { return M_PI * (std::sqr(a.ro()()) -
                                                       std::sqr(a.ri()())); }

/*! Outer box of \p a. */
template<class T>
inline pure box2<T> outer_box(const crarc<T>& a) {
    /* calculate which quadrant extremes that the
     * circle segment with angular range [p, q] passes through */
    int ef[4];
    ef[0] = (a.p() <= M_Q0 and a.q() >= M_Q0);
    ef[1] = (a.p() <= M_Q1 and a.q() >= M_Q1);
    ef[2] = (a.p() <= M_Q2 and a.q() >= M_Q2);
    ef[3] = (a.p() <= M_Q3 and a.q() >= M_Q3);
    /* usage  */
    box2<T> box = unite(getop(a),
                        getoq(a),
                        getip(a),
                        getiq(a));
    vec2<T> g_qext[4];
    make_qext(g_qext);
    /* extend box with quadrant extremes */
    for (uint i = 0; i < 4; i++) {
        if (ef[i]) {
            box.include(a.r().c() + g_qext[i] * a.r().ri());
        }
    }
    return box;
}

#if 0
/*! Sub-Ring-Segment when \p ba is viewed relative to \p a.
 * The inverse operation of \c getRel().
 */
template<class T> inline pure crarc<T> getSub(const crarc<T>& ba,
                                              const box2<T>& a) {
    return box2<T>(getSub(a, ba.r(), b.r()), ba.p(),ba.q()); // copy angles as it
}
/*! Inverse Operation of \c getSub(). */
template<class T> inline crarc<T> getRel(const box2<T>& a,
                                         const crarc<T>& b,
                                         crarc<T>& ba) {
    cring<T>_getRel(a, &b.r(), &ba.r());
    setAA(ba, b.p(), b.q()); /* just copy segment angles */
}
#endif
