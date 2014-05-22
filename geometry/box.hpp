/*! \file box.hpp
 * \brief N-Dimensional Axis-Aligned (Hyper) Box.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <utility>
#include <limits>
#include <vector>
#include <boost/serialization/access.hpp>
#include "vec.hpp"
#include "../bitwise.hpp"
#include "../numeric_x.hpp"
#include "../SFILL_enum.h"
#include "../clamp.hpp"
#include "../cc_features.h"

/*! Box Edge Type Code. */
typedef enum {
    BOXEDGE_TOP,			/**< Box Edge at Vertical Top. */
    BOXEDGE_BOT,			/**< Box Edge at Vertical Bottom. */
    BOXEDGE_LEFT,			/**< Box Edge at Horizontal Left. */
    BOXEDGE_RIGHT,		/**< Box Edge at Horizontal Right. */
} __attribute__ ((packed)) BOXEDGE_t;

/*! N-Dimensional Axis-Aligned (Hyper) Box.
 * \note We must use inclusive compares betweeen boxes and points in inclusion
 * functions such as inside() and includes() in order for the behaviour of
 * bounding boxes (especially in integer space) to work as desired.
 */
template<class T, std::size_t N>
class box : public std::pair<vec<T,N>,
                             vec<T,N> > {
public:
    typedef vec<T,N> value_type;

    /// \name Construct.
    /// \{
#if NDEBUG
    box() {}                    ///< Default Construct Uninitialized. */
#else
    box() : box(vec<T,N>(pnw::uninitialized<T>())) {} ///< Default Construct Undefined. */
#endif
    explicit box(T a) : box(vec<T,N>(a)) {}
    explicit box(const vec<T,N>& a) : box(a, a) {} ///< Construct Empty Box (Point).
    box(const vec<T,N>& a,
        const vec<T,N>& b) : std::pair<vec<T,N>,vec<T,N> >(a, b) {}
    box(const vec<T,N>& c, T r) : box(c-r, c+r) {} ///< Square with Center \p c and Radius \p rad.
    /// \}

    /// Assign lower \p l_ and upper \p u_ bound.
    void assign(const vec<T,N>& l_,
                const vec<T,N>& u_) { l() = l_; u() = u_;  }

    /// Dimensionality.
    size_t dimnum() const { return N; }
    size_t size() const { return dimnum(); }

    /*! Return true if \c this is defined. */
    friend pure bool defined(const box& a) { return a.l() <= a.u(); } // TODO: Choose either nan or unit().
    /*! Return true if \c this is defined. */
    wuur bool defined() { return l() <= u(); } // TODO: Choose either nan or unit().

    /// Get Unite Operation Unit Value.
    static constexpr box unite_unit() { return box(std::numeric_limits<T>::max(),
                                                   std::numeric_limits<T>::min()); }
    /// Undefine \c this.
    box& undefine() { return *this = unite_unit(); } // TODO: Choose either \c nan or \c unite_unit().

    /*! Check if Point (Zero Dimensions). */
    friend pure bool is_point(const box<T,N>& a) { return a.l() == a.u(); }
    friend pure bool is_defined(const box<T,N>& a) { return all(a.l() <= a.u()); }

    /// \name Element Access.
    /// Replaces box_at(const boxs * a, int i) { return (a.l)[i]; }
    /// \{
    vec<T,N>  operator() (size_t i) const { return (&std::get<0>(*this))[i]; }
    vec<T,N>& operator() (size_t i)       { return (&std::get<0>(*this))[i]; }
    vec<T,N>  operator[] (size_t i) const { return (&std::get<0>(*this))[i]; }
    vec<T,N>& operator[] (size_t i)       { return (&std::get<0>(*this))[i]; }
    vec<T,N>  l() const { return std::get<0>(*this); } ///< Lower Bound.
    vec<T,N>& l()       { return std::get<0>(*this); } ///< Lower Bound.
    vec<T,N>  u() const { return std::get<1>(*this); } ///< Upper Bound.
    vec<T,N>& u()       { return std::get<1>(*this); } ///< Upper Bound.
    T  l(size_t i) const { return l()[i]; } ///< Lower Bound.
    T& l(size_t i)       { return l()[i]; } ///< Lower Bound.
    T  u(size_t i) const { return u()[i]; } ///< Upper Bound.
    T& u(size_t i)       { return u()[i]; } ///< Upper Bound.
    /// \}

    /// \name Corner Access. \todo corner_iterator
    /// \{
    /*! Get Corner Count (2^N). */
    constexpr size_t corner_count() const { return 1<<N; }
    /*! Get \p i:th Box (Hyper-Cube) Corner. */
    vec<T,N> corner(size_t i) const {
        vec<T,N> c;
        for (size_t d = 0; d < N; ++d) { // for each dimension
            c[d] = (*this)(pnw::get_bit(i, d))[d];
        }
        return c;
    }
    /*! Read All 2^N Corners of \c this into \p c. */
    void read_corners(vec<T,N>* c) const {
        for (size_t i = 0; i < corner_count(); ++i) { // for each corner 0 -> 2^N-1
            c[i] = corner(i);
        }
    }
    friend pure std::vector<vec<T,N> > corners(const box& a) {
        std::vector<vec<T,N> > c(a.corner_count());
        a.read_corners(c.data());
        return c;
    }
    /// \}

    /// Get \em Dimensions (Size along all Dimensions).
    vec<T,N> dim()       const { return u() - l(); }
    /// Get \em Dimension (Size) along dimension \p i.
    T        dim(size_t i) const { return u(i) - l(i); }
    /// Get \em All Radii (Half-Sides).
    vec<T,N> rad()       const { return dim()/2; }
    /// Get \em Radius (Half-Side) along dimension \p i.
    T        rad(size_t i) const { return dim(i)/2; }

    /// Get \em Area.
    T area()   const { return area  (*this); } // TODO: Move to class box2
    /// Get \em Volume.
    T volume() const { return volume(*this); } // TODO: Move to class box3

    /// Get Aspect Ratio. Both elementwise are within [0,1].
    vec<T,N> arat() const { auto d = dim(); return d / max_element(d); }

    /// \name Other
    /// \{
    /*! Check if \c this lies inclusively \em inside \p a. */
    bool inside(const box& a) const {
        return (std::all_of(a.l() <= l()) and
                std::all_of(u() <= a.u()));
    }
    /*! Check if \c this lies exclusively \em beside \p a. */
    bool beside(const box& a) const {
        return (std::any_of(u() < a.l()) or  // left of
                std::any_of(l() > a.u()));   // below
    }
    /*! Check if \c this lies inclusively \em beside \p a. */
    bool beside(const vec<T,N>& a) const { return not includes(a); }
    /*! Check if \c this \em overlaps \p a. */
    bool overlap(const box& a) const { return not beside(a); }
    /*! Check if \c this \em covers \p a. */
    bool includes(const vec<T,N>& a) const {
        return (std::all_of(l() <= a()) and
                std::all_of(a() <= u()));
    }
    /*! Check if \c this \em covers \p a. */
    bool includes(const box<T,N>& a) const { return a.inside(*this); }
    /*! Check if \c this \em covers \p a and \p b. */
    bool includes(const vec<T,N>& a, const vec<T,N>& b) const { return includes(a) and includes(b); }

    /*! Get Center. */
    vec<T,N> cen() const { return (l() + u()) / 2; }
    /*! Get Center \p i. */
    T cen(size_t i) const { return cen()[i]; }

    /*! Center at \p pos. */
    void center_at(const vec<T,N>& pos) { auto h = dim()/2; l() = pos - h; u() = pos + h; }
    /*! Set the \em Radii of \p a to \p rad. */
    box<T,N>& set_rad(const vec<T,N>& rad_) { return *this = box<T,N>(cen(), rad_); }
    /*! Set the \em Dimensions of \p a to \p dim. */
    box<T,N>& set_dim(const vec<T,N>& dim_) { return set_rad(dim_/2); }

    /*! Minimum Dimension (Side). */
    T min_dim() const { return min_element(dim()); }
    /*! Maximum Dimension (Side). */
    T max_dim() const { return min_element(dim()); }

    /*! Get Inner Square. */
    box inner_square() const { return box(cen(), min_dim() / 2); }
    /*! Get Outer Square. */
    box outer_square() const { return box(cen(), max_dim() / 2); }

    box inner_arat_box(const vec<T,N>& arat_) const { return box(cen(), arat_ * min_element(dim() / arat_) / 2); }
    box outer_arat_box(const vec<T,N>& arat_) const { return box(cen(), arat_ * max_element(dim() / arat_) / 2); }
    /// \}

    /// \name Get Sub-Box.
    /// \{
    /// Get Sub-Box.
    box subbox(T rp00,
               T rp01,
               T rp10,
               T rp11) const {
        return subbox(vec<T,N>(rp00, rp01),
                      vec<T,N>(rp10, rp11)); }
    /// Get Sub-Box.
    box subbox(const vec<T,N>& rp0,
               const vec<T,N>& rp1) const {
        auto d = dim();
        return box(l() + d * rp0,
                   l() + d * rp1); }
    /// Get Sub-Box defined by \p rb (in relative coordinates).
    box subbox(const box& rb) const {
        auto d = dim();
        return box(l() + d * rb.l(),
                   l() + d * rb.u());
    }
    /// \}

    box get_halve00() const { auto a = l(); auto b = vec<T,N>(pnw::mean(l(0), u(0)), u(1)); return box(a, b); }
    box get_halve01() const { auto a = vec<T,N>(pnw::mean(l(0), u(0)), l(1));; auto b = u(); return box(a, b); }
    box get_halve10() const { auto a = vec<T,N>(l(0), pnw::mean(l(1), u(1))); auto b = u(); return box(a, b); }
    box get_halve11() const { auto a = l(); auto b = vec<T,N>(u(0), pnw::mean(l(1), u(1))); return box(a, b); }

    /// \name Splits
    /// \{
    /// Horizontal Split in \p left and \p right.
    void hsplit(box& left,
                box& right,
                T r) const {
        T d = dim(0);
        T ps = l(0) + r * d;
        left  = box<T,N>(l(0), l(1), ps, u(1));
        right = box<T,N>(ps, l(1), u(0), u(1));
    }
    /// Vertical Split in \p upper and \p lower.
    void vsplit(box& upper,
                box& lower,
                T r) const {
        T d = dim(1);
        T ps = l(1) + r * d;
        upper = box<T,N>(l(0), l(1), u(0), ps);
        lower = box<T,N>(l(0), ps, u(0), u(1));
    }
    /// \}

    /// \name Pointwise Binary Comparison.
    /// \{
    friend pure bool operator == (const box& a, const box& b) { return a.l() == b.l() and a.u() == b.u(); }
    friend pure bool operator != (const box& a, const box& b) { return a.l() != b.l() or  a.u() != b.u(); }
    /// \}

    /// \name Pointwise Binary Arithmetic and Assignment.
    /// \{
    box& operator  = (T a) { l() = a; u() = a; return *this; }
    box& operator += (T a) { l() += a; u() += a; return *this; }
    box& operator -= (T a) { l() -= a; u() -= a; return *this; }
    box& operator *= (T a) { l() *= a; u() *= a; return *this; }
    box& operator /= (T a) { l() /= a; u() /= a; return *this; }
    box& operator %= (T a) { l() %= a; u() %= a; return *this; }
    box& operator  = (const vec<T,N>& a) { l() = a; u() = a; return *this; }
    box& operator += (const vec<T,N>& a) { l() += a; u() += a; return *this; }
    box& operator -= (const vec<T,N>& a) { l() -= a; u() -= a; return *this; }
    box& operator *= (const vec<T,N>& a) { l() *= a; u() *= a; return *this; }
    box& operator /= (const vec<T,N>& a) { l() /= a; u() /= a; return *this; }
    box& operator %= (const vec<T,N>& a) { l() %= a; u() %= a; return *this; }
    /// \}

    /// \name Pointwise Unary Box Arithmetic.
    /// \{
    friend pure box operator - (const box& a) { return box(-a.l(), -a.u()); }
    friend pure box abs(const box& a) { return box(std::abs(a.l()), std::abs(a.u())); }
    /// \}

    /// \name Pointwise Binary Box-Box Arithmetic.
    /// \{
    friend pure box operator + (const box& a, const box& b) { return box(a.l() + b.l(), a.u() + b.u()); }
    friend pure box operator - (const box& a, const box& b) { return box(a.l() - b.l(), a.u() - b.u()); }
    friend pure box operator * (const box& a, const box& b) { return box(a.l() * b.l(), a.u() * b.u()); }
    friend pure box operator / (const box& a, const box& b) { return box(a.l() / b.l(), a.u() / b.u()); }
    friend pure box operator % (const box& a, const box& b) { return box(a.l() % b.l(), a.u() % b.u()); }
    /// \}

    /// \name Pointwise Binary Box-Vector Arithmetic
    /// \{
    template<class U> friend pure box operator + (const box& a, const vec<U,N>& b) { return box(a.l() + b, a.u() + b); }
    template<class U> friend pure box operator - (const box& a, const vec<U,N>& b) { return box(a.l() - b, a.u() - b); }
    template<class U> friend pure box operator * (const box& a, const vec<U,N>& b) { return box(a.l() * b, a.u() * b); }
    template<class U> friend pure box operator / (const box& a, const vec<U,N>& b) { return box(a.l() / b, a.u() / b); }
    template<class U> friend pure box operator % (const box& a, const vec<U,N>& b) { return box(a.l() % b, a.u() % b); }
    /// \}

    /// \name Pointwise Binary Box-Element Arithmetic.
    /// \{
    template<class U> friend pure box operator + (const box& a, U b) { return box(a.l() + b, a.u() + b); }
    template<class U> friend pure box operator - (const box& a, U b) { return box(a.l() - b, a.u() - b); }
    template<class U> friend pure box operator * (const box& a, U b) { return box(a.l() * b, a.u() * b); }
    template<class U> friend pure box operator / (const box& a, U b) { return box(a.l() / b, a.u() / b); }
    template<class U> friend pure box operator % (const box& a, U b) { return box(a.l() % b, a.u() % b); }
    /// \}

    /// \name Extend Sides of \p a the shift \p s.
    /// \{
    friend pure box extend(const box& a, const vec<T,N>& s) { return box(a.l() - s, a.u() + s); }
    friend pure box extend(const box& a, T s ) { return box(a.l() - s, a.u() + s); }
    void extend(const vec<T,N>& s) { l() -= s; u() += s; }
    void extend(T s) { l() -= s; u() += s; }
    /// \}

    /*! Unite \p a and \p  b. */
    friend pure box<T,N> unite(const box<T,N>& a, const box<T,N>& b) { return box<T,N>(min(a.l(), b.l()),
                                                                                       max(a.u(), b.u())); }
    /*! Unite \p a and \p  b. */
    friend pure box<T,N> unite(const box<T,N>& a, const vec<T,N>& b) { return box<T,N>(min(a.l(), b),
                                                                                       max(a.u(), b)); }
    /*! Unite \p a and \p  b. */
    friend pure box<T,N> unite(const vec<T,N>& a, const box<T,N>& b) { return unite(b, a); } // commutative
    /// \name Include \p a with \c this.
    box& include(const box& a) { return *this = unite(*this, a); }

    /// \name Compress Sides the shift \p s.
    /// \{
    /*! Compress the sides of \p a the shift [sx, sy]. */
    friend pure box compressXY(const box& a, T sx, T sy) {
        box b;
        b.l(0) = a.l(0) + sx;
        b.l(1) = a.l(1) + sy;
        b.u(0) = a.u(0) - sx;
        b.u(1) = a.u(1) - sy;
        return b;
    }
    /*! Compress Sides \p s. */
    friend pure box compress(const box& a, const vec<T,N>& s) { return box(a-s, a+s); }
    /// \}

    /// \name Scaling.
    /// \{
    /// Scale Factor \p f. */
    friend pure box scale(const box& a, T f) { box(a.cen(), f * a.dim()); }
    /// Scale Factor \p f.
    void scale(T f) { extend(- dim() * (1 - f) / 2); }
    /// Scale Factor \p f.
    void scale(const vec<T,N>& f) { extend(- dim() * (1 - f) / 2); }
    /// Scale Factor \p f along dimension \p b.
    void scale_along(T f, size_t b) {
        vec<T,N> d = 0.0;
        b = std::min(b, size() - 1);
        d(b) = dim(b);
        const auto s = d * (1 - f) / 2; // shift
        extend(-s);
    }
    /// \}

    template<class S> friend pure S concept_name(const box& m) { S s("0-Dimensional Axis-Aligned Box"); s[0] += N; return s; }
    template<class S> friend pure S typenameof(const box& a) { S s("box<T,N>"); return s; }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & l() & u(); }

    friend box& rand(box& a) { rand(a.l()); rand(a.u()); return a; }
};

/*! 2-Dimensional Axis-Aligned (Hyper) Box.
 * Explicit Specialization gives convience constructors (lx,ly,ux,uy).
 */
template<class T> class box2 : public box<T,2>
{
public:
    box2() : box2(0, 1) {}        ///< Construct Default Unit Box.
    box2(T a) : box2(vec2<T>(a)) {}
    box2(vec2<T> a) : box2(a, a) {} ///< Construct Empty Box (Point).
    box2(vec2<T> a,
         vec2<T> b) : box<T,2>(a, b) {}
    box2(vec2<T> c, T r) : box2(c-r, c+r) {} ///< Square with Center \p c and Radius \p rad.
    box2(T lx, T ly,
         T ux, T uy) : box2(vec2<T>(lx,ly),
                            vec2<T>(ux,uy)) {} ///< NOTE: This is worth all the fuzz. TODO: Is this faster than C++11 Initializer Lists?
};

/*! 3-Dimensional Axis-Aligned (Hyper) Box.
 * Explicit Specialization gives convience constructors (lx,ly,lz,ux,uy,yz).
 */
template<class T> class box3 : public box<T,3>
{
public:
    box3() : box3(0, 1) {}        ///< Construct Default Unit Box.
    box3(T a) : box3(vec3<T>(a)) {}
    box3(vec3<T> a) : box3(a, a) {} ///< Construct Empty Box (Point).
    box3(vec3<T> a,
         vec3<T> b) : box<T,3>(a, b) {}
    box3(vec3<T> c, T r) : box3(c-r, c+r) {} ///< Square with Center \p c and Radius \p rad.
    box3(T lx, T ly, T lz,
         T ux, T uy, T uz) : box3(vec3<T>(lx,ly,lz),
                                  vec3<T>(ux,uy,uz)) {} ///< NOTE: This is worth all the fuzz. TODO: Is this faster than C++11 Initializer Lists?
};

#if HAVE_CXX11_TEMPLATE_ALIASES
// template<class T> using box2 = box<T,2>;
// template<class T> using box3 = box<T,3>;
#else
#endif

typedef box2<float> box2f;
typedef box3<float> box3f;
typedef box2<double> box2d;
typedef box3<double> box3d;
typedef box2<long double> box2l;
typedef box3<long double> box3l;

// Show
template<class T, std::size_t N> inline std::ostream&         show_box(std::ostream& os, const box<T,N>& a) {
    if (is_defined(a)) {
        os << "(l" << MEMCH << a.l() << " ⇘ u" << MEMCH << a.u() << ")"; return os;
    } else {
        os << "(undefined)";
    }
    return os;
}
template<class T> inline std::ostream& show(std::ostream& os, const box2<T>& a) { return show_box(os, a); }
template<class T> inline std::ostream& show(std::ostream& os, const box3<T>& a) { return show_box(os, a); }
template<class T, std::size_t N> inline std::ostream& operator << (std::ostream& os, const box<T,N>& a) { return show(os, a); }

/*! Area of \p a. */
template<class T, std::size_t N> inline pure T area  (const box2<T>& a) { auto d = a.dim(); return d(0) * d(1); }
/*! Volume of \p a. */
template<class T, std::size_t N> inline pure T volume(const box3<T>& a) { auto d = a.dim(); return d(0) * d(1) * d(2); }

/*! Distance from \p c to Edge of the \p d. */
template<class T, std::size_t N> inline pure T edge_distance(const vec<T,N>& c,
                                                             const box<T,N>& x)
{
    const size_t pdim = c.dimnum();
    const auto& a = x.l(); // first corner
    const auto& b = x.u();
    int l = 0, r = 0;   // left, right flags (for each dimension)
    bool inside = true; // point c inside rectangle ext flag
    for (size_t i = 0; i < pdim; i++) {
        pnw::ip_put_bit(l, i, c(i) < a(i));
        pnw::ip_put_bit(r, i, b(i) < c(i));
        if (pnw::get_bit(l, i) or
            pnw::get_bit(r, i)) {
            inside = false;
        }
    }
    vec<T,N> d;
    if (inside) {
        T m = std::numeric_limits<T>::max();
        for (size_t i = 0; i < pdim; i++) {
            d(i) = std::min(c(i) - a(i),
                            b(i) - c(i));
            m = std::min(m, d(i));
        }
        return m;
    } else {
        for (size_t i = 0; i < pdim; i++) {
            if (pnw::get_bit(l, i)) {
                d(i) = a(i) - c(i);
            } else if (pnw::get_bit(r, i)) {
                d(i) = c(i) - b(i);
            } else {
                d(i) = 0;
            }
        }
        return norm(d);
    }
}

/*! Distance from \p c to Edge of the \p d. */
template<class T, std::size_t N> inline pure T edge_distance(const box<T,N>& x,
                                                              const vec<T,N>& c) { return edge_distance(c, x); }  // commutative

/*! Top Center of \p a. */
template<class T> inline pure vec2<T> cenT(const box2<T>& a) { return vec2<T>((a.l(0) + a.u(0))/2,
                                                                              (a.l(1))); }
/*! Bottom Center of \p a. */
template<class T> inline pure vec2<T> cenB(const box2<T>& a) { return vec2<T>((a.l(0) + a.u(0))/2,
                                                                              (a.u(1))); }
/*! Left Center of \p a. */
template<class T> inline pure vec2<T> cenL(const box2<T>& a) { return vec2<T>((a.l(0)),
                                                                              (a.l(1) + a.u(1))/2); }
/*! Right Center of \p a. */
template<class T> inline pure vec2<T> cenR(const box2<T>& a) { return vec2<T>((a.u(0)),
                                                                              (a.l(1) + a.u(1))/2); }

/*! Box with \em Upper Center at \p c with dimensions \p d. */
template<class T> inline pure box2<T> box_tcen_dim(const vec2<T>& c,
                                                   const vec2<T>& d) {
    return box2<T>(c(0) - d(0)/2, c(1),
                   c(0) + d(0)/2, c(1) + d(1));
}
/*! Box with \em Bottom Center at \p c with dimensions \p d. */
template<class T> inline pure box2<T> box_bcen_dim(const vec2<T>& c,
                                                   const vec2<T>& d) {
    return box2<T>(c(0) - d(0)/2, c(1) - d(1),
                   c(0) + d(0)/2, c(1));
}
/*! Box with \em Right Center at \p c with dimensions \p d. */
template<class T> inline pure box2<T> box_rcen_dim(const vec2<T>& c,
                                                   const vec2<T>& d) {
    return box2<T>(c(0) - d(0), c(1) - d(1)/2,
                   c(0),        c(1) + d(1)/2);
}
/*! Box with \em Left Center at \p c with dimensions \p d. */
template<class T> inline pure box2<T> box_lcen_dim(const vec2<T>& c,
                                                   const vec2<T>& d){
    return box2<T>(c(0),        c(1) - d(1)/2,
                   c(0) + d(0), c(1) + d(1)/2);
}

/*! Make Box \p b respecting \p a's \em Preferred \em Aspect \em Ratio (Positive) \p arat.
 * \param[in] outer_flag non-zero if outer box is wanted, otherwise inner box.
 */
template<class T, std::size_t N>
inline pure box2<T> make_box2_PARadjusted(const box2<T>& b,
                                          const vec2<T>& arat,
                                          bool outer_flag)
{
    arat = std::abs(arat);      // force positive
    auto c = b.cen();
    auto d = b.d();
    auto q = d / arat;
    vec2<T> s;          // sides
    if (q(0) < q(1)) {
        if (outer_flag) {
            s = arat * q(1)/2;
        } else {
            s = arat * q(0)/2;
        }
    } else if (q(0) > q(1)) {
        if (outer_flag) {
            s = arat * q(0)/2;
        } else {
            s = arat * q(1)/2;
        }
    } else {
        s = d/2;
    }
    return box2<T>(c-s, c+s);
}

/*! Put Inner Romb of \p box into \p romb. */
template<class T, std::size_t N>
void inner_romb(const box2<T>& box, vec<T,N> romb[4])
{
    auto c = box.cen();             // center
    romb[0] = { c(0),     box.l(1) };
    romb[1] = { box.u(0), c(1) };
    romb[2] = { c(0),     box.u(1) };
    romb[3] = { box.l(0), c(1) };
}

/*! Resize \p a using the user (mouse) drag from \p mpos the shift \p mshift.
 * \param[in] mask Control Point Mask (4 LSBits) to use.
 * \return 1 if extents were changed, -1 if type not supported, 0 otherwise.
 */
template<class T, std::size_t N>
box<T,N> shiftCtrlP(box<T,N>& a,
                    const vec<T,N>& mpos, const vec<T,N>& mshift,
                    const vec<T,N>& min_dim, const uint8_t * mask,
                    const vec<T,N>& PAR)
{
    auto dim = a.dim();

    bool xOK = true, yOK = true;
    if (min_dim != 0) {                            /* if minimum dimension given */
        xOK = (dim(0) - mshift(0)) > min_dim(0); /* OK to shift along x */
        yOK = (dim(1) - mshift(1)) > min_dim(1); /* OK to shift along y */
    }

    int b0 = pnw::get_bit(*mask, 0);
    int b1 = pnw::get_bit(*mask, 1);
    int b2 = pnw::get_bit(*mask, 2);
    int b3 = pnw::get_bit(*mask, 3);

    if (b0 or b2) { if (xOK) { a.l(0) += mshift(0); } }
    if (b1 or b3) { if (xOK) { a.u(0) += mshift(0); } }
    if (b0 or b1) { if (yOK) { a.l(1) += mshift(1); } }
    if (b2 or b3) { if (yOK) { a.u(1) += mshift(1); } }

    if (PAR != 0) {                    /* if PAR should be respected */
        auto dimN = a.dim();
        T nX = dimN(0) / PAR(0); /* normalized X */
        T nY = dimN(1) / PAR(1); /* normalized Y */

        vec<T,N> dimNA;                  /* adjusted new dimension */
        if (nX >= nY) {                /* if X-major shift */
            dimNA(0) = dimN(0);            /* X leads */
            dimNA(1) = nX * PAR(1);      /* Y follows */
        } else {                      /* if Y-major shift */
            dimNA(1) = dimN(1);            /* Y leads */
            dimNA(0) = nY * PAR(0);      /* X follows */
        }

        auto dimAD = dimNA - dimN;                /* dimension adjustment difference */

        /* do real adjustment */
        if (b0 or b2) { if (xOK) { a.l(0) -= dimAD(0); } }
        if (b1 or b3) { if (xOK) { a.u(0) += dimAD(0); } }
        if (b0 or b1) { if (yOK) { a.l(1) -= dimAD(1); } }
        if (b2 or b3) { if (yOK) { a.u(1) += dimAD(1); } }
    }

    return a;
}

#if HAVE_CXX11_EXTERN_TEMPLATES
extern template box<float,2> shiftCtrlP(box<float,2>& a,
                                        const vec<float,2>& mpos,
                                        const vec<float,2>& mshift,
                                        const vec<float,2>& min_dim,
                                        const uint8_t * mask,
                                        const vec<float,2>& PAR);
#endif

#if 0

/*! Return non-zero if \p a and [\p lx, \p ly, \p ux, \p uy] are <em>equal</em>. */
inline pure bool box2<T>_eq_f(const box2<T>& a, T lx, T ly, T ux, T uy)
{
    return (a.l(0) == lx and
            a.l(1) == ly and
            a.u(0) == ux and
            a.u(1) == uy);
}

/*! Return non-zero if \p s and [\p lx, \p ly, \p ux, \p uy] are <em>equal</em>. */
inline pure bool box<T,N>_eq_sf(const box<T,N>& a, T s)
{
    return (a.l(0) == s and
            a.l(1) == s and
            a.u(0) == s and
            a.u(1) == s);
}

/*! Return non-zero if \p a and [\p lx, \p ly, \p ux, \p uy] are <em>not
 *  equal</em>. */
inline pure bool box<T,N>_neq_f(const box<T,N>& a, T lx, T ly, T ux, T uy)
{
    return (a.l(0) != lx or
            a.l(1) != ly or
            a.u(0) != ux or
            a.u(1) != uy);
}

/*! Return non-zero if \p a and \p b are equal. */
inline pure bool box<T,N>_eq(const box<T,N>& a, const box<T,N>& b)
{
    return (vec<T,N>_eq(a.l, b.l) and
            vec<T,N>_eq(a.u, &b.u));
}

/*! Return non-zero if \p a and \p b are not equal. */
inline pure bool box<T,N>_neq(const box<T,N>& a, const box<T,N>& b)
{
    return (vec<T,N>_neq(&a.l, &b.l) or
            vec<T,N>_neq(&a.u, &b.u));
}

/*! Extend the sides of \p a the shift \p s. */
inline void
box<T,N>_extendself(box<T,N>& a, const vec<T,N>& s)
{
    a.l(0) -= s(0);
    a.l(1) -= s(1);
    a.u(0) += s(0);
    a.u(1) += s(1);
}

/*! Extend the sides of \p a the shift \p s. */
inline void
box<T,N>_extendselfxy(box<T,N>& a, T sx, T sy)
{
    a.l(0) -= sx;
    a.l(1) -= sy;
    a.u(0) += sx;
    a.u(1) += sy;
}

/*! Extend the sides of \p a the shift \p s. */
inline box<T,N>
box<T,N>_extend(const box<T,N>& a, const vec<T,N>& s)
{
    box<T,N> b;
    b.l(0) = a.l(0) - s(0);
    b.l(1) = a.l(1) - s(1);
    b.u(0) = a.u(0) + s(0);
    b.u(1) = a.u(1) + s(1);
    return b;
}

/*! Extend the sides of \p a the scalar shift \p s. */
inline box<T,N>
box<T,N>_extends(const box<T,N>& a, const T s)
{
    box<T,N> b;
    b.l(0) = a.l(0) - s;
    b.l(1) = a.l(1) - s;
    b.u(0) = a.u(0) + s;
    b.u(1) = a.u(1) + s;
    return b;
}

/*! Horizontally Extend the sides of \p a the shift \p sx. */
inline box<T,N>
box<T,N>_extendH(const box<T,N>& a, T sx)
{
    box<T,N> b;
    b.l(0) = a.l(0) - sx;
    b.l(1) = a.l(1);
    b.u(0) = a.u(0) + sx;
    b.u(1) = a.u(1);
    return b;
}

/*! Vertically Extend the sides of \p a the shift \p sy. */
inline box<T,N>
box<T,N>_extendV(const box<T,N>& a, T sy)
{
    box<T,N> b;
    b.l(0) = a.l(0);
    b.l(1) = a.l(1) - sy;
    b.u(0) = a.u(0);
    b.u(1) = a.u(1) + sy;
    return b;
}

/*! Read the Control Point Mask of \p box at \p mpos and bit-or it with contents
 *  of \p mask. */
int
box<T,N>_rdCtrlPM_SFORM(const box<T,N>& box,
                        const vec<T,N>& mpos, uint8_t * mask,
                        uint32_t * rsz_i, bool set_flag);

/*! Read the Control Point Mask of \p box at \p mpos and bit-or it with contents
 *  of \p mask. */
int
box<T,N>_rdCtrlPM_SFILL(const box<T,N>& box, SFILL_t sfill,
                        const vec<T,N>& mpos, uint8_t * mask,
                        uint32_t * rsz_i, bool set_flag);

/*! Shift the sides of \p a the shift \p s. */
inline void box<T,N>_shift(box<T,N>& a, const vec<T,N>& s) {
    a.l(0) = a.l(0) + s(0);
    a.l(1) = a.l(1) + s(1);
    a.u(0) = a.u(0) + s(0);
    a.u(1) = a.u(1) + s(1);
}

/*! Horizontally Shift the sides of \p a the shift \p sx. */
inline void box<T,N>_hshift(box<T,N>& a, T sx) {
    a.l(0) = a.l(0) + sx;
    a.u(0) = a.u(0) + sx;
}

/*! Vertically Shift the sides of \p a the shift \p sy. */
inline void box<T,N>_vshift(box<T,N>& a, T sy) {
    a.l(1) = a.l(1) + sy;
    a.u(1) = a.u(1) + sy;
}

/*! Horizontally Translate the sides of \p a the shift \p sx. */
inline void box<T,N>_htransl(const box<T,N>& a, T sx, box<T,N>& b) {
    b.l(0) = a.l(0) + sx;
    b.l(1) = a.l(1);
    b.u(0) = a.u(0) + sx;
    b.u(1) = a.u(1);
}

/*! Vertically Translate the sides of \p a the shift \p sy. */
inline void box<T,N>_vtransl(const box<T,N>& a, T sy, box<T,N>& b) {
    b.l(0) = a.l(0);
    b.l(1) = a.l(1) + sy;
    b.u(0) = a.u(0);
    b.u(1) = a.u(1) + sy;
}

/*! Centered Minimum Or Maximum Scale.
 *
 * Used to generate a frame to \p a to that has a frame width of
 * MIN or MAX of (f * a.width, f * a.height)
 *
 * \param[in] maxflag non-zero if max-dimension is used,
 * 0 if min-dimension is used.
 */
inline void
box<T,N>_scaleMoM(const box<T,N>& a, T f, box<T,N>& b, int maxflag)
{
    vec<T,N> dim; box<T,N>_rdDim(a, &dim); /* dimension */

    T w = (maxflag ?		/* frame width */
           max(dim(0), dim(1)) :
           min(dim(0), dim(1))) * (1.0f - f)/2;

    vec<T,N> s;			/* shift */
    s(0) = - w;
    s(1) = - w;

    *b = *a;
    box<T,N>_extendself(b, &s);
}

/*! Inner Square of \p a scaled the factor \p f. */
inline box<T,N>
box<T,N>_rdISqr(const box<T,N>& a, T f)
{
    vec<T,N> dim; box<T,N>_rdDim(a, &dim); /* dimension */
    T s = f * std::min(dim(0), dim(1))/2; /* shift */
    return box(a.cen(), s);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Sub-Vec when \p ba is viewed relative to \p a and put
 * result in \p b.
 * Inverse Operation of \c vec<T,N>_getRel().
 */
inline vec<T,N> getSub(const box<T,N>& a, const const vec<T,N>& ba) { return a.l + a.dim() * ba; }

/*! Relative Vector \p ba when \p b is viewed in relation to \p a.
 * Inverse Operation of \c vec<T,N>_getSub().
 */
inline vec<T,N> getRel(const box<T,N>& a, const const vec<T,N>& b) { return (b - a.l) / a.dim(); }

/*! Relative Affine Transform.
 * Upon changing \p extP to \p extN change the \p src to @p dst accordingly so
 * that the \p src relative size position relative to \p extP is preserved.
 */
inline vec<T,N> relaffine(const box<T,N>& extP, const box<T,N>& extN, const const vec<T,N>& src) { return getSub(extN, getRel(extP, src)); }
inline box<T,N> relaffine(const box<T,N>& extP, const box<T,N>& extN, const box<T,N>& src) { return getSub(extN, getRel(extP, src)); }

/* ---------------------------- Group Separator ---------------------------- */

/*! Sub-Box when [\p rlx, \p rly, \p rux, \p ruy] is viewed relative to \p a.
 * The inverse operation of \c box<T,N>_getRel().
 */
inline box<T,N>;
box<T,N>_getSubs(const box<T,N>& a,
                T rlx, T rly, T rux, T ruy)
{
    vec<T,N> ad; box<T,N>_rdDim(a, &ad);
    box<T,N> b;
    box<T,N>_set(b,
                 a.l(0) + ad(0) * rlx,
                 a.l(1) + ad(1) * rly,
                 a.l(0) + ad(0) * rux,
                 a.l(1) + ad(1) * ruy);
    return b;
}

/*! Sub-Box when \p ba is viewed relative to \p a.
 * The inverse operation of \c box<T,N>_getRel().
 */
inline box<T,N> box<T,N>_getSub(const box<T,N>& a, const box<T,N>& ba)
{
    box<T,N> b;
    box<T,N>_getSubs(a,
                    ba.l(0), ba.l(1), ba.u(0), ba.u(1),
                    b);
    return b;
}

/*! Compute the Relative Box \p ba when \p b is viewed in relation to \p a.
 * The inverse operation of \c box<T,N>_getSub().
 * \return 1 if a relative box was defined, 0 otherwise.
 */
inline bool
box<T,N>_getRel(const box<T,N>& a, const box<T,N>& b, box<T,N>& ba)
{
    int ret = 0;
    vec<T,N> ad; box<T,N>_rdDim(a, &ad);
    ba.l(0) = (b.l(0) - a.l(0)) / ad(0);
    ba.l(1) = (b.l(1) - a.l(1)) / ad(1);
    ba.u(0) = (b.u(0) - a.l(0)) / ad(0);
    ba.u(1) = (b.u(1) - a.l(1)) / ad(1);
    if (ad(0) != 0 and ad(1) != 0) { ret = 1; }
    else { PWARN("Relative box is undefined\n"); }
    return ret;
}

/*! Step \p a its whole dimensions one step to the "Bottom-Right". */
inline void
box<T,N>_stepBR(const box<T,N>& a, box<T,N>& b)
{
    b.l(0) = a.u(0);
    b.l(1) = a.u(1);
    b.u(0) = a.u(0) + (a.u(0) - a.l(0));
    b.u(1) = a.u(1) + (a.u(1) - a.l(1));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Left-Aligned Horizontal Scale \p a the factor \p f. */
inline box<T,N>_scaleLH(const box<T,N>& a, T f, box<T,N>& b)
{
    T aw = a.u(0) - a.l(0);	/* width of a */
    b.l(0) = a.l(0);
    b.l(1) = a.l(1);
    b.u(0) = a.l(0) + f*aw;
    b.u(1) = a.u(1);
}

/*! Right-Aligned Horizontal Scale \p a the factor \p f. */
inline box<T,N>_scaleRH(const box<T,N>& a, T f, box<T,N>& b)
{
    T aw = a.u(0) - a.l(0);	/* width of a */
    b.l(0) = a.u(0) - f*aw;
    b.l(1) = a.l(1);
    b.u(0) = a.u(0);
    b.u(1) = a.u(1);
}

/*! Top-Aligned Vertical Scale \p a the factor \p f. */
inline box<T,N>_scaleTV(const box<T,N>& a, T f) {
    box<T,N> b;
    T ah = a.u(1) - a.l(1);	/* height of a */
    b.l(0) = a.l(0);
    b.l(1) = a.l(1);
    b.u(0) = a.u(0);
    b.u(1) = a.l(1) + f*ah;
    return b;
}

/*! Bottom-Aligned Vertical Scale \p a the factor \p f.  */
inline box<T,N>_scaleBV(const box<T,N>& a, T f) {
    box<T,N> b;
    T ah = a.u(1) - a.l(1);	/* height of a */
    b.l(0) = a.l(0);
    b.l(1) = a.u(1) - f*ah;
    b.u(0) = a.u(0);
    b.u(1) = a.u(1);
    return b;
}

/*! Center-Aligned Horizontal Scale \p a the factor \p f.
 */
inline box<T,N> scaleCH(const box<T,N>& a, T f) {
    box<T,N> b;
    T aw = a.u(0) - a.l(0);	/* width of a */
    b.l(0) = a.l(0) + aw * (1 - f)/2;
    b.l(1) = a.l(1);
    b.u(0) = a.u(0) - aw * (1 - f)/2;
    b.u(1) = a.u(1);
    return b;
}

/*! Center-aligned Vertical Scale \p a the factor \p f. */
inline box<T,N> scaleCV(const box<T,N>& a, T f) {
    box<T,N> b;
    T ah = a.u(1) - a.l(1);	/* height of a */
    b.l(0) = a.l(0);
    b.l(1) = a.l(1) + ah * (1 - f)/2;
    b.u(0) = a.u(0);
    b.u(1) = a.u(1) - ah * (1 - f)/2;
    return b;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Extract Horizontal Sub-Box of \p a from \p i to \p j. */
inline box<T,N> subH(const box<T,N>& a, T i, T j) {
    box<T,N> b;
    T aw = a.u(0) - a.l(0);	/* width of a */
    T alx = a.l(0);
    b.l(0) = alx + aw * i;
    b.l(1) = a.l(1);
    b.u(0) = alx + aw * j;
    b.u(1) = a.u(1);
    return b;
}

/*! Extract Vertical Sub-Box of \p a from \p i to \p j. */
inline box<T,N> subV(const box<T,N>& a, T i, T j) {
    box<T,N> b;
    T ah = a.u(1) - a.l(1);	/* height of a */
    T aly = a.l(1);
    b.l(0) = a.l(0);
    b.l(1) = aly + ah * i;
    b.u(0) = a.u(0);
    b.u(1) = aly + ah * j;
    return b;
}

/*! Extract Horizontal Sub-Box of \p a from \p i / \p n to \p j / \p n. */
inline box<T,N> subHi(const box<T,N>& a, int i, int j, int n) {
    box<T,N> b;
    T aw = a.u(0) - a.l(0);	/* width of a */
    T alx = a.l(0);
    b.l(0) = alx + aw * i / n;
    b.l(1) = a.l(1);
    b.u(0) = alx + aw * j / n;
    b.u(1) = a.u(1);
    return b;
}

/*! Extract Vertical Sub-Box of \p a from \p i / \p n to \p j / \p n. */
inline box<T,N> subVi(const box<T,N>& a, int i, int j, int n) {
    box<T,N> b;
    T ah = a.u(1) - a.l(1);	/* height of a */
    T aly = a.l(1);
    b.l(0) = a.l(0);
    b.l(1) = aly + ah * (T)i / n;
    b.u(0) = a.u(0);
    b.u(1) = aly + ah * (T)j / n;
    return b;
}

/*! Extract Horizontal and Vertical Sub-Box of \p a from \p i / \p n to
 * (\p i + \p l) / \p n in int-precision.
 */
inline box<T,N> subHVij_i(const box<T,N>& a,
                          int ix, int lx, int nx,
                          int iy, int ly, int ny) {
    box<T,N> b;
    vec<T,N> dim; box<T,N>_rdDim(a, &dim);
    T alx = a.l(0), aly = a.l(1);
    b.l(0) = alx + dim(0) * ix / nx;
    b.l(1) = aly + dim(1) * iy / ny;
    b.u(0) = alx + dim(0) * (ix + lx) / nx;
    b.u(1) = aly + dim(1) * (iy + ly) / ny;
    return b;
}

/*! Extract Horizontal and Vertical Sub-Box of \p a from \p i / \p n to
 * (\p i + \p l) / \p n in T-precision. */
inline box<T,N> subHVij_f(const box<T,N>& a,
                          T ix, T lx, T nx,
                          T iy, T ly, T ny) {
    box<T,N> b;
    vec<T,N> dim; box<T,N>_rdDim(a, &dim);
    T alx = a.l(0), aly = a.l(1);
    b.l(0) = alx + dim(0) * ix / nx;
    b.l(1) = aly + dim(1) * iy / ny;
    b.u(0) = alx + dim(0) * (ix + lx) / nx;
    b.u(1) = aly + dim(1) * (iy + ly) / ny;
    return b;
}

/*! Extract Horizontal and Vertical Sub-Box of \p a from \p i / \p n to
 * (\p i + \p l) / \p n in double-precision.
 */
inline box<T,N> subHVij_d(const box<T,N>& a,
                          double ix, double lx, double nx,
                          double iy, double ly, double ny)
{
    box<T,N> b;
    vec<T,N> dim; box<T,N>_rdDim(a, &dim);
    T alx = a.l(0), aly = a.l(1);
    b.l(0) = alx + dim(0) * ix / nx;
    b.l(1) = aly + dim(1) * iy / ny;
    b.u(0) = alx + dim(0) * (ix + lx) / nx;
    b.u(1) = aly + dim(1) * (iy + ly) / ny;
    return b;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Return non-zero if \p a lies inclusively inside \p b along x. */
inline pure bool insideX<T,N>(const box<T,N>& a, const box<T,N>& b) {
    return (b.l(0) <= a.l(0) and a.u(0) <= b.u(0)); /* covers along x */
}

/*! Return non-zero if \p a lies inclusively inside \p b along y. */
inline pure bool insideY<T,N>(const box<T,N>& a, const box<T,N>& b) {
    return (b.l(1) <= a.l(1) and a.u(1) <= b.u(1)); /* covers along y */
}

/*! Return non-zero if \p a lies inclusively inside b. */
inline pure bool inside<T,N>(const box<T,N>& a, const box<T,N>& b) {
    return (box<T,N>_insideX_box<T,N>(a, b) and /* covers along x */
            box<T,N>_insideY_box<T,N>(a, b)); /* covers along y */
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Unite \p a and \p b into \p c.
 */
inline void box<T,N>_unitev(const box<T,N>& a, const vec<T,N>& b, box<T,N>& c)
{
    c.l(0) = std::min(a.l(0), b(0));
    c.l(1) = std::min(a.l(1), b(1));
    c.u(0) = std::max(a.u(0), b(0));
    c.u(1) = std::max(a.u(1), b(1));
}

/*! Unite \p a and \p b into \p a. */
inline void box<T,N>_uniteself(box<T,N>& a, const box<T,N>& b)
{
    box<T,N>_unite(a, b, a);
}

/*! Unite \p a and \p b into \p a. */
inline void box<T,N>_unitevself(box<T,N>& a, const vec<T,N>& b)
{
    box<T,N>_unitev(a, b, a);
}

/*! Truncate box \p a to fit inside \p b. */
inline pure box<T,N> box<T,N>_truncInto(const box<T,N>& a, const box<T,N>& b)
{
    const box<T,N> t = *b;		/* temporary */
    box<T,N> c;
    c.l(0) = clamp(t.l(0), a.l(0), t.u(0));
    c.u(0) = clamp(t.l(0), a.u(0), t.u(0));
    c.l(1) = clamp(t.l(1), a.l(1), t.u(1));
    c.u(1) = clamp(t.l(1), a.u(1), t.u(1));
    return c;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Indexing of box \p a at limit \p i along dimensinon j, or for short at
 *  (i,j). */
inline pure T box<T,N>_at2(const box<T,N>& a, int i, int j)
{
    return (&(&a.l)[i](0))[j];
}

/*! Determine the Spatial Relation between the two boxes \p a and \p b.
 * \return relation as an integer that is > 0 if \p a and \p b are spatially
 * separated. Individually
 * - bit 0 is set if \p a lies before \p b along dimension 0
 * - bit 1 is set if \p b lies before \p a along dimension 0
 * - bit 2 is set if \p a lies before \p b along dimension 1
 * - bit 3 is set if \p a lies before \p b along dimension 1
 */
inline pure size_t box<T,N>_relate(const box<T,N>& a, const box<T,N>& b)
{
    return ((a.u(0) < b.l(0)) << 0 | // left of
            (a.l(0) > b.u(0)) << 1 | // right of
            (a.u(1) < b.l(1)) << 2 | // above
            (a.l(1) > b.u(1)) << 3); // below
}

/*! Matrix-Vector (Self) Multiply "around Point", y = A * (y-c) + c;
 * \complexity 4 MUL, 4 ADD, 2 SUB
 */
inline void mat2f_sub_mvmul_add(vec<T,N>& y, mat2f a, vec<T,N>& c)
{
    vec<T,N>_sub(y, c);
    mat2f_mvprod(y, a, y);
    vec<T,N>_add(y, c);
}

/*! Pointwise addition, Transform and multiply.
 * d = (a * b) * M + c;
 */
inline void mat2f_pwmul_mvmul_add(const vec<T,N>& a, const vec<T,N>& b,
                                  mat2f M, const vec<T,N>& c,
                                  const vec<T,N>&& d)
{
    vec<T,N>_pwprod(a, b, d);
    vec<T,N>_mvmul_mat2f(d, M);
    vec<T,N>_add(d, c);
}

/*! Rotate set of \p vtxN number of vertices at \p vtx around position
 * (center) \p cen the angle \p ang.
 *
 * \param[in] cen if non-NULL Center Position around which rotation is
 *                performed, if NULL default center position to [0, 0].
 * \param[in] ang rotation angle [Radians]
 */
void vec<T,N>array_rot(vec<T,N>& vtx, size_t vtxN,
                       const vec<T,N>& cen, T ang);

/* ---------------------------- Group Separator ---------------------------- */

/*! Read Bounding Box of the array vtx of length vtxN into \p ext.
 * \return 1 if ext was read, 0 otherwise.
 */
int vec<T,N>array_rdBnd_box<T,N>(const vec<T,N>& vtx, size_t vtxN, box<T,N>& ext);

/*! Read Bounding Box of the pair \p pair into \p ext.
 */
inline void
vec<T,N>_pair_rdBnd_box<T,N>(const vec<T,N> pair[2], box<T,N>& ext)
{
    ext.l(0) = std::min(pair[0](0), pair[1](0));
    ext.l(1) = std::min(pair[0](1), pair[1](1));
    ext.u(0) = std::max(pair[0](0), pair[1](0));
    ext.u(1) = std::max(pair[0](1), pair[1](1));
}

/*! Read Bounding Box of the Triangle \p tri into \p ext.
 */
inline void
vec<T,N>_tri_rdBnd_box<T,N>(const vec<T,N> tri[3], box<T,N>& ext)
{
    ext.l(0) = std::min3(tri[0](0), tri[1](0), tri[2](0));
    ext.l(1) = std::min3(tri[0](1), tri[1](1), tri[2](1));
    ext.u(0) = std::max3(tri[0](0), tri[1](0), tri[2](0));
    ext.u(1) = std::max3(tri[0](1), tri[1](1), tri[2](1));
}

/*! Read Bounding Box of the Quad \p quad into \p ext.
 */
inline void
vec<T,N>_quad_rdBnd_box<T,N>(const vec<T,N> quad[4], box<T,N>& ext)
{
    ext.l(0) = std::min4(quad[0](0), quad[1](0), quad[2](0), quad[3](0));
    ext.l(1) = std::min4(quad[0](1), quad[1](1), quad[2](1), quad[3](1));
    ext.u(0) = std::max4(quad[0](0), quad[1](0), quad[2](0), quad[3](0));
    ext.u(1) = std::max4(quad[0](1), quad[1](1), quad[2](1), quad[3](1));
}

/*! Read Bounding Box of the Pentagon \p pent into \p ext. */
inline void
vec<T,N>_pent_rdBnd_box<T,N>(const vec<T,N> pent[5], box<T,N>& ext)
{
    ext.l(0) = std::min5(pent[0](0), pent[1](0), pent[2](0), pent[3](0), pent[4](0));
    ext.l(1) = std::min5(pent[0](1), pent[1](1), pent[2](1), pent[3](1), pent[4](1));
    ext.u(0) = std::max5(pent[0](0), pent[1](0), pent[2](0), pent[3](0), pent[4](0));
    ext.u(1) = std::max5(pent[0](1), pent[1](1), pent[2](1), pent[3](1), pent[4](1));
}

/*! Read Bounding Box of the Hexagon \p hex into \p ext. */
inline void
vec<T,N>_hex_rdBnd_box<T,N>(const vec<T,N> hex[6], box<T,N>& ext)
{
    ext.l(0) = std::min6(hex[0](0), hex[1](0), hex[2](0), hex[3](0), hex[4](0), hex[5](0));
    ext.l(1) = std::min6(hex[0](1), hex[1](1), hex[2](1), hex[3](1), hex[4](1), hex[5](1));
    ext.u(0) = std::max6(hex[0](0), hex[1](0), hex[2](0), hex[3](0), hex[4](0), hex[5](0));
    ext.u(1) = std::max6(hex[0](1), hex[1](1), hex[2](1), hex[3](1), hex[4](1), hex[5](1));
}

/*! Read Bounding Box of the Heptagon \p hept into \p ext. */
inline void
vec<T,N>_hept_rdBnd_box<T,N>(const vec<T,N> hept[6], box<T,N>& ext)
{
    ext.l(0) = std::min7(hept[0](0), hept[1](0), hept[2](0), hept[3](0), hept[4](0), hept[5](0), hept[6](0));
    ext.l(1) = std::min7(hept[0](1), hept[1](1), hept[2](1), hept[3](1), hept[4](1), hept[5](1), hept[6](1));
    ext.u(0) = std::max7(hept[0](0), hept[1](0), hept[2](0), hept[3](0), hept[4](0), hept[5](0), hept[6](0));
    ext.u(1) = std::max7(hept[0](1), hept[1](1), hept[2](1), hept[3](1), hept[4](1), hept[5](1), hept[6](1));
}

void
vec<T,N>array_rot(vec<T,N>& vtx, size_t vtxN, const vec<T,N>& cen, T ang)
{
    mat2f rottab = rotation(ang);		  /* rotation matrix (table with one entry) */
    if (cen) {                    /* if center given */
        for (size_t i = 0; i < vtxN; i++) {
            vec<T,N>_sub(&vtx[i], cen);
            vec<T,N>_mvmul_mat2f(&vtx[i], &rottab);
            vec<T,N>_add(&vtx[i], cen);
        }
    } else {                      /* if no center given */
        for (size_t i = 0; i < vtxN; i++) {
            vec<T,N>_mvmul_mat2f(&vtx[i], &rottab); /* rotate around origo */
        }
    }
}

int vec<T,N>array_rdBnd_box<T,N>(const vec<T,N>& vtx, size_t vtxN, box<T,N>& ext)
{
    int ret = 0;
    if (vtxN > 0) {
        ext.l(0) = ext.u(0) = vtx[0](0);
        ext.l(1) = ext.u(1) = vtx[0](1);
        for (size_t i = 1; i < vtxN; i++) {
            ext.l(0) = std::min(ext.l(0), vtx[i](0));
            ext.l(1) = std::min(ext.l(1), vtx[i](1));
            ext.u(0) = std::max(ext.u(0), vtx[i](0));
            ext.u(1) = std::max(ext.u(1), vtx[i](1));
        }
        ret = 1;
    }
    return ret;
}

int
box<T,N>_rdCtrlPM_SFORM(const box<T,N>& box,
                        const vec<T,N>& mpos, uint8_t * mask,
                        uint32_t * rsz_i, bool set_flag)
{
    auto dim = box.dim();
    auto rlb = box.scale(M_1_3); /* resize limiter box */
    int xi, yi;				     /* x and y index */

    if      (mpos(0) < rlb.l(0)) { xi = -1; }
    else if (mpos(0) > rlb.u(0)) { xi = +1; }
    else                        { xi =  0; }
    if      (mpos(1) < rlb.l(1)) { yi = -1; }
    else if (mpos(1) > rlb.u(1)) { yi = +1; }
    else                        { yi =  0; }

    if      (xi == -1 and yi == -1) { *rsz_i = 0; *mask = int8_set_bitTo  (*mask, 0,       set_flag); }
    else if (xi ==  0 and yi == -1) {             *mask = int8_set2BitsTo(*mask, 0,1,     set_flag); }
    else if (xi == +1 and yi == -1) { *rsz_i = 1; *mask = int8_set_bitTo  (*mask, 1,       set_flag); }

    else if (xi == -1 and yi ==  0) {             *mask = int8_set2BitsTo(*mask, 0,2,     set_flag); }
    else if (xi ==  0 and yi ==  0) {             *mask = int8_set4BitsTo(*mask, 0,1,2,3, set_flag); }
    else if (xi == +1 and yi ==  0) {             *mask = int8_set2BitsTo(*mask, 1,3,     set_flag); }

    else if (xi == -1 and yi == +1) { *rsz_i = 2; *mask = int8_set_bitTo  (*mask, 2,       set_flag); }
    else if (xi ==  0 and yi == +1) {             *mask = int8_set2BitsTo(*mask, 2,3,     set_flag); }
    else if (xi == +1 and yi == +1) { *rsz_i = 3; *mask = int8_set_bitTo  (*mask, 3,       set_flag); }

    return 1;
}

int
box<T,N>_rdCtrlPM_SFILL(const box<T,N>& box, SFILL_t sfill,
                        const vec<T,N>& mpos, uint8_t * mask,
                        uint32_t * rsz_i, bool set_flag)
{
    int ret = 0;
    auto cen = box.cen();
    switch (sfill) {
    case SFILL_SINGLE: {
        *rsz_i = 0;
        *mask = int8_set_bitTo(*mask, 0, set_flag);
        ret = 1;
        break;
    }
    case SFILL_GRADV: {
        int yi;				 /* y index */
        if      (mpos(1) < cen(1)) { yi = 0; } /* "upper" half */
        else                      { yi = 1; } /* "lower" half */
        if (yi == 0) { *rsz_i = 0; *mask = int8_set_bitTo(*mask, 0, set_flag); }
        else         { *rsz_i = 1; *mask = int8_set_bitTo(*mask, 1, set_flag); }
        ret = 1;
        break;
    }
    case SFILL_GRADH: {
        int xi;				 /* x index */
        if      (mpos(0) < cen(0)) { xi = 0; } /* "left" half */
        else                      { xi = 1; } /* "right" half */
        if (xi == 0) { *rsz_i = 0; *mask = int8_set_bitTo(*mask, 0, set_flag); }
        else         { *rsz_i = 1; *mask = int8_set_bitTo(*mask, 1, set_flag); }
        ret = 1;
        break;
    }
    case SFILL_GRADR: {
        box<T,N> inner;
        box<T,N>_getSubs(box, M_1_3, M_1_3, M_2_3, M_2_3, &inner);
        int xi;				 /* x index */
        if (mpos(0) >= inner.l(0) and
            mpos(1) >= inner.l(1) and
            mpos(0) <  inner.u(0) and
            mpos(1) <  inner.u(1)) {
            xi = 0;
        } else { xi = 1; } /* "right" half */
        if (xi == 0) { *rsz_i = 0; *mask = int8_set_bitTo(*mask, 0, set_flag); }
        else         { *rsz_i = 1; *mask = int8_set_bitTo(*mask, 1, set_flag); }
        ret = 1;
        break;
    }
    case SFILL_GRADA:
        PTODO("case SFILL_GRADA\n");
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        PTODO("case SFILL_PAL\n");
        break;
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, sfill); break;
    }

    /*   PTEST("sfill:%d *rsz_i:%d *mask:0x%02x\n", */
    /* 	   sfill, *rsz_i, *mask); */

    return ret;
}

#endif
