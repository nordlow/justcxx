/*! \file poly.hpp
 * \brief N-Dimensional Polygon.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <vector>
#include <ostream>
#include <boost/serialization/access.hpp>
#include "vec.hpp"
#include "outer_box.hpp"

/*! N-Dimensional Polygon. */
template<class T, std::size_t N>
class poly : public std::vector<vec<T,N> >
{
public:
    poly(std::size_t size_ = 0) : std::vector<vec<T,N> >(size_) {}
    poly(std::size_t size_, T a) : std::vector<vec<T,N> >(size_, a) {}
#if HAVE_CXX11_INITIALIZER_LISTS
    // \see http://stackoverflow.com/questions/5438671/static-assert-on-initializer-listsize
    poly(const std::initializer_list<vec<T,N> >& list) : poly(list.size()) { std::copy(begin(list), end(list), begin(*this)); }
#endif

    size_t dimnum() const { return N; }

    /*! Get Center. */
    vec<T,N> cen() const { return pnw::mean_element(*this); }
    /*! Get Center \p i. */
    T cen(size_t i) const { return cen()[i]; }

    box<T,N> outer_box() const { return ::outer_box(*this); }

    void fit_in(const box<T,N>& a) {
        const auto ext = outer_box();
        for (auto i = begin(*this); i != end(*this); i++) {
            const auto& op = *i;	// old position
            const auto np = (op - ext.l()) / ext.dim() * a.dim() + a.l(); // new position
            *i = np;
        }
    }

    void shift_pos(const vec<T,N>& a) { for (auto i = begin(*this); i != end(*this); i++) { *i += a; } } // TODO: for_each

    template<class S> friend pure S concept_name(poly m) { S s("0-Triangle Polygon"); s[0] += N; return s; }
    template<class S> friend pure S typenameof(poly a) { S s("poly<T,N>"); return s; }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & *(reinterpret_cast<std::vector<vec<T,N> >*>(this)) ; }
};

template<class T> using poly2 = poly<T,2>;
template<class T> using poly3 = poly<T,3>;

#if 0

/*! Read Polygon Side Midpoints.
 * \param[out] mids contains space for poly.n points.
 */
template<class T, std::size_t N>
inline pure vec<T,N> rdSideMids(const poly<T,N>& poly)
{
    vec<T,N> mids = 0;
    for (size_t i = 0; i < poly.n; i++) {	/* for each vertex */
        size_t j = i + 1;
        if (j >= poly.n) { j = 0; } /* wrap-around */
        mean(&poly.v[i], &poly.v[j], &mids[i]);
    }
    return mids;
}

/*! Relative Affine Transform.
 * Upon changing \p extP to \p extN change the \p src to @p
 * dst accordingly so that the \p src relative size position relative to
 * \p extP is preserved.
 */
template<class T, std::size_t N>
inline pure poly<T,N> relaffine(const box<T,N>& extP,
                                 const box<T,N>& extN,
                                 const poly<T,N>& src)
{
    poly<T,N> dst;
    if (src.n != dst.n) {
        leprintf("src's vertex count differs from dst's\n"); return;
    }
    for (size_t i = 0; i < src.n; i++) { /* for each vertex */
        dst.v[i] = relaffine(extP, extN, &src.v[i]);
    }
    return dst;
}

/*! Linearly Interpolate from \p src to \p dst at progress \p prg with transition \p trans. */
template<class T, std::size_t N>
inline pure poly<T,N>
linear_interpolate(const poly<T,N>& src, float prg,
                   const poly<T,N>& dst)
{
    poly<T,N> curr;
    if (src.n != dst.n) { PWARN("src.n not equal to dst.n\n"); return; }
    if (src == dst) {
        poly<T,N>_set_copy(curr, src);
        return;
    }
    if (src.n != curr.n) {
        poly<T,N>_clear(curr);
        poly<T,N>_init(curr, src.v, src.n); /* init to source */
    }
    PTEST("\n");
    for (size_t i = 0; i < src.n; i++) {	/* for each vertex */
        linear_interpolate(&src.v[i], prg, &dst.v[i], &curr.v[i]);
    }
}

/*! Fit Polygon Poly precisely Inside (Bounding) Box extents \p ext. */
inline pure poly<T,N> fit_in(const box<T,N>& ext)
{
    poly<T,N> poly;
    box<T,N> old_ext;
    if (poly<T,N>_rdBnd_box<T,N>(poly, &old_ext)) {
        poly<T,N>_relaffine(&old_ext, ext, poly, poly);
    }
    return poly;
}

#endif
