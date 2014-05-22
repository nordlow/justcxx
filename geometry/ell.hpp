/*! \file ell.hpp
 * \brief N-Dimensional Ellipse.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <boost/serialization/access.hpp>
#include "sphere.hpp"

/*! N-Dimensional Ellipse. */
template<class T, std::size_t N>
class ell : public box<T,N>
{
public:
    ell(const vec<T,N>& c, T r) : sphere<T,N>(c, r) {}
    box<T,N> inner_box() const { return this->scale(M_SQRT2_2); }

    bool includes(T dirang, const vec<T,N>& b, bool lin_flag) {
        auto cen_ = this->cen();
        auto rad_ = this->rad();
        auto ir = 1/rad_;        /* inverses */
        auto pos = b - cen_;
        if (dirang != 0.0f) { vec<T,N>array_rot(&pos, 1, NULL, dirang); } /* rotate */
        if (lin_flag) {
            return l1norm(pos*ir) < 1;
        } else {
            return sqrnorm(pos*ir) < 1;
        }
    }

    template<class S> friend pure S concept_name(const ell& m) { S s("0-Dimensional Ellipse"); s[0] += N; return s; }
    template<class S> friend pure S typenameof(const ell& a) { S s("ell<T,N>"); return s; }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) { ar & this->l() & this->h(); }
};

template<class T> using ell2 = ell<T,2>;
template<class T> using ell3 = ell<T,3>;
