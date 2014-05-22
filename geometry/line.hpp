/*! \file line.hpp
 * \brief N-dimensional Line.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <utility>
#include "vec.hpp"
#include "box.hpp"
#include "../mean.hpp"

/*! N-Dimensional Line. */
template<class T, std::size_t N>
class line : public std::pair<vec<T,N>,
                              vec<T,N> >
{
public:
    /// \name Construct.
    /// \{
    line() : line(0, 1) {}        ///< Construct Default Unit Line
    line(vec<T,N> p_,
         vec<T,N> q_) : std::pair<vec<T,N>,vec<T,N> >(p_, q_) {}
    /// \}

    /// Dimensionality.
    pure size_t dimnum() const { return N; }
    pure size_t size() const { return dimnum(); }

    /// \name Element Access.
    /// \{
    vec<T,N>  operator() (size_t i) const { return (&std::get<0>(*this))[i]; }
    vec<T,N>& operator() (size_t i)       { return (&std::get<0>(*this))[i]; }
    vec<T,N>  operator[] (size_t i) const { return (&std::get<0>(*this))[i]; }
    vec<T,N>& operator[] (size_t i)       { return (&std::get<0>(*this))[i]; }
    vec<T,N>  p() const { return std::get<0>(*this); } ///< Lower Bound.
    vec<T,N>& p()       { return std::get<0>(*this); } ///< Lower Bound.
    vec<T,N>  q() const { return std::get<1>(*this); } ///< Upper Bound.
    vec<T,N>& q()       { return std::get<1>(*this); } ///< Upper Bound.
    T  p(size_t i) const { return p()[i]; } ///< Lower Bound.
    T& p(size_t i)       { return p()[i]; } ///< Lower Bound.
    T  q(size_t i) const { return q()[i]; } ///< Upper Bound.
    T& q(size_t i)       { return q()[i]; } ///< Upper Bound.
    /// \}

    pure vec<T,N> mid() const { return pnw::mean_element(*this); }
    pure box<T,N> inner_box() const { return box<T,N>(min(p(), q()),
                                                 max(p(), q())); }
    box<T,N> outer_box() const { return inner_box(); }

    pure T distance_to_infinite_line(vec<T,N> pos) const { return norm(get_orthogonal_part(pos - p(),
                                                                                           q() - p())); }
    pure T middle_distance_to(vec<T,N> pos) const { return dist(mid(), pos); }

    template<class S> friend pure S concept_name(const line& m) { S s("0-Dimensional Line"); s[0] += N; return s; }
    template<class S> friend pure S typenameof(const line& a) { S s("line<T,N>"); return s; }

    friend std::ostream& show        (std::ostream& os, const line& a) { os << "(s" << MEMCH << a(0) << " ⇒ d" << MEMCH << a(1) << ")"; return os; }
    friend std::ostream& operator << (std::ostream& os, const line& a) { return show(os, a); }
};

#if HAVE_CXX11_TEMPLATE_ALIASES
template<class T> using line2 = line<T,2>;
template<class T> using line3 = line<T,3>;
template<class T> using line4 = line<T,4>;
typedef line2<float> line2f;
typedef line3<float> line3f;
typedef line4<float> line4f;
typedef line2<double> line2d;
typedef line3<double> line3d;
typedef line4<double> line4d;
typedef line2<long double> line2l;
typedef line3<long double> line3l;
typedef line4<long double> line4l;
#else
typedef line<float,2> line2f;
typedef line<float,3> line3f;
typedef line<float,4> line4f;
typedef line<double,2> line2d;
typedef line<double,3> line3d;
typedef line<double,4> line4d;
typedef line<long double,2> line2l;
typedef line<long double,3> line3l;
typedef line<long double,4> line4l;
#endif
