/*! \file rbox.hpp
 * \brief N-Dimensional Rounded Box.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <utility>
//#include <boost/math/constants/constants.hpp>
#include <boost/serialization/access.hpp>
#include "box.hpp"
#include "../bitwise.hpp"
#include "CORNMASK_enum.hpp"
#include "CORNSTYLE_enum.hpp"

/*! N-Dimensional Rounded Box. */
template<class T, std::size_t N>
class rbox : public std::pair<box<T,N>,
                              vec<T,N> > {
public:
    rbox(const box<T,N>& b_ = { 0, 1 },
         const vec<T,N>& r_ = 1,
         CORNMASK_t cornmask_ = CORNMASK_FULL)
        : std::pair<box<T,N>, vec<T,N>> (b_,r_),
          m_cornmask(cornmask_),
          m_cornstyle(CORNSTYLE_ELL_MIN) {}
    rbox(const vec<T,N>& l_,
         const vec<T,N>& u_,
         const vec<T,N>& r_ = 1, CORNMASK_t cornmask_ = CORNMASK_FULL)
        : rbox(box<T,N>(l_,u_), r_, cornmask_) {}
    rbox(T lx,T ly, T ux,T uy, T rx,T ry, CORNMASK_t cornmask_ = CORNMASK_FULL)
        : rbox(vec2<T>(lx,ly), vec2<T>(ux,uy), vec<T,N>(rx,ry), cornmask_) {}

    box<T,N>  b() const { return std::get<0>(*this); }
    box<T,N>& b()       { return std::get<0>(*this); }
    vec<T,N>  r() const { return std::get<1>(*this); }
    vec<T,N>& r()       { return std::get<1>(*this); }
    CORNMASK_t  cornmask() const { return m_cornmask; }
    CORNMASK_t& cornmask()       { return m_cornmask; }
    CORNSTYLE_t  cornstyle() const { return m_cornstyle; }
    CORNSTYLE_t& cornstyle()       { return m_cornstyle; }

    /*! Get Center. */
    vec<T,N> cen() const { return b().cen(); }
    /*! Get Center \p i. */
    T cen(size_t i) const { return cen()(i); }
    /*! Center at \p pos. */
    void center_at(vec<T,N> pos) { b().center_at(); }

    /// Get \em All Dimensions (Size along all Dimensions).
    vec<T,N> dim() { return b().dim() + 2*r(); }
    /// Get \em Dimension (Size) along dimension \p i.
    T        dim(size_t i) const { return dim(i); }
    /// Get \em Area.
    T area()   const { return area  (*this); } // TODO: Move to class rbox2
    T volume() const { return volume(*this); } // TODO: Move to class rbox3

    /*! Set Rounded Square centered at \p c with Side-Radius s and Round Radius r.
     * More precisely set a to [c(0) - s, c(1) - s, c(0) + s, c(1) + s, r, r]
     */
    void setSqr(vec<T,N> c_, const T s_, const T r_) {
        b().l(0) = c_(0) - s_;
        b().l(1) = c_(1) - s_;
        b().u(0) = c_(0) + s_;
        b().u(1) = c_(1) + s_;
        r()(0) = r_;
        r()(1) = r_;
        cornmask() = CORNMASK_FULL;
        cornstyle() = CORNSTYLE_ELL_MIN;
    }

    /*! Set \p rbox using the Outer Box \p boxO and the Inner Relative Rounding
     *  Radius \p rradI from 0.0 to 1.0.
     */
    void set_OBox_IRRad(const box<T,N>& boxO,
                        const vec<T,N>& rradI,
                        CORNMASK_t cornmask_, CORNSTYLE_t cornstyle_);

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Relative Affine Transform.
     * Upon changing \p extP to \p extN change the \p src to \p dst accordingly
     * so that the src relative size position relative to \p extP is preserved.
     */
    rbox relaffine(const box<T,N>& extP,
                   const box<T,N>& extN,
                   const rbox& src) {
        return rbox(relaffine(extP, extN, src.b()),
                    src.r() * extN.dim() / extP.dim());
    }

    /*! Extend Sides by \p s. */
    void extend(const vec<T,N>& s) {
        b().l(0) -= s(0);
        b().l(1) -= s(1);
        b().u(0) += s(0);
        b().u(1) += s(1);
    }
    /*! Compress the sides of \p a the shift \p s. */
    void compress(const vec<T,N>& s) { extend(-s); }

    /*! Read the Control Point Mask of \p const rbox& at \p mpos and bit-or it with
     *  contents of \p mask. */
    int rdCtrlPM_SFORM(const rbox& rbox,
                       const vec<T,N>& mpos,
                       uint8_t * mask,
                       uint32_t * rsz_i,
                       bool set_flag);

    /*! Horizontally Translate the sides of \p a the shift \p sx.*/
    void htransl(const rbox& a_, T sx_, rbox* b_) { box_htransl(a_.b(), sx_, b_->b()); }

    /*! Vertically Translate the sides of \p a the shift \p sy.*/
    void vtransl(const rbox& a_, T sy_, rbox* b_) { box_vtransl(a_.b(), sy_, b_->b()); }

    /*! Scale \p a the factor \p f. */
    friend pure rbox scale(const rbox& a, T f) {
        rbox b;
        vec<T,N> dim;			/* dimension */
        vec<T,N> s;			/* shift */
        rdDim(a, &dim);
        s(0) = - (dim(0) * (1 - f) / 2);
        s(1) = - (dim(1) * (1 - f) / 2);
        *b = *a;
        b.extend(&s);
        return b;
    }

    /*! Centered Scale \p a the factor \p f. */
    friend pure rbox scalev(const rbox& a,
                            const vec<T,N>& f) {
        rbox b;
        vec<T,N> dim;			/* dimension */
        vec<T,N> s;			/* shift */
        rdDim(a, &dim);
        s(0) = - (dim(0) * (1 - f(0)) / 2);
        s(1) = - (dim(1) * (1 - f(1)) / 2);
        *b = *a;
        b.extend(&s);
        return b;
    }

    /*! Sub-Rbox when \p ba is viewed relative to \p a and put result in \p b.
     * The inverse operation of \c \c getRel().
     */
    void getSub(const box<T,N>& a, const rbox& ba, rbox* b_) {
        vec<T,N> ad; box_rdDim(a, &ad);
        vec_set(b_->b().l, a.l(0) + ad(0) * ba.b().l(0), a.l(1) + ad(1) * ba.b().l(1));
        vec_set(b_->b().u, a.l(0) + ad(0) * ba.b().u(0), a.l(1) + ad(1) * ba.b().u(1));
        vec_pwprod(&ba.r(), &ad, &b_->r());
        b_->cornmask() = ba.cornmask();
        b_->cornstyle = ba.cornstyle;
    }

    /*! The inverse operation of \c \c getSub(). */
    void getRel(const box<T,N>& a, const rbox& b_, rbox* ba) {
        vec<T,N> ad; box_rdDim(a, &ad);
        vec_set(&ba->b().l, (b_.b().l(0) - a.l(0)) / ad(0), (b_.b().l(1) - a.l(1)) / ad(1));
        vec_set(&ba->b().u, (b_.b().u(0) - a.l(0)) / ad(0), (b_.b().u(1) - a.l(1)) / ad(1));
        vec_pwquot(&b_.r(), &ad, &ba->r());
        ba->cornmask() = b_.cornmask();
        ba->cornstyle = b_.cornstyle;
    }

    bool includes(const vec<T,N>& b_, bool lin_flag) const {
        const auto& a = *this;
        box<T,N> exta;			/* vertically extend part of \p a */

        /* check horizontally extend inner box */
        box_extendH(&b(), r()(0), exta);
        if (exta.includes(b_)) { return true; }

        /* check vertically extend inner box */
        box_extendV(&b(), r()(1), exta);
        if (exta.includes(b_)) { return true; }

        const T irx = 1.0f / r()(0), iry = 1.0f / r()(1); /* inverses */
        const vec<T,N> e0 = { b().l(0), b().l(1) };
        const vec<T,N> e1 = { b().u(0), b().l(1) };
        const vec<T,N> e2 = { b().u(0), b().u(1) };
        const vec<T,N> e3 = { b().l(0), b().u(1) };

        if (lin_flag) {
            if (std::abs((b(0)-e0(0))*irx) + std::abs((b(1)-e0(1))*iry) < 1) { return true; }
            if (std::abs((b(0)-e1(0))*irx) + std::abs((b(1)-e1(1))*iry) < 1) { return true; }
            if (std::abs((b(0)-e2(0))*irx) + std::abs((b(1)-e2(1))*iry) < 1) { return true; }
            if (std::abs((b(0)-e3(0))*irx) + std::abs((b(1)-e3(1))*iry) < 1) { return true; }
        } else {
            if (std::sqr((b(0)-e0(0))*irx) + std::sqr((b(1)-e0(1))*iry) < 1) { return true; }
            if (std::sqr((b(0)-e1(0))*irx) + std::sqr((b(1)-e1(1))*iry) < 1) { return true; }
            if (std::sqr((b(0)-e2(0))*irx) + std::sqr((b(1)-e2(1))*iry) < 1) { return true; }
            if (std::sqr((b(0)-e3(0))*irx) + std::sqr((b(1)-e3(1))*iry) < 1) { return true; }
        }

        return false;
    }

    /*! Linearly Interpolate from \p src to \p dst at progress \p prg with transition \p trans. */
    friend pure rbox linear_interpolate(const rbox& src,
                                        T prg,
                                        const rbox& dst) {
        return rbox(linear_interpolate(src.b().l, prg, dst.b().l),
                    linear_interpolate(src.r(), prg, dst.r()),
                    src.cornmask(),
                    src.m_cornstyle);
    }

    friend std::ostream& show        (std::ostream& os, const rbox& a) { os << "(b" << MEMCH << a.b() << " ⇘ r" << MEMCH << a.r() << ")"; return os; }
    friend std::ostream& operator << (std::ostream& os, const rbox& a) { return show(os, a); }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & b() & r() & cornmask() & cornstyle(); }

    friend rbox& rand(rbox& a) { rand(a.b()); rand(a.r()); return a; }
private:
    CORNMASK_t  m_cornmask;		/**< Corner Rounding Mask. */
    CORNSTYLE_t m_cornstyle;    /**< Corner Style. */
};

template<class T> using rbox2 = rbox<T,2>;
template<class T> using rbox3 = rbox<T,3>;

template<class T, std::size_t N> inline pure T area(const rbox2<T>& a) {
    auto bd = a.b().dim();         // inner box dimensions
    return (a.b().area() +           // inner box
            M_PI * a.r(0) * a.r(1) + // rounding ellipse
            // outer boxes
            2 * (bd(0) * a.r()(1) +
                 bd(1) * a.r()(0)));
}
template<class T, std::size_t N> inline pure T volume(const rbox3<T>& a) {
    auto bd = a.b().dim();                      // inner box dimensions
    return (a.b().volume() +                    // inner box
            M_4PI_3 * a.r(0) * a.r(1) * a.r(2) + // rounding ellipsoid. \see https://sv.wikipedia.org/wiki/Ellipsoid
            // outer boxes
            2 * (bd(0) * a.r()(1) * a.r()(2) +
                 bd(1) * a.r()(2) * a.r()(0) +
                 bd(2) * a.r()(0) * a.r()(1)
                 ));
}

template<class T, std::size_t N>
inline void
rbox_set_OBox_IRRad(rbox<T,N>& rbox,
                    const box<T,N>& boxO,
                    const vec<T,N>& rradI,
                    CORNMASK_t cornmask,
                    CORNSTYLE_t cornstyle)
{
    auto dimO = boxO.dim(); /* outer bounding box dimensions */
    vec<T,N> rad = { 0.0f, 0.0f };	/* rbox rounding radius */
    switch (cornstyle) {
    case CORNSTYLE_ELL:
    case CORNSTYLE_ELL_MIN:
    case CORNSTYLE_ELL_MAX:
        vec_set(&rad,
                0.5 * rradI(0) * dimO(0),
                0.5 * rradI(1) * dimO(1));
        if (cornstyle == CORNSTYLE_ELL_MIN) {
            rad = std::min(rad(0), rad(1));
        } else if (cornstyle == CORNSTYLE_ELL_MAX) {
            rad = std::max(rad(0), rad(1));
        }
        break;
    default:
        //PWARN("Cannot handle cornstyle %d\n", cornstyle);
        break;
    }
    rbox_set_IBox_Rad(rbox, compress(boxO, rad), rad, cornmask, cornstyle);
}

template<class T, std::size_t N>
inline int
rbox_rdCtrlPM_SFORM(const rbox<T,N>& rbox,
                    const vec<T,N>& mpos,
                    uint8_t * mask,
                    uint32_t * rsz_i, bool set_flag)
{
    int ret = 0;
    if (box_includes(rbox.b, mpos)) {
        /* NOTE: rsz_i: [0..3] */
        ret = box_rdCtrlPM_SFORM(rbox.b, mpos, mask, rsz_i, set_flag);
    } else {			/* rounding frame */
        /* NOTE: rsz_i: [4..5] */
        int xi, yi;				     /* x and y index */
        if      (mpos(0) < rbox.b.l(0)) { xi = -1; }
        else if (mpos(0) > rbox.b.u(0)) { xi = +1; }
        else                            { xi =  0; }
        if      (mpos(1) < rbox.b.l(1)) { yi = -1; }
        else if (mpos(1) > rbox.b.u(1)) { yi = +1; }
        else                            { yi =  0; }
        if      (xi == -1 or xi == +1) { *rsz_i = 0; *mask = pnw::put_bit(*mask, 4, set_flag); ret = 1; }
        if      (yi == -1 or yi == +1) { *rsz_i = 0; *mask = pnw::put_bit(*mask, 5, set_flag); ret = 1; }
    }
    return ret;
}

template<class T, std::size_t N>
inline rbox<T,N>
shiftCtrlP(const rbox<T,N>& a,
           const vec<T,N>& mpos,
           const vec<T,N>& mshift,
           const vec<T,N>& mindim,
           const uint8_t * mask)
{
    if (includes(a.b, mpos)) {
        a.b = shiftCtrlP(a.b, mpos, mshift, mindim, mask);
    } else {			/* rounding frame */
        auto ndim = a.r - mshift;
        int xOK = (ndim(0) > 0 or ndim(0) > mindim(0)); /* OK to shift along x */
        int yOK = (ndim(1) > 0 or ndim(1) > mindim(1)); /* OK to shift along y */
        int b4 = pnw::get_bit(*mask, 4);
        int b5 = pnw::get_bit(*mask, 5);
        auto cen = a.b().cen();
        if (b4) {
            if (mpos(0) < cen(0)) { if (xOK) { a.r(0) -= mshift(0); } }
            if (mpos(0) > cen(0)) { if (xOK) { a.r(0) += mshift(0); } }
        }
        if (b5) {
            if (mpos(1) < cen(1)) { if (yOK) { a.r(1) -= mshift(1); } }
            if (mpos(1) > cen(1)) { if (yOK) { a.r(1) += mshift(1); } }
        }
    }
    return a;
}

template<class T, std::size_t N>
inline void rbox_test(void)
{
    box<T,N> boxO = { { 0, 0 }, { 2, 1 } };
    vec<T,N> rradI = { 0.1, 0.1 };
    rbox<T,N> rbox;
    rbox_set_OBox_IRRad(rbox, boxO, rradI,
                        CORNMASK_FULL, CORNSTYLE_ELL_MIN);
}
