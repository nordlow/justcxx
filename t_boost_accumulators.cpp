/*!
 * \file t_boost_accumulators.cpp
 * \brief Test Boost.Accumulators.
 */

#include <iostream>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/kurtosis.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/weighted_kurtosis.hpp>

#include "accumulate.hpp"

// \todo Make this more functional to use.
int main(int argc, const char * argv[], const char * envp[])
{
    typedef double T;
    using namespace boost::accumulators;
    accumulator_set<T, stats<tag::min, tag::max, tag::median, tag::mean,
                             tag::moment<2>, tag::kurtosis, tag::weighted_kurtosis
                             > > acc;
    acc(1.2);
    acc(2.3);
    acc(3.4);
    acc(4.5);
    std::cout << "min:   " << min(acc) << std::endl;
    std::cout << "max:   " << max(acc) << std::endl;
    std::cout << "median:   " << median(acc) << std::endl;
    std::cout << "mean:   " << mean(acc) << std::endl;
    std::cout << "moment: " << moment<2>(acc) << std::endl;
    std::cout << "kurtosis: " << kurtosis(acc) << std::endl;
    std::cout << "weighted_kurtosis: " << weighted_kurtosis(acc) << std::endl;
    return 0;
}
