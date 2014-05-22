/*! \file ubox.hpp
 * \brief N-Dimensional Uniformly Sided Box.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <boost/serialization/access.hpp>
#include "../math_x.hpp"
#include "sphere.hpp"
#include "../cc_features.h"

/*! N-Dimensional Uniformly Sided Box. */
template<class T, std::size_t N>
class ubox : public sphere<T,N>
{
public:
    ubox(const vec<T,N>& c, T r) : sphere<T,N>(c, r) {}
    box<T,N> outer_box() const { return box<T,N>(this->c() - this->r(),
                                                 this->c() + this->r()); }
    box<T,N> inner_box() const { return outer_box(); }

    template<class S> friend pure S concept_name(const ubox& m) { S s("0-Dimensional Uniformly Side Box"); s[0] += N; return s; }
    template<class S> friend pure S typenameof(const ubox& a) { S s("ubox<T,N>"); return s; }

    friend std::ostream& show        (std::ostream& os, const ubox& a) { os << "(c" << MEMCH << a.c() << ",r" << MEMCH << a.r() << ")"; return os; }
    friend std::ostream& operator << (std::ostream& os, const ubox& a) { return show(os, a); }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & this->c() & this->r(); }
};

#if HAVE_CXX11_TEMPLATE_ALIASES
template<class T> using   sqr = ubox<T,2>;
template<class T> using ubox3 = ubox<T,3>;
#endif

// Area
template<class T>                inline pure T area(const sqr<T>&   a) { return      std::sqr(a.r()); }
template<class T>                inline pure T area(const ubox3<T>& a) { return 6 * (std::sqr(a.r())); }
// Volume
template<class T>                inline pure T volume(const ubox3<T>&  a) { return std::cube(a.r()); }
template<class T, std::size_t N> inline pure T volume(const ubox<T,N>& a) { return std::pow(a.r(), N); }

/*! Resize \p a using the user (mouse) drag from \p mpos the shift \p mshift.
 * \param[in] mask Control Point Mask to use.
 */
template<class T>
inline sqr<T> shiftCtrlP(const sqr<T>& a,
                         const vec2<T>& mpos,
                         const vec2<T>& mshift,
                         const vec2<T>& mindim,
                         const uint8_t * mask)
{
    if (mask) {
        if (pnw::get_bit(*mask, 0)) { a.c() += mshift; }
        if (pnw::get_bit(*mask, 1)) { a.r() -= dot(normalize(a.c() - mpos), mshift); }
    }
    return a;
}

#if 0
/*! Inner Square of \p a scaled the factor \p f. */
template<class T, std::size_t N>
inline pure sqr<T,N> box2f_rdISqr_sqr2f(const box<T,N>& a, T f)
{
    sqr<T,N> b;
    b.c() = a.cen();
    auto dim = a.dim()
        b.r = f*std::min(dim(0), dim(1))/2;
    return b;
}

/*! Horizontally Translate \p a the shift sx. */
inline void htransl(sqr<T,N> a, T sx, sqr<T,N>& b) { b.c(0) = a.c(0) + sx; }

/*! Vertically Translate \p a the shift sy. */
inline void vtransl(const sqr<T,N>& a, T sy, sqr<T,N>& b) { b.c(1) = a.c(1) + sy; }

/*! Scale \p a the factor \p f. */
inline void scale(const sqr<T,N> a, T f, sqr<T,N>& b) { b.r = f*a.r; }

/* ---------------------------- Group Separator ---------------------------- */

inline bool beside(const sqr<T,N>& a,
                   const vec<T,N>& b)
{
    return (vec2f_dimdistmax(&a.c, b) > a.r);
}

inline bool includes(const sqr<T,N>& a,
                     const vec<T,N>& b)
{
    return not beside(a, b);
}

/*! Return non-zero if \p a lies beside \p b. */
inline bool beside(const sqr<T,N>& a,
                   const box<T,N>& b)
{
    PTEST("Function not verified\n");
    const vec<T,N> vtx[4] = { { b.l(0), b.l(1) },
                              { b.u(0), b.l(1) },
                              { b.u(0), b.u(1) },
                              { b.l(0), b.u(1) } };
    /* square beside all box-corners => lies beside the whole box */
    return (not includes(a, vtx[0]) and
            not includes(a, vtx[1]) and
            not includes(a, vtx[2]) and
            not includes(a, vtx[3]));
}

/*! Return non-zero if \p a completely includes \p b. */
inline bool includes(const sqr<T,N>& a,
                     const box<T,N>& b)
{
    PTEST("Function not verified\n");
    const vec<T,N> vtx[4] = { { b.l(0), b.l(1) },
                              { b.u(0), b.l(1) },
                              { b.u(0), b.u(1) },
                              { b.l(0), b.u(1) } };
    /* square includes all box-corners => includes the whole box */
    return (includes(a, vtx[0]) and
            includes(a, vtx[1]) and
            includes(a, vtx[2]) and
            includes(a, vtx[3]));
}

/*! Fit Square \p sqr precisely Inside (Bounding) Box extents \p ext.
 * \return 1 if extents were changed, -1 if type not supported, 0 otherwise.
 */
inline int fit_in(sqr<T,N>& sqr,
                  const box<T,N> ext)
{
    int ret = 1;
    auto cen = ext.cen();
    auto dim = ext.dim();
    set(sqr, cen(0), cen(1),
        std::min(dim(0), dim(1))/2); /* use inner square */
    return ret;
}
#endif
