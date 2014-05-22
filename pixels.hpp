/*!
 * \file pixels.hpp
 * \brief Pixels (Picture Elements)
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "algorithm_x.hpp"
#include "encode.hpp"
#include "decode.hpp"
#include "interpolate.hpp"

using pnw::bitmask;

// ============================================================================

extern "C"
{
#include "pixels.h"
}

// ============================================================================

/*!
 * Templatized alpha-blending function.
 *
 * Inputs:
 * - T a: Any red, green, blue-pixel
 * - Tq b: Any red, green, blue, alpha-pixel
 */
template<class T, class Tq>
inline pure T alpha_blend(const T& a, const Tq& b)
{
    return T(alpha_blend(a.red_8bit(), b.red_8bit(), b.alpha_8bit()),
             alpha_blend(a.green_8bit(), b.green_8bit(), b.alpha_8bit()),
             alpha_blend(a.blue_8bit(), b.blue_8bit(), b.alpha_8bit()));
}

// ============================================================================

/*! Pixel Grey 8-bit. */
class PixG8
{
public:
    PixG8() {}
    PixG8(uint8_t g) : pix(g) {}

    friend PixG8 saturated_add(PixG8 a, PixG8 b);
    friend PixG8 alpha_blend(PixG8 a, PixG8 b, PixG8 alpha);

    friend std::ostream& operator << (std::ostream& os, const PixG8 a) { return os << static_cast<uint>(a.pix); }
    operator uint() const { return pix; }
private:
    uint8_t pix;
};

inline pure PixG8 saturated_add(PixG8 a, PixG8 b)
{
    uint c = (static_cast<uint>(a) + static_cast<uint>(b));
    c = std::min<uint>(c, 255);
    return static_cast<PixG8>(c);
}

/*!
 * Perform an alpha-blending of the two color component values \p a and \p b using
 * the alpha (transparency) factor c onto d according to the formula
 *                d = (c * a + (256 - c) * b) / 256.
 *
 * The arguments \p a and \p b are normally
 * interpreted as the background and foreground color respectively.
 *
 * The alpha factor c measures the amount of transparency the foreground color
 * b should have when we "lay it over" the background color a.
 *
 * This means that when the alpha factor is set to
 * - the minimum value of 0: we will have no transparency (full opaqueness)
 *   and then return value will be equal to b.
 * - the maximum value of 255: we will have the maximum provided transparency
 *   and the return value will be equal to (255 * a + 1 * b) / 256.
 *
 * As we see there is no way to express a total transparency with alpha. That
 * would mean that the operation would return the background color a
 * which is not a meaningful calculation in applications where we want
 * to put colors on top of each other.
 * If we would want such a behaviour we could simply skip the call to this
 * function.
 *
 * Logic is designed for performance:
 * - The calculation of (256 - z) can be made very efficient in unsigned 8-bit
 *   integer arithmetic noting that 256 is equal 0 modulo 255 making the
 *   operation a simple negation (-z).
 * - Therefore we only need (all in uinteger arithmetic):
 *   - two 8-bit to 16-bit MULS,
 *   - one 8-bit SUB (or NEG),
 *   - one 16-bit ADD and
 *   - one 16-bit shift (into an 8-bit).
 */
inline pure PixG8 alpha_blend(PixG8 a, PixG8 b, PixG8 c)
{
    // cast to uint for peformance
    const uint x = static_cast<uint>(a);
    const uint y = static_cast<uint>(b);
    const uint z = static_cast<uint>(c);

    uint d = (z * x + (256 - z) * y) >> 8;

    d = std::min<uint>(d, 255);

    return static_cast<PixG8>(d);
}

/*! One-dimensional linear interpolation from \p a to \p b at \p i (from 0 to m). */
inline pure PixG8 linear_interpolate_1d(PixG8 a, PixG8 b,
                                                         const double i, const double m)
{
    return PixG8(static_cast<uint>
                 (linear_interpolate_1d_same(static_cast<double>(a),
                                   static_cast<double>(b),
                                   i, m)));
}

// ============================================================================

/*! Pixel Alpha-Blue-Green-Red 32-bit. */
class PixABGR32
{
public:
    PixABGR32() {}

    /***
     * Default alpha (transparency) to 0, that is no transparency.
     */
    PixABGR32(PixG8 r, PixG8 g, PixG8 b, PixG8 a = 0) { init(r, g, b, a); }
    PixABGR32(ulong c) : pix(c) {}
    PixABGR32(PixG8 g) { init(g, g, g); }

    void init(PixG8 r, PixG8 g, PixG8 b, PixG8 a = 0) {
        pix = (((static_cast<ulong>(r) bitand bitmask(8)) <<  0) bitor
               ((static_cast<ulong>(g) bitand bitmask(8)) <<  8) bitor
               ((static_cast<ulong>(b) bitand bitmask(8)) << 16) bitor
               ((static_cast<ulong>(a) bitand bitmask(8)) << 24));
    }

    PixG8 red_8bit  () const { return (pix >>  0) bitand bitmask(8); }
    PixG8 green_8bit() const { return (pix >>  8) bitand bitmask(8); }
    PixG8 blue_8bit () const { return (pix >> 16) bitand bitmask(8); }
    PixG8 alpha_8bit() const { return (pix >> 24) bitand bitmask(8); }

    friend PixABGR32 saturated_add(PixABGR32 a, PixABGR32 b);
    friend PixABGR32 operator + (PixABGR32 a, PixABGR32 b);
    PixABGR32 operator += (PixABGR32 a);

    void set_intensity(PixG8 a) {
        const uint mean = (static_cast<uint>(red_8bit()) +
                           static_cast<uint>(green_8bit()) +
                           static_cast<uint>(blue_8bit())) / 3;
        init(static_cast<uint>(red_8bit()) * a / mean,
             static_cast<uint>(green_8bit()) * a / mean,
             static_cast<uint>(blue_8bit()) * a / mean);
    }

    friend std::ostream& operator << (std::ostream& os, const PixABGR32 a) {
        return os << "[ "
                  << a.red_8bit() << ' '
                  << a.green_8bit() << ' '
                  << a.blue_8bit() << ' '
                  << a.alpha_8bit()
                  << " ]";
    }

    operator ulong() const { return pix; }

    friend void encode(std::ostream& os, PixABGR32 a) { encode(os, a.pix); }
    friend void decode(std::istream& is, PixABGR32& a) { decode(is, a.pix); }
private:
    uint32_t pix;
};

inline pure PixABGR32 saturated_add(PixABGR32 a, PixABGR32 b)
{
    return PixABGR32(saturated_add(a.red_8bit(), b.red_8bit()),
                     saturated_add(a.green_8bit(), b.green_8bit()),
                     saturated_add(a.blue_8bit(), b.blue_8bit()),
                     saturated_add(a.alpha_8bit(), b.alpha_8bit()));
}

inline pure PixABGR32 operator + (PixABGR32 a, PixABGR32 b)
{
    return alpha_blend(a, b);
}

inline PixABGR32 PixABGR32::operator += (PixABGR32 a)
{
    return *this = *this + a;
}

/*!
 * One-dimensional linear interpolation from \p a to \p b at \p i (from 0 to m).
 */
inline pure PixABGR32 linear_interpolate_1d(PixABGR32 a, PixABGR32 b,
                                                   const double i,
                                                   const double m)
{
    return PixABGR32(linear_interpolate_1d(a.red_8bit(), b.red_8bit(), i, m),
                     linear_interpolate_1d(a.green_8bit(), b.green_8bit(), i, m),
                     linear_interpolate_1d(a.blue_8bit(), b.blue_8bit(), i, m),
                     linear_interpolate_1d(a.alpha_8bit(), b.alpha_8bit(), i, m));
}

// ============================================================================

/*! Pixel Red Green Blue 15-bit. */
class PixRGB555
{
public:
    PixRGB555() {}
    PixRGB555(PixG8 r, PixG8 g, PixG8 b) { init(r, g, b); }
    PixRGB555(PixG8 g) { init(g, g, g); }
    PixRGB555(PixABGR32 c) {
        init(c.red_8bit(),
             c.green_8bit(),
             c.blue_8bit());
    }

    void init(PixG8 r, PixG8 g, PixG8 b) {
        pix = (((static_cast<uint>(b) >> 3) <<  0) bitor
               ((static_cast<uint>(g) >> 3) <<  5) bitor
               ((static_cast<uint>(r) >> 3) << 10));
    }

    PixG8 red_5bit  () const { return (pix >> 10) bitand bitmask(5); }
    PixG8 green_5bit() const { return (pix >>  5) bitand bitmask(5); }
    PixG8 blue_5bit () const { return (pix >>  0) bitand bitmask(5); }

    friend std::ostream& operator << (std::ostream& os, const PixRGB555 a) {
        return os << "[ "
                  << a.red_5bit() << ' '
                  << a.green_5bit() << ' '
                  << a.blue_5bit()
                  << " ]";
    }

    operator uint() const { return pix; }
private:
    uint16_t pix;
};

// ============================================================================

/*! Pixel Blue Green Red 15-bit. */
class PixBGR555
{
public:
    PixBGR555() {}
    PixBGR555(PixG8 r, PixG8 g, PixG8 b) { init(r, g, b); }
    PixBGR555(PixG8 g) { init(g, g, g); }
    PixBGR555(PixABGR32 c) {
        init(c.red_8bit(),
             c.green_8bit(),
             c.blue_8bit());
    }

    void init(PixG8 r, PixG8 g, PixG8 b)
    {
        pix = (((static_cast<uint>(r) >> 3) <<  0) bitor
               ((static_cast<uint>(g) >> 3) <<  5) bitor
               ((static_cast<uint>(b) >> 3) << 10));
    }

    PixG8 red_5bit  () const { return (pix >>  0) bitand bitmask(5); }
    PixG8 green_5bit() const { return (pix >>  5) bitand bitmask(5); }
    PixG8 blue_5bit () const { return (pix >> 10) bitand bitmask(5); }

    friend std::ostream& operator << (std::ostream& os, const PixBGR555 a) {
        return os << "[ "
                  << a.red_5bit() << ' '
                  << a.green_5bit() << ' '
                  << a.blue_5bit()
                  << " ]";
    }

    operator uint() const { return pix; }
private:
    uint16_t pix;
};

// ============================================================================

/*! Pixel Red Green Blue 15-bit. */
class PixRGB565
{
public:
    PixRGB565() {}
    PixRGB565(PixG8 r, PixG8 g, PixG8 b) { init(r, g, b); }
    PixRGB565(PixG8 g) { init(g, g, g); }
    PixRGB565(PixABGR32 c) {
        init(c.red_8bit(),
             c.green_8bit(),
             c.blue_8bit());
    }

    void init(PixG8 r, PixG8 g, PixG8 b)
    {
        pix = (((static_cast<uint>(b) >> 3) <<  0) bitor
               ((static_cast<uint>(g) >> 2) <<  5) bitor
               ((static_cast<uint>(r) >> 3) << 11));
    }

    PixG8 red_5bit  () const { return (pix >> 11) bitand bitmask(5); }
    PixG8 green_6bit() const { return (pix >>  5) bitand bitmask(6); }
    PixG8 blue_5bit () const { return (pix >>  0) bitand bitmask(5); }

    friend std::ostream& operator << (std::ostream& os, const PixRGB565 a) {
        return os << "[ "
                  << a.red_5bit() << ' '
                  << a.green_6bit() << ' '
                  << a.blue_5bit()
                  << " ]";
    }

    operator uint() const { return pix; }
private:
    uint16_t pix;
};

// ============================================================================

/*! Pixel Blue Green Red 16-bit. */
class PixBGR565
{
public:
    PixBGR565() {}
    PixBGR565(PixG8 r, PixG8 g, PixG8 b) { init(r, g, b); }
    PixBGR565(PixG8 g) { init(g, g, g); }
    PixBGR565(PixABGR32 c) {
        init(c.red_8bit(),
             c.green_8bit(),
             c.blue_8bit());
    }

    void init(PixG8 r, PixG8 g, PixG8 b)
    {
        pix = (((static_cast<uint>(r) >> 3) <<  0) bitor
               ((static_cast<uint>(g) >> 2) <<  5) bitor
               ((static_cast<uint>(b) >> 3) << 11));
    }

    PixG8 red_5bit  () const { return (pix >>  0) bitand bitmask(5); }
    PixG8 green_6bit() const { return (pix >>  5) bitand bitmask(6); }
    PixG8 blue_5bit () const { return (pix >> 11) bitand bitmask(5); }

    friend std::ostream& operator << (std::ostream& os, const PixBGR565 a) {
        return os << "[ "
                  << a.red_5bit() << ' '
                  << a.green_6bit() << ' '
                  << a.blue_5bit()
                  << " ]";
    }

    operator uint() const { return pix; }
private:
    uint16_t pix;
};

// ============================================================================

/*! Pixel Unused Red Green Blue 32-bit. */
class PixURGB32
{
public:
    PixURGB32() {}
    PixURGB32(PixG8 r, PixG8 g, PixG8 b) { init(r, g, b); }
    PixURGB32(PixG8 g) { init(g, g, g); }
    PixURGB32(PixABGR32 c) {
        init(c.red_8bit(),
             c.green_8bit(),
             c.blue_8bit());
    }

    void init(PixG8 r, PixG8 g, PixG8 b) {
        pix = (((static_cast<ulong>(b) bitand bitmask(8)) <<  0) bitor
               ((static_cast<ulong>(g) bitand bitmask(8)) <<  8) bitor
               ((static_cast<ulong>(r) bitand bitmask(8)) << 16));
    }

    PixG8 red_8bit  () const { return (pix >> 16) bitand bitmask(8); }
    PixG8 green_8bit() const { return (pix >>  8) bitand bitmask(8); }
    PixG8 blue_8bit () const { return (pix >>  0) bitand bitmask(8); }

    friend std::ostream& operator << (std::ostream& os, const PixURGB32 a) {
        return os << "[ "
                  << a.red_8bit() << ' '
                  << a.green_8bit() << ' '
                  << a.blue_8bit()
                  << " ]";
    }

    operator ulong() const { return pix; }
private:
    uint32_t pix;
};

// ============================================================================

/*! Pixel Unused Blue Green Red 32-bit. */
class PixUBGR32
{
public:
    PixUBGR32() {}
    PixUBGR32(PixG8 r, PixG8 g, PixG8 b) { init(r, g, b); }
    PixUBGR32(PixG8 g) { init(g, g, g); }
    PixUBGR32(PixABGR32 c) {
        init(c.red_8bit(),
             c.green_8bit(),
             c.blue_8bit());
    }

    void init(PixG8 r, PixG8 g, PixG8 b) {
        pix = (((static_cast<ulong>(r) bitand bitmask(8)) <<  0) bitor
               ((static_cast<ulong>(g) bitand bitmask(8)) <<  8) bitor
               ((static_cast<ulong>(b) bitand bitmask(8)) << 16));
    }

    PixG8 red_8bit  () const { return (pix >>  0) bitand bitmask(8); }
    PixG8 green_8bit() const { return (pix >>  8) bitand bitmask(8); }
    PixG8 blue_8bit () const { return (pix >> 16) bitand bitmask(8); }

    friend std::ostream& operator << (std::ostream& os, const PixUBGR32 a) {
        return os << "[ "
                  << a.red_8bit() << ' '
                  << a.green_8bit() << ' '
                  << a.blue_8bit()
                  << " ]";
    }

    operator ulong() const { return pix; }
private:
    uint32_t pix;
};

// ============================================================================

/*!
 * Convert a pixel to another format.
 *
 * \param[in] a is the pixel to be converted.
 * \param[in] f is the pixel format to convert to.
 */
inline pure uint32_t pix_convert(PXF_t f, PixABGR32 a)
{
    uint32_t p;

    switch (f)
    {
    case PXF_RGB32: p = static_cast<PixURGB32>(a); break;
    case PXF_BGR32: p = static_cast<PixUBGR32>(a); break;
    case PXF_RGB555: p = static_cast<PixRGB555>(a); break;
    case PXF_RGB565: p = static_cast<PixRGB565>(a); break;
    default:
        leprintf("could not handle format %s\n", PXF_getName(f));
        p = 0;
        break;
    }

    return p;
}
