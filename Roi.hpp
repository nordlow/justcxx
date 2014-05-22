/*!
 * \file Roi.hpp
 * \brief Region of Interest (Roi)
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "FixQ.hpp"

// ============================================================================

/*!
 * Region Of Interest
 */
template<class T = ushort, std::size_t N = 1>
class Roi
{
public:
    // construct

    Roi() {}                   // \todo REMove?

    Roi(FixQ<T, N> i, FixQ<T, N> j, FixQ<T, N> m, FixQ<T, N> n)
	: i(i), j(j), m(m), n(n) {}

    /// Read internal area, that is the area in sub-pixel square units.
    uint get_internal_area() const { return (m.get_internal() *
						     n.get_internal()); }

    /// Return true if region cannot be divided any further.
    bool is_atomic() const { return (m.is_epsilon() and n.is_epsilon()); }

    /// Return true if region is empty.
    bool is_empty() const { return (m == (T)0 and n == (T)0); }

    /// Return true if region represents exactly one whole pixel.
    bool is_1x1() const { return (m.is_1() and n.is_1()); }

    /// Return true if region is fractional.
    bool is_frac() const { return (m < (T)1 or n < (T)1); }

    /// Return true if region is rational.
    bool is_rational() const
    {
	return (i.is_rational() or
		j.is_rational() or
		m.is_rational() or
		n.is_rational());
    }

    /***
     * Split this roi into sub rois along the row (m) dimension (dimension 0).
     *
     * \return the row (position) at which the split occurred.
     */
    FixQ<T, N> row_split_to(Roi & a, Roi & b) const
    {
	FixQ<T, N> m0;
        if (m > (T)1) { m0 = (uint)m / 2; } // Normal.
        else             { m0 = m >> (uint)1; }	// Anti-alias.
        const FixQ<T, N> m1 = m - m0;

	const FixQ<T, N> i0 = i;
	const FixQ<T, N> i1 = i + m0;

        a = Roi(i0, j, m0, n);
        b = Roi(i1, j, m1, n);

	return m0;
    }

    FixQ<T, N> col_split_to(Roi & a, Roi & b) const
    {
	FixQ<T, N> n0;
        if (n > (T)1) { n0 = (uint)n / 2; } // Normal.
        else             { n0 = n >> (uint)1; }	// Anti-alias.
        const FixQ<T, N> n1 = n - n0;

	const FixQ<T, N> j0 = j;
	const FixQ<T, N> j1 = j + n0;

        a = Roi(i, j0, m, n0);
        b = Roi(i, j1, m, n1);

	return n0;
    }

    // io

    void print(std::ostream & os) const
    {
        os << "[ "
	   << "i=" << i
           << " j=" << j
           << " m=" << m
           << " n=" << n
	   << " ]";
    }

    friend std::ostream & operator << (std::ostream & os, const Roi & a)
    {
	a.print(os); return os;
    }

public:
    FixQ<T, N> i, j;	      ///< The start row i and column j of the region.
    FixQ<T, N> m, n;	///< The number of rows m and columns n of the region.
};

// ============================================================================

/*!
 * Region with a maximum dimension of 32768 x 32768 with 2x2 subpixel area.
 */
typedef Roi<ushort, 1> Roi_15_1;

/*!
 * Region with a maximum dimension of 16384 x 16384 with 4x4 subpixel area.
 */
typedef Roi<ushort, 2> Roi_14_2;

/*!
 * Region with a maximum dimension of 8192 x 8192 with 8x8 subpixel area.
 */
typedef Roi<ushort, 3> Roi_13_3;
