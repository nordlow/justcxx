/*!
 * \file gcd.hpp
 * \brief Greatest Common Divisor (GCD)
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/*! Greatest Common Divisor (GCD)
 */
template<typename T>
T gcd(const T & m, const T & n)
{
    T r;
    while ((r = m % n) != 0)
    {
        m = n;
        n = r;
    }
    return n;
}
