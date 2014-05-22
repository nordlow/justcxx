/*!
 * \file opt.hpp
 * \brief Optimization Algorithms.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-25 00:12
 * \see https://hbfs.wordpress.com/2012/04/24/introduction-to-gradient-descent/
 */

#pragma once

#include "nelder_mead.hpp"

template<class T>
void gradient_descent(const std::vector<T> & y, T & a, T & b)
{
    const size_t n = y.size();
    const T sx = sum_of_int(n-1);
    const T sx2 = sum_of_sqr(n-1);

    T sy=0;
    T sxy=0;

    for (size_t i=0;i<n;i++) {
        sy+=y[i];
        sxy+=i*y[i];
    }

    // initial guesses
    b = 0;
    a = sy/n;

    // iterations
    T last_a,last_b;
    T sa = 1e-3;                // for n=100
    T sb = 1e-6;                // for n=100

    do {
        last_a=a;
        last_b=b;
        T db= -(sxy - a*sx - b*sx2);
        T da= -(sy - n*a - b*sx );

        a-=sa*da;
        b-=sb*db;
    }
    while ((std::abs(a-last_a)/a > 0.0001) or
           (std::abs(b-last_b)/b > 0.0001));
}
