/*! \file intersect.hpp
 * \brief Intersections between lines and between boxes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Reuse Boost.Geomtry http://www.boost.org/doc/libs/1_48_0/libs/geometry/doc/html/geometry/reference.html
 * #if HAVE_BOOST_GEOMETRY
 * #  include <boost/geometry/algorithms/intersection.hpp>
 * #endif
 */

#pragma once
#include <algorithm>
#include "vec.hpp"
//#include "mat.hpp"
#include "box.hpp"
#include "line.hpp"

/*! Intersection Indexes \p i between Two Lines.
 * The 1:st and 2:nd line are defined by
 * - a + c * i(0)
 * - b + d * i(1)
 * respectively.
 *
 * Inputs:
 * - a, b: The start POSITIONS of the lines parameterizations.
 * - c, d: The DIRECTIONS of the lines parameterizations.
 *
 * \return the parameter values of the lines at the intersection point.
 */
template<class T, std::size_t N>
inline pure vec<T,N> line_intersect(const vec<T,N>& a, const vec<T,N>& b,
                                    const vec<T,N>& c, const vec<T,N>& d){
    return inv(colwise_mat(c, -d)) * (b - a);
}

/*! See above */
template<class T, std::size_t N>
inline pure vec<T,N> intersect(const line<T,N>&  p,
                               const line<T,N>&  q) {
    return line_intersect(p(0), q(0), p(1) - p(0), q(1) - q(0));
}

/*! Get Intersection Point using the Value Intersection Parameter \p i returned from \c line_intersect().
 * The line is defined by: \p a + \p b * \p i.
 * \param[in] a Start Position of the line parameterization.
 * \param[in] b Direction of the line parameterizations.
 */
template<class T, std::size_t N>
inline pure vec<T,N> line_intersection_point(const vec<T,N>& a,
                                             const vec<T,N>& b,
                                             T i) {
    return a + b * i;
}

/*! See above */
template<class T, std::size_t N>
inline pure vec<T,N> intersection_point(const line<T,N>&  p,
                                        T i) {
    const auto& a = p(0);
    const auto& c = p(1) - p(0);
    return line_intersection_point(a, c, i);
}

/*! Intersection of \p a and \p b. */
template<class T, std::size_t N>
inline pure box<T,N> intersect(const box<T,N>&  a,
                               const box<T,N>&  b) {
    auto l = max(a.l(), b.l());
    auto h = min(a.h(), b.h());
    return std::all_of(l < h) ? box<T,N>(l, h) : box<T,N>().undefine();
}
