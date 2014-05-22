/*! \file show_histogram.hpp
 * \brief Show pnw::histograms.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-06-29 01:15
 * \todo Default Plotting to PGF-Histogram in fshow_histogram.hpp.
 */

#pragma once
#include "show.hpp"
#include "histogram.hpp"

namespace pnw { namespace histogram {

/*! Show Sparse Histogram Bins. */
template<class V,
         class C = size_t,
         class H = std::unordered_map<V,C>
         >
inline std::ostream& show_sparse(std::ostream& os,
                                 const H& h,
                                 const char * opening = "[",
                                 const char * closing = "]",
                                 const char * separator = " ") {
    os << opening << ' ';
    for (const auto& e : h) { // TODO: Provide std::for_each aswell.
        os << (std::is_integral<V>::value ? static_cast<size_t>(e.first) : e.first)
           << SEPCH
           << static_cast<size_t>(e.second)
           << separator;
    }
    os << closing;
    os << " : pnw::histogram::dense<V=" << typenameof<V>() << ",C=" << typenameof<C>() << ">";
    return os;
}

/*! Show Sparse Data Histogram Bins. */
template<class V,
         class C = size_t,
         class D = size_t,
         class H = std::unordered_map<V,std::pair<C,D> >
         >
inline std::ostream& show_d_sparse(std::ostream& os,
                                   const H& h,
                                   const char * opening = "[",
                                   const char * closing = "]",
                                   const char * separator = " ") {
    os << opening << ' ';
    for (const auto& e : h) { // TODO: Provide std::for_each aswell.
        os << (std::is_integral<V>::value ? static_cast<size_t>(e.first) : e.first)
           << SEPCH
           << static_cast<size_t>(e.second)
           << separator;
    }
    os << closing;
    os << " : pnw::histogram::dense<V=" << typenameof<V>() << ",C=" << typenameof<C>() << ",D=" << typenameof<D>() << ">";
    return os;
}

/*! Show Dense Histogram Bins. */
template<class V,
         class C = size_t,
         class H = typename std::vector<C>
         >
inline std::ostream& show_dense(std::ostream& os,
                                const H& h,
                                const char * opening = "[",
                                const char * closing = "]",
                                const char * separator = " ") {
    os << opening << ' ';
    size_t i = 0;
    for (const auto& e : h) { // TODO: Provide std::for_each aswell.
        if (e) {
            os << i
               << SEPCH
               << static_cast<size_t>(e)
               << separator;
        }
        ++i;
    }
    os << closing;
    os << " : pnw::histogram::sparse<V=" << typenameof<V>() << ",C=" << typenameof<C>() << ">";
    return os;
}

template<class V,
         class C = size_t,
         class H = std::unordered_map<V,C>
         >
std::ostream& operator << (std::ostream& os, const generic<V,C,H>& a) { return show_sparse<C,V,H>(os, a.get()); }

template<class V,
         class C = size_t,
         class D = size_t,
         class H = std::unordered_map<V,std::pair<C,D> >
         >
std::ostream& operator << (std::ostream& os, const d_generic<V,C,D,H>& a) { return show_d_sparse<C,D,V,H>(os, a.get()); }

template<class V,
         class C = size_t,
         class H = typename std::vector<C>
         >
std::ostream& operator << (std::ostream& os, const dense<V,C,H>& a) { return show_dense<V,C,H>(os, a.get()); }

template<class V,
         class C = size_t,
         class H = typename std::vector<C>
         >
std::ostream& operator << (std::ostream& os, const dense<C>& a) { return show_dense<V,C,H>(os, a.get()); }

}}
