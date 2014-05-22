/*! \file polyn.hpp
 * \brief N-Dimensional D-Degree Polygon.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <array>
#include <boost/serialization/access.hpp>
#include "../utils.h"
#include "vec.hpp"
#include "box.hpp"
#include "../mean.hpp"
#include "../algorithm_x.hpp"
#include "outer_box.hpp"

/*! \c N-Dimensional \c D-Degree Polygon.
 */
template<class T, std::size_t N, std::size_t D>
class polyn : public std::array<vec<T,N>, D>
{
public:
    typedef vec<T,N> V;         ///< Value Type.
    polyn(V a = 0) { this->fill(a); }

#if HAVE_CXX11_INITIALIZER_LISTS
    // \see http://stackoverflow.com/questions/5438671/static-assert-on-initializer-listsize
    polyn             (const std::initializer_list<V>& list) { enforce_eq(N, list.size()); std::copy(begin(list), end(list), begin(*this)); }
    polyn& operator = (const std::initializer_list<V>& list) { enforce_eq(N, list.size()); std::copy(begin(list), end(list), begin(*this)); }
#endif
    size_t dimnum() const { return N; }

    /// \name Element Access.
    /// \{
    vec<T,N>  operator () (size_t i) const { return (*this)[i]; }
    vec<T,N>& operator () (size_t i)       { return (*this)[i]; }
    /// \}

    pure box<T,N> outer_box() const { return ::outer_box(*this); }

    /*! Sub-Polygon when \p ba is viewed relative to \p a.
     * The inverse operation of \c getRel().
     */
    friend polyn getSub(const box<T,N>& a, const polyn& ba) {
        polyn b;
        for (size_t i = 0; i != D; ++i) { b(i) = getSub(a, ba(i)); }
        return b;
    }

    /*! Relative Polygon when \p b is viewed in relation to \p a.
     * The inverse operation of \c getSub().
     */
    friend polyn getRel(const box<T,N>& a, const polyn& b) {
        polyn ba;                // b relative to a
        for (size_t i = 0; i != D; ++i) { ba(i) = getRel(a, b(i)); }
        return ba;
    }

    /*! Fit Triangle \p a Precisely Inside (Bounding) Box extents \p ext. */
    void fit_in(const box<T,N>& ext) {
        polyn& a = *this;
        for (size_t i = 0; i != N; ++i) {
            a(i) = relaffine(outer_box(a), ext, a(i));
        }
    }

    friend std::ostream& show        (std::ostream& os, const polyn& a) {
        os << "(";
        for (auto e : a) { os << e << " ⇒ "; }
        os << ")";
        return os;
    }
    friend std::ostream& operator << (std::ostream& os, const polyn& a) { return show(os, a); }

    template<class S> friend pure S concept_name(const polyn& m) { S s("0-Dimensional 0-Degree Fixed-Sized Polygon"); s[0] += N; s[14] += D; return s; }
    template<class S> friend pure S typenameof(const polyn& a) { S s("polyn<T,N,D>"); return s; }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) {
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
        for (auto& e : *this) { ar & e; }
#else
        std::for_each(begin(*this), end(*this), [&ar](T& e) { ar & e; });
#endif
    }

    friend polyn& rand(polyn& a) { rand(*(reinterpret_cast<std::array<vec<T,N>, D>*>(&a))); return a; }
};

#if HAVE_CXX11_TEMPLATE_ALIASES
template<class T, std::size_t N> using tri  = polyn<T,N,3>; ///< Triangle.
template<class T, std::size_t N> using quad = polyn<T,N,4>; ///< Quad
template<class T, std::size_t N> using pent = polyn<T,N,5>; ///< Pentagon
template<class T, std::size_t N, class S> inline pure S concept_name(const tri<T,N>& m) { S s("0-Dimensional Triangle"); s[0] += N; return s; }
template<class T, std::size_t N, class S> inline pure S concept_name(const quad<T,N>& m) { S s("0-Dimensional Quad"); s[0] += N; return s; }
template<class T, std::size_t N, class S> inline pure S concept_name(const pent<T,N>& m) { S s("0-Dimensional Pentagon"); s[0] += N; return s; }
template<class T, std::size_t N, class S> inline pure S typenameof(const tri<T,N>& a) { S s("tri<T,N>"); return s; }
template<class T, std::size_t N, class S> inline pure S typenameof(const quad<T,N>& a) { S s("quad<T,N>"); return s; }
template<class T, std::size_t N, class S> inline pure S typenameof(const pent<T,N>& a) { S s("pent<T,N>"); return s; }
template<class T> using tri2  = tri <T,2>;
template<class T> using tri3  = tri <T,3>;
template<class T> using tri4  = tri <T,4>;
template<class T> using quad2 = quad<T,2>;
template<class T> using quad3 = quad<T,3>;
template<class T> using quad4 = quad<T,4>;
template<class T> using pent2 = pent<T,2>;
template<class T> using pent3 = pent<T,3>;
template<class T> using pent4 = pent<T,4>;
#endif

template<class T> inline pure vec<T,3> get_normal(const vec<T,3>& a) { return normalize(cross(a[1] - a[0],
                                                                                              a[2] - a[1])); }

typedef polyn<float,2,3> tri2f;
typedef polyn<float,3,3> tri3f;
typedef polyn<float,4,3> tri4f;
typedef polyn<float,2,4> quad2f;
typedef polyn<float,3,4> quad3f;
typedef polyn<float,4,4> quad4f;
typedef polyn<float,2,5> pent2f;
typedef polyn<float,3,5> pent3f;
typedef polyn<float,4,5> pent4f;
typedef polyn<double,2,3> tri2d;
typedef polyn<double,3,3> tri3d;
typedef polyn<double,4,3> tri4d;
typedef polyn<double,2,4> quad2d;
typedef polyn<double,3,4> quad3d;
typedef polyn<double,4,4> quad4d;
typedef polyn<double,2,5> pent2d;
typedef polyn<double,3,5> pent3d;
typedef polyn<double,4,5> pent4d;
typedef polyn<long double,2,3> tri2l;
typedef polyn<long double,3,3> tri3l;
typedef polyn<long double,4,3> tri4l;
typedef polyn<long double,2,4> quad2l;
typedef polyn<long double,3,4> quad3l;
typedef polyn<long double,4,4> quad4l;
typedef polyn<long double,2,5> pent2l;
typedef polyn<long double,3,5> pent3l;
typedef polyn<long double,4,5> pent4l;
