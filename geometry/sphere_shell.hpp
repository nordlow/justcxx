/*! \file sphere_shell.hpp
 * \brief N-Dimensional Spherical Shell, Circular Ring in 2-D.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <tuple>
//#include <boost/math/constants/constants.hpp>
#include <boost/serialization/access.hpp>
#include "vec.hpp"
#include "box.hpp"
#include "../mean.hpp"
#include "../bitwise.hpp"

template<class T, std::size_t N>
class sphere_shell : public std::tuple<vec<T,N>, T, T> {  // Center and Inner and Outer Radius.
public:
    sphere_shell() : sphere_shell(0, 0.5, 1) {}  ///< Construct Default Unit Ring of Thickness 0.5.
    sphere_shell(const vec<T,N>& c_, T ri_, T ro_)
        : std::tuple<vec<T,N>, T, T>(c_,ri_,ro_) {} // \old cring_setCrr
    sphere_shell(const vec<T,N>& c_, T r_)
        : sphere_shell(c_, r_, r_) {} // C++11

    vec<T,N>  c() const { return std::get<0>(*this); } ///< Center.
    vec<T,N>& c()       { return std::get<0>(*this); } ///< Center.
    T  ri() const { return std::get<1>(*this); }       ///< Inner Radius.
    T& ri()       { return std::get<1>(*this); }       ///< Inner Radius.
    T  ro() const { return std::get<2>(*this); }       ///< Outer Radius.
    T& ro()       { return std::get<2>(*this); }       ///< Outer Radious.

    box<T,N> outer_box() const { return box<T,N>(c()-ro(), c()+ro()); } ///< Outer Box.
    box<T,N> inner_box() const { return box<T,N>(c()-ri(), c()+ri()); } ///< Inner Box.

    /*! Fit Inside \p a. */
    void fit_in(const box<T,N>& a) {
        c() = a.cen();          // copy center
        auto md = a.min_dim();
        ri() = md/4;
        ro() = md/2;
    }

    template<class S> friend pure S concept_name(const sphere_shell& m) { S s("0-Dimensional Spherical Shell"); s[0] += N; return s; }
    template<class S> friend pure S typenameof(const sphere_shell& a) { S s("sphere_shell<T,N>"); return s; }

    friend std::ostream& show        (std::ostream& os, const sphere_shell& a) { os << "(c" << MEMCH << a.c() << ",ri" << MEMCH << a.ri() << ",ro" << MEMCH << a.ro() << ")"; return os; }
    friend std::ostream& operator << (std::ostream& os, const sphere_shell& a) { return show(os, a); }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & c() & ri() & ro(); }
};

template<class T> using cring         = sphere_shell<T,2>; ///< Circular 2-Dimensional Ring.
template<class T> using sphere3_shell = sphere_shell<T,3>; ///< Spherical 3-Dimensional Shell.

/*! Area of \p a. */
template<class T> inline pure T area(const cring<T>& a) { return M_PI * (std::sqr(a.ro()) -
                                                                          std::sqr(a.ri())); }

/*! Check if \p a lies beside \p b. */
template<class T, std::size_t N> inline pure bool beside(const cring<T>& a,
                                                          const vec<T,N>& b){
    return ((dist(a.c(), b) <  a.ri()) or
            (dist(a.c(), b) >= a.ro()));
}
/*! Check if \p a \em includes \p b. */
template<class T, std::size_t N> inline pure bool includes(const cring<T>& a,
                                                            const vec<T,N>& b) {
    return not beside(a, b);
}
/*! Check if \p a lies beside \p b. */
template<class T, std::size_t N> inline pure bool beside(const cring<T>& a,
                                                          const box<T,N>& b) {
    const vec<T,N> vtx[4] = { { b.l(0), b.l(1) },
                              { b.u(0), b.l(1) },
                              { b.u(0), b.u(1) },
                              { b.l(0), b.u(1) } };
    /* circle beside all box-corners => lies beside the whole box */
    return (not includes(a, vtx[0]) and
            not includes(a, vtx[1]) and
            not includes(a, vtx[2]) and
            not includes(a, vtx[3]));
}

/*! Check if \p a \em overlaps \p b. */
template<class T, std::size_t N> inline bool overlap(const cring<T>& a,
                                                     const box<T,N>& b) {
    return not beside(a, b);
}

/*! Return non-zero if \p a completely includes \p b. */
template<class T, std::size_t N> inline bool includes(const cring<T>& a,
                                                      const box<T,N>& b) {
    const vec<T,N> vtx[4] = { { b.l(0), b.l(1) },
                              { b.u(0), b.l(1) },
                              { b.u(0), b.u(1) },
                              { b.l(0), b.u(1) } };
    return (includes(a, vtx[0]) and
            includes(a, vtx[1]) and
            includes(a, vtx[2]) and
            includes(a, vtx[3]));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Resize \p a using the user (mouse) drag from \p mpos the shift \p mshift.
 * \param[in] mask Control Point Mask to use.
 */
template<class T, std::size_t N>
inline cring<T> shiftCtrlP(const cring<T>& a,
                           const vec<T,N>& mpos,
                           const vec<T,N>& mshift,
                           const vec<T,N>& mindim,
                           uint8_t mask)
{
    if (pnw::get_bit(mask, 0)) { a.c() += mshift; }
    if (pnw::get_bit(mask, 1)) { a.ri() -= dot(normalize(a.c() - mpos), mshift); }
    if (pnw::get_bit(mask, 2)) { a.ro() -= dot(normalize(a.c() - mpos), mshift); }
    return a;
}

/*! Set Outer Radius of \p a to \p radO and scale inner radius linearly with change of outer radious. */
template<class T, std::size_t N> inline void setRadO_autoscaledRadI(cring<T>& a, T radO) {
    const T old_or = a.ro();
    a.ro() = radO;
    a.ri() = a.ri() * a.ro() / old_or; /* scale inner radius relatively */
}

#if 0
/*! Sub-Sphere-Shell when \p ba is viewed relative to \p a.
 * The inverse operation of \c getRel().
 */
template<class T, std::size_t N> inline pure cring<T> getSub(const cring<T>& ba,
                                                             const box<T,N>& a) {
    auto ad = a.dim();
    if (ad(0) != ad(1)) { PTODO("ad(0) != ad(1) => may result in an ellipse\n"); } // TODO: ad(0) != ad(1) => may result in an ellipse
    const T m = mean(ad(0), ad(1));
    return cring<T>(a(1) + ad * ba.c(),
                    ba.ri() * m,
                    ba.ro() * m);
}
/*! The inverse operation of \c getSub(). */
template<class T, std::size_t N> inline pure cring<T> getRel(const box<T,N>& a,
                                                             const cring<T>& b) {
    auto ad = a.dim();
    if (ad(0) != ad(1)) { PTODO("ad(0) != ad(1) => may result in an ellipse\n"); }
    const T m = mean(ad(0), ad(1));
    return cring<T>((b.c()(0) - a.l(0)) / ad(0),
                    (b.c()(1) - a.l(1)) / ad(1),
                    b.ri() / m,
                    b.ro() / m);
}
#endif
