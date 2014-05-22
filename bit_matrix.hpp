/*! \file bit_matrix.hpp
 * \brief Matrix of bits (bool).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <iostream>
#include <vector>
#include "utils.hpp"
#include "algorithm_x.hpp"
#include "rand.hpp"
#include "hamming_distance.hpp"

/*!
 * Bit Matrix
 *
 * Matrix of bits (bool) similar to std::bit_vector.
 */
class bit_matrix
{
public:
    bit_matrix() : nrows(0), ncols(0), data(0) {}
    bit_matrix(size_t m, size_t n)
        : nrows(m), ncols(n), data(m*n) {}

    /// \name Dimension Getters.
    /// \{
    /// Get the number of rows.
    size_t n_rows() const { return nrows; }
    /// Get the number of columns.
    size_t n_cols() const { return ncols; }
    /// Resize matrix to m rows and n columns.
    void resize(size_t m, size_t n)
    {
        nrows = m;
        ncols = n;
        data.resize(m * n);
    }
    // \}

    /// \name Predicates.
    /// \{
    /// Return true if matrix is a square matrix.
    bool is_square() const { return nrows == ncols; }
    /// Return true if matrix is an adjacency matrix.
    bool is_adjacency() const;
    /// Return true if matrix is a roundtrip matrix.
    bool is_roundtrip() const { return false; }
    // \}

    /// \name Element Accessors.
    /// \{
    /// Set the element at row \p i, column \p j (to one).
    void set_bit(size_t i, size_t j)
    {
        const unsigned ij = i * ncols + j;
        DEBUG_CHECK_RANGE(ij, data.size());
        data[ij] = 1;
    }
    /// Set the element at row \p i, column \p j to the value of \p a.
    void put_bit(size_t i, size_t j, bool a)
    {
        const unsigned ij = i * ncols + j;
        DEBUG_CHECK_RANGE(ij, data.size());
        data[ij] = a;
    }
    /// Clear the element at row \p i, column \p j (to zero).
    void clr_bit(size_t i, size_t j)
    {
        const unsigned ij = i * ncols + j;
        DEBUG_CHECK_RANGE(ij, data.size());
        data[ij] = 0;
    }
    /// Get the element at row \p i, column \p j.
    bool get_bit(size_t i, size_t j) const
    {
        const unsigned ij = i * ncols + j;
        DEBUG_CHECK_RANGE(ij, data.size());
        return data[ij];
    }
    /// Get the element at index \p i (row-major order).
    bool operator () (size_t i) const
    {
        DEBUG_CHECK_RANGE(i, data.size());
        return data[i];
    }
    bool operator () (size_t i, size_t j) const
    {
        return get_bit(i, j);
    }
    /// Get the \p i:th row.
    std::vector<bool> row(size_t i) const
    {
        const size_t n = n_cols();
        std::vector<bool> a(n);
        for (size_t j = 0; j < n; j++)
        {
            a[i] = get_bit(i, j);
        }
        return a;
    }
    /// Get the \p j:th column.
    std::vector<bool> col(size_t j) const
    {
        const size_t n = n_rows();
        std::vector<bool> a(n);
        for (size_t i = 0; i < n; i++)
        {
            a[i] = get_bit(i, j);
        }
        return a;
    }
    // \}

    /// \name Generators.
    /// \{
    /// Zero all elements.
    void zero();
    /// Assign all elements a random value.
    void rand() {
        // TODO: Reuse rand(bit_vector)
        for (size_t i = 0; i < data.size(); i++) {
            data[i] = pnw::get_random_bit();
        }
    }
    /// Assign n random elements a value of one.
    void randomly_set_n_ones(size_t n)
    {
        for (size_t i = 0; i < n; i++)
        {
            data[rand() % (data.size())] = 1;
        }
    }
    /// Construct a random adjacency matrix.
    void random_adjacency();
    void bidirect();
    // \}

    /// \name Bitwise operations.
    /// \{
    friend void bit_or(bit_matrix & a,
                       const bit_matrix & b,
                       const bit_matrix & c);
    friend void bit_and(bit_matrix & a,
                        const bit_matrix & b,
                        const bit_matrix & c);
    friend void bit_xor(bit_matrix & a,
                        const bit_matrix & b,
                        const bit_matrix & c);
    /***
     * Return the hamming distance between the arguments \p a and \p b.
     *
     * Result is undefined if the sizes of \p a and \p b are not equal.
     */
    friend size_t hamming_distance(const bit_matrix & a,
                                   const bit_matrix & b)
    {
        return pnw::hamming_distance(a.data, b.data);
    }
    // \}

    /// \name IO.
    /// \{
    void print(std::ostream & os) const
    {
        const bit_matrix & mat = *this;
        os << "[ ";
        for (size_t i = 0; i < n_rows(); i++)
        {
            for (size_t j = 0; j < n_cols(); j++)
            {
                os << mat(i,j) << ' ';
            }
            if (i != n_rows()-1)
            {
                os << "; ";
            }
        }
        os << "]";
    }
    friend std::ostream & operator << (std::ostream & os,
                                       const bit_matrix & in)
    {
        in.print(os);
        return os;
    }
    // \}

protected:
    size_t nrows;		/// The number of rows of the matrix.
    size_t ncols;		/// The number of columns of the matrix.
    std::vector<bool> data;	///< The matrix data.
};
