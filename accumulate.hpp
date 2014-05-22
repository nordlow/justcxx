/*! \file accumulate.hpp
 * \brief Accumulate.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-22 13:01
 */

#pragma once
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/kurtosis.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/weighted_kurtosis.hpp>
#include <algorithm>

// Easier.
namespace accmulate {

/// @name Kurtosis.
/// \{
template<class C> typename C::value_type kurtosis(typename C::const_iterator begin,
                                                   typename C::const_iterator end) {
    using namespace boost::accumulators;
    typedef typename C::value_type E;
    accumulator_set<E, stats<tag::kurtosis> > acc;
    std::for_each(begin, end, [&acc](const E & elt) { acc(elt); });
    return kurtosis(acc);
}
template<class C> typename C::value_type kurtosis(C & c) { return kurtosis(begin(c), end(c)); }
/// \}

/// @name Weighted Kurtosis.
/// \{
template<class C> typename C::value_type weighted_kurtosis(typename C::const_iterator begin,
                                                            typename C::const_iterator end) {
    using namespace boost::accumulators;
    typedef typename C::value_type E; // element
    accumulator_set<E, stats<tag::weighted_kurtosis> > acc;
    std::for_each(begin, end, [&acc](const E & elt) { acc(elt); });
    return weighted_kurtosis(acc);
}
template<class C> typename C::value_type weighted_kurtosis(C & c) { return weighted_kurtosis(begin(c), end(c)); }
/// \}

}
