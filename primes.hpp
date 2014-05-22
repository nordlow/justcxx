/*!
 * \file prime.hpp
 * \brief Check whether Integer is a Prime Number.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-12-03 13:31
 */

#pragma once

/*! Return \c true if \p n is \p a prime, \c false otherwise.
 * \complexity[time] O(n)
 */
template<typename T>
inline pure bool is_prime(const T & n)
{
    for (T j = 3; j <= ceil(sqrt(n)); j += 2) {
        if (n % j == 0) {
            return false;
        }
    }
    return true;
}
