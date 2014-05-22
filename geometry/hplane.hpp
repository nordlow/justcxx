/*! \file hplane.hpp
 * \brief N-Dimensional Half-Plane.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <utility>
#include <boost/serialization/access.hpp>
#include "vec.hpp"

/*! N-Dimensional Half-Plane.
 * Implicitly defined by all the points that fullfil:
 * - dot(n(), a) <= d()
 * - where \c a is an arbitrary point in the plane.
 */
template<class T, std::size_t N>
class hplane : public std::pair<vec<T,N>, T>
{
public:
    hplane(const vec<T,N>& n_ = 1, T d_ = 0) : std::pair<vec<T,N>, T>((normalize(n_)), d_) {}

    vec<T,N>  n() const { return std::get<0>(*this); } ///< Normal.
    vec<T,N>& n()       { return std::get<0>(*this); } ///< Normal.
    T  d() const { return std::get<1>(*this); }        ///< Displacement.
    T& d()       { return std::get<1>(*this); }        ///< Displacement.

    pure bool includes(const vec<T,N>& a) const { return dot(n(), a) <= d(); }

    template<class S> friend pure S concept_name(const hplane& m) { S s("0-Dimensional Half-Plane"); s[0] += N; return s; }
    template<class S> friend pure S typenameof(const hplane& a) { S s("hplane<T,N>"); return s; }

    friend std::ostream& show        (std::ostream& os, const hplane& a) { os << "(n" << MEMCH << a.n() << ",d" << MEMCH << a.d() << ")"; return os; }
    friend std::ostream& operator << (std::ostream& os, const hplane& a) { return show(os, a); }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & n() & d(); }
};

#if HAVE_CXX11_TEMPLATE_ALIASES
template<class T> using hplane2 = hplane<T,2>;
template<class T> using hplane3 = hplane<T,3>;
template<class T> using hplane4 = hplane<T,4>;
typedef hplane2<float> hplane2f;
typedef hplane3<float> hplane3f;
typedef hplane4<float> hplane4f;
typedef hplane2<double> hplane2d;
typedef hplane3<double> hplane3d;
typedef hplane4<double> hplane4d;
typedef hplane2<long double> hplane2l;
typedef hplane3<long double> hplane3l;
typedef hplane4<long double> hplane4l;
#else
typedef hplane<float,2> hplane2f;
typedef hplane<float,3> hplane3f;
typedef hplane<float,4> hplane4f;
typedef hplane<double,2> hplane2d;
typedef hplane<double,3> hplane3d;
typedef hplane<double,4> hplane4d;
typedef hplane<long double,2> hplane2l;
typedef hplane<long double,3> hplane3l;
typedef hplane<long double,4> hplane4l;
#endif
