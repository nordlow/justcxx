/*! \file sigmoids.hpp
 * \brief Sigmoid Functions often used in Artificial Neural Networks (ANN).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <cmath>

/*! The Logistic Function.
 * \see https://en.wikipedia.org/wiki/Logistic_function
 */
template<class T>
inline T logistic_function(T a)
{
    return 1 / (1 + std::exp(-a));
}
