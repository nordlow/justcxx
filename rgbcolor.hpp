#pragma once

/*!
 * \file rgbcolor.hpp
 * \brief Generalized templatized red green blue (RGB) color class.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include "clamp.hpp"

// ============================================================================

template<class T>
class rgbcolor
{
public:

    rgbcolor() {}

    rgbcolor(T gray) { init(gray); }

    rgbcolor(T red, T green, T blue) { init(red,green,blue); }

    void init(T gray) { red = gray; green = gray; blue = gray; }
    void init(T rr, T gg, T bb) { red = rr; green = gg; blue = bb; }

    // pointwise binary arithmetic

    friend rgbcolor operator + (const rgbcolor & c0, const rgbcolor & c1)
    {
	return rgbcolor(c0.red + c1.red, c0.green + c1.green, c0.blue + c1.blue);
    }
    friend rgbcolor operator - (const rgbcolor & c0, const rgbcolor & c1)
    {
	return rgbcolor(c0.red - c1.red, c0.green - c1.green, c0.blue - c1.blue);
    }
    friend rgbcolor pwmul(const rgbcolor & c0, const rgbcolor & c1)
    {
	return rgbcolor(c0.red * c1.red, c0.green * c1.green, c0.blue * c1.blue);
    }
    friend rgbcolor operator * (const rgbcolor & c0, const rgbcolor & c1)
    {
	return pwmul(c0, c1);
    }

    friend rgbcolor operator + (const rgbcolor & color, T a)
    {
	return rgbcolor(color.red + a, color.green + a, color.blue + a);
    }
    friend rgbcolor operator - (const rgbcolor & color, T a)
    {
	return rgbcolor(color.red - a, color.green - a, color.blue - a);
    }
    friend rgbcolor pwmul_scalar(const rgbcolor & color, T a)
    {
	return rgbcolor(color.red * a, color.green * a, color.blue * a);
    }
    friend rgbcolor operator * (const rgbcolor & color, T a)
    {
	return pwmul_scalar(color, a);
    }
    friend rgbcolor pwdiv_scalar(const rgbcolor & color, T a)
    {
	return rgbcolor(color.red / a, color.green / a, color.blue / a);
    }
    friend rgbcolor operator / (const rgbcolor & color, T a)
    {
	return pwdiv_scalar(color, a);
    }

    // pointwise binary arithmetic and assignment

    void operator += (T a) { red += a; green += a; blue += a; };
    void operator -= (T a) { red -= a; green -= a; blue -= a; };
    void operator *= (T a) { red *= a; green *= a; blue *= a; };
    void operator /= (T a) { red /= a; green /= a; blue /= a; };

    void operator += (const rgbcolor & a)
    {
	red += a.red; green += a.green; blue += a.blue;
    }
    void operator -= (const rgbcolor & a)
    {
	red -= a.red; green -= a.green; blue -= a.blue;
    }
    void operator *= (const rgbcolor & a)
    {
	red *= a.red; green *= a.green; blue *= a.blue;
    }
    void operator /= (const rgbcolor & a)
    {
	red /= a.red; green /= a.green; blue /= a.blue;
    }

    // clamp

    void clamp(T lower, T upper)
    {
	red   = pnw::clamp(lower, red,   upper);
	green = pnw::clamp(lower, green, upper);
	blue  = pnw::clamp(lower, blue,  upper);
    }

    uchar red_8bit() const
    {
	return(uchar) pnw::clamp((T)0, (T)red,   (T)255);
    }

    uchar green_8bit() const
    {
	return(uchar) pnw::clamp((T)0, (T)green, (T)255);
    }

    uchar blue_8bit() const
    {
	return(uchar) pnw::clamp((T)0, (T)blue,  (T)255);
    }

    // io

    void print(std::ostream & os) const
    {
	os << "rgbcolor: " << red << ' ' << green << ' ' << blue;
    }

    friend std::ostream & operator << (std::ostream & os, const rgbcolor a)
    {
	a.print(os); return os;
    }

private:
    T red, green, blue;        // red, green, blue
};
