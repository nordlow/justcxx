/*! \file sphere.hpp
 * \brief N-Dimensional Sphere (N-Sphere) (or Circle for 2D-case).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see https://en.wikipedia.org/wiki/N-sphere
 */

#pragma once
#include <utility>
//#include <boost/math/constants/constants.hpp>
#include "../cc_features.h"
#include <boost/serialization/access.hpp>
#include "../algorithm_x.hpp"
#include "../math_x.hpp"
#include "vec.hpp"
#include "box.hpp"

/*! N-Dimensional Sphere (or Circle for 2D-case). */
template<class T, std::size_t N>
class sphere : public std::pair<vec<T,N>, T>
{
public:
    sphere() : sphere(0, 1) {}  ///< Construct Default Unit Sphere.
    sphere(const vec<T,N>& c_, T r_) : std::pair<vec<T,N>, T>(c_, r_) {}

    vec<T,N>  c() const { return std::get<0>(*this); } ///< Center.
    vec<T,N>& c()       { return std::get<0>(*this); } ///< Center.
    T  r() const { return std::get<1>(*this); }        ///< Radius.
    T& r()       { return std::get<1>(*this); }        ///< Radius.

    box<T,N> outer_box() const { return box<T,N>(c(), r()); }
    box<T,N> inner_box() const { return box<T,N>(c(), r() * M_SQRT2_2); }

    /*! Return true if \c this includes \p a. */
    bool pure includes(const vec<T,N>& a) const { return dist(a, c()) <= r(); }
    /*! Return true if \c this lies beside \p a. */
    bool pure beside(const vec<T,N>& a) const { return not includes(a); }
    /*! Return true if \p a and \p b overlap. */
    friend pure bool overlap(const sphere& a,
                              const box<T,N>& b) { return not separate(a, b); }
    /*! Return true if \p a and \p b are separate. */
    friend pure bool separate(const sphere<T,N>& a,
                               const box<T,N>& b) {
        const auto n = b.corner_count();
        vec<T,N> c[n]; b.read_corners(c);
        return std::all_of(c,c+n, [a](vec<T,N>& p) { return a.beside(p); });
    }
    /*! Return true if \p a includes \p b. */
    friend pure bool includes(const sphere<T,N>& a,
                               const box<T,N>& b) {
        const auto n = b.corner_count();
        vec<T,N> c[n]; b.read_corners(c);
        return std::all_of(c,c+n, [a](vec<T,N>& p) { return a.includes(p); });
    }

    /*! Return \em Area of \c this. */
    T area() const { return area(*this); }
    /*! Return \em Volume of \c this. */
    T volume() const { return volume(*this); }

    /*! Extend Radius of \p a the shift \p s. */
    friend pure sphere extend(const sphere& a, T s) { a.r() += s; return a; }
    /*! Extend Radius of \p this the shift \p s. */
    sphere& extend(T s) { r() += s; return *this; }

    /*! Centered Scale Radius of \p a the factor \p f. */
    friend pure sphere scale(const sphere& a, T f) { return sphere(a.c(), f * a.r()); }
    /*! Centered Scale Radius the factor \p f. */
    sphere& scale(T f) { r() *= f; return *this; }

    template<class S> friend pure S concept_name(const sphere& m) { S s("0-Dimensional Circle"); s[0] += N; return s; }
    template<class S> friend pure S typenameof(const sphere& a) { S s("sphere<T,N>"); return s; }

    friend std::ostream& show        (std::ostream& os, const sphere& a) { os << "(c" << MEMCH << a.c() << ",r" << MEMCH << a.r() << ")"; return os; }
    friend std::ostream& operator << (std::ostream& os, const sphere& a) { return show(os, a); }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & c() & r(); }

    friend sphere& rand(sphere& a) { rand(a.c()); rand(a.r()); return a; }
};

#if HAVE_CXX11_TEMPLATE_ALIASES
template<class T> using circle = sphere<T,2>;
template<class T> using sphere3 = sphere<T,3>;
#endif

// Area
template<class T>                inline pure T area(const circle<T>& a) { return M_PI * std::sqr(a.r()); }
template<class T>                inline pure T area(const sphere3<T>& a) { return M_4PI * std::sqr(a.r()); }
template<class T, std::size_t N> inline pure T area(const sphere<T,N>& a) { return 0; } // \see https://en.wikipedia.org/wiki/N-sphere
// Volume
template<class T>                inline pure T volume(const sphere3<T>& a) { return M_4PI_3 * std::cube(a.r()); }
template<class T, std::size_t N> inline pure T volume(const sphere<T,N>& a) { return 0; } // \see https://en.wikipedia.org/wiki/N-sphere

/// Get Distance from \p a to the Edge of \p b.
template<class T, std::size_t N> inline pure T edge_distance(const vec<T,N>& a,
                                                             const sphere<T,N>& b)
{
    // handy aliases
    auto& c = b.cen;
    auto& r = b.rad;
    T f = dist(a, c);	// distance to circ center
    auto d = (a - c) / f;
    T h = norm(r * d);
    return std::abs(f - h); // distance to edge
}

/*! Inner Sphere of \p a scaled the factor \p f. */
template<class T, std::size_t N> inline pure sphere<T,N> inner_sphere(const box<T,N>& a, T f = 1) { return sphere<T,N>(a.cen(), f * a.min_dim() / 2); }
/*! Inner Circle of \p a scaled the factor \p f. */
template<class T>                inline pure circle<T>   inner_circle(const box2<T>&  a, T f = 1) { return inner_sphere<T,2>(a, f); }

/*! Outer Sphere of \p a scaled the factor \p f. */
template<class T, std::size_t N> inline pure sphere<T,N> outer_sphere(const box<T,N>& a, T f = 1) { return sphere<T,N>(a.cen(), f * norm(a.dim()) / 2); }
/*! Outer Circle of \p a scaled the factor \p f. */
template<class T, std::size_t N> inline pure circle<T>   outer_circle(const box2<T>&  a, T f = 1) { return outer_sphere<T,2>(a, f); }

typedef circle<float> cirf;
typedef circle<double> cird;
typedef sphere3<float> spheref;
typedef sphere3<double> sphered;

/*! Resize \p a using the user (mouse) drag from \p mpos the shift \p mshift.
 * \param[in] mask Control Point Mask to use.
 * \return 1 if extents were changed, -1 if type not supported, 0 otherwise.
 */
template<class T, std::size_t N>
inline sphere<T,N> shiftCtrlP(const sphere<T,N>& a,
                              const vec<T,N>& mpos,
                              const vec<T,N>& mshift,
                              const vec<T,N>& mindim,
                              const uint8_t * mask)
{
    if (mask) {
        if (pnw::get_bit(*mask, 0)) { a.c() += mshift; }
        if (pnw::get_bit(*mask, 1)) { a.r() -= dot(normalize(a.c() - mpos), mshift); }
    }
    return a;
}

#if 0

/* ---------------------------- Group Separator ---------------------------- */

/*! Sub-Sphere when \p ba is viewed relative to \p a and put
 * result in \p b.
 * The inverse operation of \c sphere<T,N>_getRel().
 */
inline void
sphere<T,N>_getSub(const box<T,N>& a,
                   const sphere<T,N>& ba,
                   sphere<T,N>& b)
{
    vec3f ad; box3f_rdDim(a, &ad);
    vec3f_set(&b.c(),
              a.l(0) + ad(0) * ba.c()(0),
              a.l(1) + ad(1) * ba.c()(1),
              a.l(2) + ad(2) * ba.c()(2));
    b.r() = ba.r() * pnw::mean(ad(0), ad(1), ad(2));
    if (ad(0) != ad(1) or
        ad(1) != ad(2) or
        ad(2) != ad(0)) {
        PTODO("ad's components not all equal => may result in an ellipse\n");
    }
}

/*! Inverse Operation of \c sphere<T,N>_getSub(). */
inline sphere<T,N> getRel(const box<T,N>& a,
                          const sphere<T,N>& b)
{
    auto ad = a.dim();
    sphere<T,N> ba((b.c() - a.l()) / ad,
                   b.r() / pnw::mean(ad(0), ad(1), ad(2)));
    if (ad(0) != ad(1) or
        ad(1) != ad(2) or
        ad(2) != ad(0)) {
        PTODO("ad's components not all equal => may result in an ellipse\n");
    }
    return ba;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Sub-Sphere when \p ba is viewed relative to \p a.
 * Inverse Operation of \c sphere<T,N>_getRel_box2f().
 */
inline void
sphere<T,N>_getSub_box2f(const box2f& a,
                         const sphere<T,N>& ba,
                         sphere<T,N>& b)
{
    vec2f ad; box2f_rdDim(a, &ad);
    vec3f_set(&b.c(),
              a.l(0) + ad(0) * ba.c()(0),
              a.l(1) + ad(1) * ba.c()(1),
              ba.c()(2));		/* z is ignored */
    b.r() = ba.r() * mean(ad(0), ad(1));
    if (ad(0) != ad(1)) {
        PTODO("ad's components not all equal => may result in an ellipse\n");
    }
}

/*! Inverse Operation of \c sphere<T,N>_getSub_box2f(). */
inline void
sphere<T,N>_getRel_box2f(const box2f& a,
                         const sphere<T,N>& b,
                         sphere<T,N>& ba)
{
    vec2f ad; box2f_rdDim(a, &ad);
    vec3f_set(&ba.c(),
              (b.c()(0) - a.l(0)) / ad(0),
              (b.c()(1) - a.l(1)) / ad(1),
              b.c()(2));		/* z is ignored */
    ba.r() = b.r() / mean(ad(0), ad(1));
    if (ad(0) != ad(1)) {
        PTODO("ad's components not all equal => may result in an ellipse\n");
    }
}

/*! Horizontally Translate \p a the shift sx and put the result in \p b. */
inline void cirf_htransl(const cirf& a, T sx, cirf& b)
{
    b.c(0) = a.c(0) + sx;
}

/*! Vertically Translate \p a the shift sy and put the result in \p b. */
inline void cirf_vtransl(const cirf& a, T sy, cirf& b)
{
    b.c(1) = a.c(1) + sy;
}

/*! Sub-Circle when \p ba is viewed relative to \p a.
 * The inverse operation of \c cirf_getRel().
 */
inline void
getSub(const box& a, const cirf& ba, cirf& b)
{
    vec2f ad; box_rdDim(a, &ad);
    vec2f_set(&b.c, a.l(0) + ad(0) * ba.c(0), a.l(1) + ad(1) * ba.c(1));
    b.r() = ba.r() * mean(ad(0), ad(1));
    if (ad(0) != ad(1)) {
        PTODO("ad(0) != ad(1) => may result in an ellipse\n");
    }
}

/*! Inverse Operation of \c cirf_getSub(). */
inline void
getRel(const box& a, const cirf& b, cirf& ba)
{
    vec2f ad; box_rdDim(a, &ad);
    vec2f_set(&ba.c, (b.c(0) - a.l(0)) / ad(0), (b.c(1) - a.l(1)) / ad(1));
    ba.r() = b.r() / mean(ad(0), ad(1));
    if (ad(0) != ad(1)) {
        PTODO("ad(0) != ad(1) => may result in an ellipse\n");
    }
}

#endif
