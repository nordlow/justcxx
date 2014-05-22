/*!
 * \file vips_x.hpp
 * \brief Extensions to VIPS Image Processing Library.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-09-29 12:59
 */

#pragma once
#include <vips/vips>
#include <iostream>
#include <iomanip>

namespace vips {

inline VImage resize(VImage & a, double factor)
{
    return a.resize_linear(factor*a.Xsize(),
                           factor*a.Ysize());
}
inline VImage resize_to_width(VImage & a, int width)
{
    return a.resize_linear(width,
                           round(static_cast<double>(a.Ysize()) * width / a.Xsize()));
}
inline VImage resize_to_height(VImage & a, int height)
{
    return a.resize_linear(round(static_cast<double>(a.Xsize()) * height / a.Ysize()),
                           height);
}

/// Add Intensity
template<typename S, typename T> inline auto operator +=(S& a, const T& b) throw(VError) -> decltype(a+b)  { return a = a + b; }
// inline VImage operator +=(VImage& a, const VImage& b)  throw( VError ) { return a = a + b; }
// inline VImage operator +=(VImage& a, double b)  throw( VError ) { return a = a + b; }

/// Subtract Intensity
template<typename S, typename T> inline auto operator -=(S& a, const T& b) throw(VError) -> decltype(a-b)  { return a = a - b; }
// inline VImage operator -=(VImage& a, const VImage& b) throw(VError) { return a = a - b; }
// inline VImage operator -=(VImage& a, double b) throw(VError) { return a = a - b; }

/// Multiply Intensity
template<typename S, typename T> inline auto operator *=(S& a, const T& b) throw(VError) -> decltype(a*b)  { return a = a * b; }
// inline VImage operator *=(VImage& a, const VImage& b) throw(VError) { return a = a * b; }
// inline VImage operator *=(VImage& a, double b) throw(VError) { return a = a * b; }

/// Divide Intensity
template<typename S, typename T> inline auto operator /=(S& a, const T& b) throw(VError) -> decltype(a/b)  { return a = a / b; }
// inline VImage operator /=(VImage& a, const VImage& b) throw(VError) { return a = a / b; }
// inline VImage operator /=(VImage& a, double b) throw(VError) { return a = a / b; }

/// Modulus Intensity
template<typename S, typename T> inline auto operator %=(S& a, const T& b) throw(VError) -> decltype(a%b)  { return a = a % b; }
// inline VImage operator %=(VImage& a, const VImage& b) throw(VError) { return a = a % b; }
// inline VImage operator %=(VImage& a, double b) throw(VError) { return a = a % b; }

/// And
template<typename S, typename T> inline auto operator &=(S& a, const T& b) throw(VError) -> decltype(a&b)  { return a = a & b; }
// inline VImage operator &=(VImage& a, const VImage& b) throw(VError) { return a = a & b; }
// inline VImage operator &=(VImage& a, int b) throw(VError) { return a = a & b; }

/// Or
template<typename S, typename T> inline auto operator |=(S& a, const T& b) throw(VError) -> decltype(a|b)  { return a = a | b; }
// inline VImage operator |=(VImage& a, const VImage& b) throw(VError) { return a = a | b; }
// inline VImage operator |=(VImage& a, int b) throw(VError) { return a = a | b; }

/// Shift Left
inline VImage operator <<=(VImage& a, int b) throw(VError) { return a = a << b; }

/// Shift Right
inline VImage operator >>=(VImage& a, int b) throw(VError) { return a = a >> b; }
}
