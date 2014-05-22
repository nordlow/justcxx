/*! \file conn_matrix.hpp
 * \brief Connectivity (Square) Matrix (of Bits/Booleans).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "bit_matrix.hpp"
#include "algorithm_x.hpp"

/*! Connectivity Matrix
 */
class conn_matrix : public bit_matrix {
public:
    /// Construct empty.
    conn_matrix() : bit_matrix(0, 0) {}

    /// Construct with m rows and m columns.
    conn_matrix(size_t m) : bit_matrix(m, m) {}

    /// Resize matrix to m rows and m columns.
    void resize(size_t m)
    {
	nrows = m;
	ncols = m;
	data.resize(m * m);
    }

    /// Return true if matrix is a square matrix.
    bool is_square() const { return true; }
};
