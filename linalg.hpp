/*!
 * \file linalg.hpp
 * \brief Various Linear Algebra.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/*! Return the part of \p a that is parallel to \p b. */
template<typename T> T get_parallel_part(const T & a, const T & b) {
    return b * (dot(a, b) / sqrnorm(b));
}

/*!
 * Return the part of \p a that is orthogonal to \p b.
 */
template<typename T> T get_orthogonal_part(const T & a, const T & b) {
    return a - get_parallel_part(a, b);
}
