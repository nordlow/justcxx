/*! \file parallel_sum.hpp
 * \brief Parallel Sum.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <algorithm>
#include <numeric>
#include <future>

/*! Parallel Sum.
 * \tparam RAI Random Access Iterator.
 * \see http://en.cppreference.com/w/cpp/thread/async
 */
template<class RAI> int parallel_sum(RAI beg, RAI end)
{
    typename RAI::difference_type len = end-beg;
    if (len < 1000) {            // for small sizes
        return std::accumulate(beg, end, 0); // revert to ordinary algorithm
    }
    auto mid = beg + len/2;     // middle value
    auto handle = std::async(std::launch::async, parallel_sum<RAI>, mid, end);
    int sum = parallel_sum(beg, mid);
    return sum + handle.get();
}
