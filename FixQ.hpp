/*!
 * \file FixQ.hpp
 * \brief Fixed precision rational number.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <iostream>

// ============================================================================

/*!
 * Fixed (Fix) precision rational (Q) positive number.
 *
 * Used as region (of interest) component (RoiC) in anti-aliased rendering.
 *
 * Simulates rational positive numbers of fixed precision where denominator
 * DENOM is limited to powers of two.
 *
 * Interpreted as a rational number [val / DENOM]
 *
 * z + q, q = n / d, where
 * n is numerator and d is denominator (DENOM)
 *
 * Used in various anti-aliasing calculations when rendering 2d/3d
 * vector-based objects.
 *
 * The template argument T represents the precision. 16-bits should suffice
 * for most frame buffer applications today and in the future.
 *
 * The template argument N represents the number of fractional bits which
 * of course must be <= sizeof(T) * 8.
 */
template<class T = ushort, std::size_t N = 1>
class FixQ
{
public:
    /// The number of bits used as the rational part.
    static const uint SHIFT = N;

    /// The rational factor (denominator).
    static const uint DENOM = (1 << SHIFT);

    /// The factor squared.
    static const uint SQR_DENOM = (1 << (2 * SHIFT));

    /// Mask used to filter out the pure fractional part.
    static const uint MASK = (DENOM - 1);

public:
    FixQ() {}

    /// NOTE: This is inlined efficiently, at least on gcc-3.
    FixQ(uint z,
	 uint p = 0,
	 uint q = DENOM)
    {
	set_z_p_q(z, p, q);
    }

private:
    /// z is the integer part, p / q (p < q, q > 0) is the fractional part.
    void set_z_p_q(uint z,
		   uint p = 0,
		   uint q = DENOM)
    {
	val = ((z << SHIFT) bitor
	       (p * (DENOM / q) bitand MASK));
    }

    void set_uint(uint int_part) { val = int_part << SHIFT; }

public:
    /// Get the integer part (val / DENOM).
    uint get_integer_part() const { return (val >> SHIFT); }

    /// Return the fractional part (multiplied by DENOM).
    uint get_frac_part() const { return (val bitand MASK); }

    /// Return the internal value.
    uint get_internal() const { return val; }

    /// Return true if this has a rational part > 0.
    bool is_rational() const { return get_frac_part() > 0; }

    /// Return half.
    FixQ get_half() const { return (T)(val >> 1); }

    /// Cast to an uint by truncating away the fractional part.
    operator uint() const
    {
#ifndef NDEBUG
	if (is_rational())
	{
 	    PERR("Casting should not be used in this way!\n");
	}
#endif
	return get_integer_part();
    }

    operator float() const { return static_cast<float>(val) / SHIFT; }
    operator double() const { return static_cast<double>(val) / SHIFT; }

    /// Return true if this has the value of 1.
    uint is_1() const { return (val == (1 << SHIFT)); }

    // pointwise binary arithmetic and assignment

    void operator += (FixQ a) { val += a.val; }
    void operator -= (FixQ a) { val -= a.val; }

    // pointwise binary arithmetic

    friend FixQ operator + (FixQ a, FixQ b) { return (T)(a.val + b.val); }
    friend FixQ operator - (FixQ a, FixQ b) { return (T)(a.val - b.val); }

    // pointwise binary comparison

    friend bool operator == (FixQ a, FixQ b) { return a.val == b.val; }
    friend bool operator != (FixQ a, FixQ b) { return ! (a == b); }
    friend bool operator <  (FixQ a, FixQ b) { return a.val <  b.val; }
    friend bool operator <= (FixQ a, FixQ b) { return a.val <= b.val; }
    friend bool operator >  (FixQ a, FixQ b) { return a.val >  b.val; }
    friend bool operator >= (FixQ a, FixQ b) { return a.val >= b.val; }

    friend bool operator == (FixQ a, T b) { return a.val == (b << SHIFT); }
    friend bool operator != (FixQ a, T b) { return ! (a == b); }
    friend bool operator <  (FixQ a, T b) { return a.val <  (b << SHIFT); }
    friend bool operator <= (FixQ a, T b) { return a.val <= (b << SHIFT); }
    friend bool operator >  (FixQ a, T b) { return a.val >  (b << SHIFT); }
    friend bool operator >= (FixQ a, T b) { return a.val >= (b << SHIFT); }

    // bitwise shift

    /// Shift a left b bits.
    friend FixQ operator << (FixQ a, uint b)
    {
	return(T)(a.val << b);
    }

    /// Shift a right b bits.
    friend FixQ operator >> (FixQ a, uint b)
    {
	return(T)(a.val >> b);
    }

    /// Return epsilon - the smallest possible value larger than zero.
    static FixQ get_epsilon()
    {
	FixQ a;
	a.val = 1;
	return a;
    }

    /***
     * Return true if this is equal to epsilon, the smallest possible value
     * larger than zero.
     */
    bool is_epsilon() const { return *this == get_epsilon(); }

    /***
     * Print the fractional number as mixe format.
     *
     * For example if N is 2: [a+b/4].
     */
    void print(std::ostream& os) const
    {
        os << "[" << get_integer_part()
           << "+" << get_frac_part()
           << "/" << DENOM
	   << "]";
    }

    friend std::ostream& operator << (std::ostream& os, const FixQ a)
    {
	a.print(os); return os;
    }

private:
    FixQ(T val) : val(val) {} // used by arithmetic operators

    T val;
};
