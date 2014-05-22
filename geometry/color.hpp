/*! \file color.hpp
 * \brief Color Type and Related Macros and Constants.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "../utils.h"
#include "../byterev.h"
#include "../extremes.h"
#include "../rangerand.h"
#include "../sadd.h"
#include "../clamp.hpp"

/*! Platform Independent Representation of Color.
 *
 * Pixel format is chosen to be UBGR32 / ABGR32 which means that the
 * - first byte is Red,
 * - second is Green,
 * - third is Blue and
 * - the fourth and the last is either unused or used as transparency (Alpha).
 *
 * Note that this format is assumed by other code to be UBGR32.
 * Changing the macros related to this color_t will result in incorrect color
 * handling.
 */
typedef uint32_t color_t;

/*! OpenGL Pixel Type. */
typedef uint32_t cGL_t;

/* ---------------------------- Group Separator ---------------------------- */

/*! These CPU-sensitive definitions assure that a color is always stored so
 * that the Color Component
 * - Red   is always stored at byte offset 0
 * - Green is always stored at byte offset 1
 * - Blue  is always stored at byte offset 2
 * - Alpha is always stored at byte offset 3
 *
 * This enables us to use a pointer to an array of color_t directly to
 * glColorPointer() independently of if we are an on
 * Big or Little Endian System.
 */

#if defined(CONFIG_LITTLE_ENDIAN)
#define COLOR_R_SH (0)		/**< Bit Position Shift to \em Red part */
#define COLOR_G_SH (8)		/**< Bit Position Shift to \em Green part */
#define COLOR_B_SH (16)		/**< Bit Position Shift to \em Blue part */
#define COLOR_A_SH (24)		/**< Bit Position Shift to \em Alpha part */
#endif

#if defined(CONFIG_BIG_ENDIAN)
#define COLOR_R_SH (24)		/**< Bit Position Shift to \em Red part */
#define COLOR_G_SH (16)		/**< Bit Position Shift to \em Green part */
#define COLOR_B_SH (8)		/**< Bit Position Shift to \em Blue part */
#define COLOR_A_SH (0)		/**< Bit Position Shift to \em Alpha part */
#endif

#define COLOR_COMP_MIN ((color_t)0x00) /**< Color Component Minimum Value */
#define COLOR_COMP_MAX ((color_t)0xff) /**< Color Component Maximum Value */

#define COLOR_R_MASK  ((color_t)(0xff << COLOR_R_SH))
#define COLOR_G_MASK  ((color_t)(0xff << COLOR_G_SH))
#define COLOR_B_MASK  ((color_t)(0xff << COLOR_B_SH))
#define COLOR_A_MASK  ((color_t)(0xff << COLOR_A_SH))

#define COLOR_RGB_MASK  ((color_t)COLOR_R_MASK | COLOR_G_MASK | COLOR_B_MASK)
#define COLOR_RGBA_MASK ((color_t)0xffffffff)

/*! Generate \c color_t from \em red component only. */
#define COLOR_R(r) ((color_t)((r) & COLOR_COMP_MAX) << COLOR_R_SH)

/*! Generate \c color_t from \em green component only. */
#define COLOR_G(g) ((color_t)((g) & COLOR_COMP_MAX) << COLOR_G_SH)

/*! Generate \c color_t from \em blue component only. */
#define COLOR_B(b) ((color_t)((b) & COLOR_COMP_MAX) << COLOR_B_SH)

/*! Set this to make color_t's internal memory storage of alpha channel
 * compatible with OpenGL's preferred transparency format, for example
 * used when specifying Color Arrays through the function @c
 * glColorPointer().
 */
#define COLOR_A_REVERSED (0)

#if COLOR_A_REVERSED
#  define COLOR_A_MIN (0xff)
#  define COLOR_A_MAX (0x00)
/*! Generate \c color_t from \em alpha component only. */
#  define COLOR_A(a) ((color_t)((COLOR_COMP_MAX-a) & COLOR_COMP_MAX) << COLOR_A_SH)
#else
#  define COLOR_A_MIN (0x00)
#  define COLOR_A_MAX (0xff)
/*! Generate \c color_t from \em alpha component only. */
#  define COLOR_A(a) ((color_t)((a) & COLOR_COMP_MAX) << COLOR_A_SH)
#endif

/*! Color Alpha Normalization Factor.
 */
#define COLOR_A_NORM (0x100)

#define COLOR_RGB(r, g, b)                      \
    (COLOR_R(r) |                               \
     COLOR_G(g) |                               \
     COLOR_B(b) |                               \
     COLOR_A(0))

#define COLOR_RGBA(r, g, b, a)                  \
    (COLOR_R(r) |                               \
     COLOR_G(g) |                               \
     COLOR_B(b) |                               \
     COLOR_A(a))

#define COLOR_GRAY(g) COLOR_RGBA(g, g, g, COLOR_A(0))

/*! Construct an RGB-Color from the red \p r, green \p g and blue \p b part.
 * Alpha part is defaulted to zero, meaning no transparency (fully opaque).
 */
inline color_t color_genRGB(uint8_t r, uint8_t g, uint8_t b) { return COLOR_RGB(r, g, b); }
/*! Construct an RGBA-Color from the red \p r, green \p g, blue \p b and
 * alpha \p a part.
 */
inline color_t color_genRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return COLOR_RGBA(r, g, b, a); }
/*! Construct a Grayscale Color with intensity \p g. */
inline color_t color_genGray(uint8_t g) { return COLOR_GRAY(g); }

/*! Get \em Red Part of \p a. */
inline uint8_t color_getR(color_t a) { return (((a) >> COLOR_R_SH) & COLOR_COMP_MAX); }
/*! Get \em Green Part of \p a. */
inline uint8_t color_getG(color_t a) { return (((a) >> COLOR_G_SH) & COLOR_COMP_MAX); }
/*! Get \em Blue Part of \p a. */
inline uint8_t color_getB(color_t a) { return (((a) >> COLOR_B_SH) & COLOR_COMP_MAX); }
/*! Get \em Alpha part of \p a. */
inline uint8_t color_getA(color_t a) { return (((a) >> COLOR_A_SH) & COLOR_COMP_MAX); }
/*! Get \em Alpha (in OpenGL format) part of \p a. */
inline uint8_t color_getA_GL(color_t a) {
#if COLOR_A_REVERSED
    return (color_getA(a));
#else
    return (COLOR_COMP_MAX-color_getA(a));
#endif
}

/*! Get \em Hue (H) part of \p a. */
float color_getH(color_t a);
/*! Get \em Saturation (S) part of \p a. */
float color_getS(color_t a);
/*! Get \em Value/Luminance (V) part of \p a. */
float color_getV(color_t a);

/*! The following code computes a CIE \em luminance value from linear
 * RGB values defined according to ITU-R Recommendation BT.709 (the
 * HDTV color standard):
 *
 * float luminance = dot(vec3(0.2125, 0.7154, 0.0721), rgbColor);
 */

/*! Get \em Value/Luminance (V) part of \p a. */
float color_getV_alt(color_t a);

/*! Get \em Hue, Saturation and Value/Luminance (HSV) part of \p a. */
int color_rdHSV(color_t a, float * h, float * s, float * v);

/*! \em Modify the Alpha-Transparency part of \p x to \p alpha and return the result.
 */
inline color_t color_modA(color_t x, uint8_t alpha)
{
    return ((x & COLOR_RGB_MASK) |
            ((color_t)alpha << COLOR_A_SH));
}

void colorarray_modA(color_t * x, size_t n, uint8_t alpha);

/*! \em Reverse the Alpha-Transparency (\c COLOR_A_MAX-a => a) part of \p x and return the result. */
inline color_t color_revA(color_t x)
{
    return ((x & COLOR_RGB_MASK) |
            ((color_t)(COLOR_A_MAX - color_getA(x)) << COLOR_A_SH));
}

/*! \em Blink the Transparency part of \p x in progress \p prg. */
inline color_t color_blinkT(color_t x, uint8_t prg)
{
    uint xA = color_getA(x);
    xA = xA + (COLOR_A_MAX-xA) * (prg+1) * 3 / 1024;
    return color_modA(x, xA);
}

/*! \em Scale the Transparency of \p x with \p prg and return the result. */
inline color_t color_scaleT(color_t x, uint8_t prg)
{
    uint xA = color_getA(x);
    xA = xA + (COLOR_A_MAX-xA) * (prg+1) / COLOR_A_NORM;
    return color_modA(x, xA);
}

void colorarray_scaleT(color_t * x, size_t n, uint8_t prg);

/*! \em Convert the color \p a to OpenGL \em RGBA format for use in OpenGL Color Arrays.
 *
 * \NOTE the combination of this functions and what arguments that were
 * given to the function \c glBlendFunc() determines the end result of
 * the OpenGL color-transparency-blending logic.
 */
inline
cGL_t color_to_GL_RGBA(color_t a) { return color_modA(a, color_getA_GL(a)); }

/*! \em Generate a Random RGB-Color and return it. */
inline color_t color_randRGB(void)
{
    return color_genRGB(uint8_rand(), uint8_rand(), uint8_rand());
}

/*! \em Generate a Random RGBA-Color and return it. */
inline color_t color_randRGBA(void)
{
    return color_genRGBA(uint8_rand(), uint8_rand(), uint8_rand(), uint8_rand());
}

/*! \em Generate a Random RGB-Color Array in \p x of length \p n. */
void colorarray_randRGB(color_t * x, size_t n);

/*! \em Generate a Random RGBA-Color Array in \p x of length \p n. */
void colorarray_randRGBA(color_t * x, size_t n);

/*! Modify the Hue of \p a to \p h where \p h within [0, 360].
 * \return the changed color
 */
color_t color_modH(color_t a, float h);

/*! Modify the Saturation of \p a to \p s within [0, 1].
 * \return the changed color
 */
color_t color_modS(color_t a, float s);

/*! Modify the Value of \p a to \p v within [0, 1].
 * \return the changed color
 */
color_t color_modV(color_t a, float v);

/*! Modify both the Hue and Saturation of \p a to \p h, s.
 * \return the changed color
 */
color_t color_modHS(color_t a, float h, float s);

/*! Construct a Fully Opaque RGB-Color by mixing Red \p r, Green \p g, Blue \p b.
 * \return the constructed color
 */
inline color_t color_mixRGB(uint8_t r, uint8_t g, uint8_t b)
{
    return color_genRGB(r, g, b);
}

/*! Construct an RGBA-Color by mixing Red \p r, Green \p g, Blue \p b and Alpha \p a.
 * \return the constructed color
 */
inline color_t color_mixRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return color_genRGBA(r, g, b, a);
}

/*! Construct a HSVA-Color by mixing Hue \p h, Saturation \p s, Value \p v and Alpha \p a.
 * \return the constructed color
 */
color_t color_mixHSVA(float h, float s, float v, uint8_t a);

/*! Construct a Full Opaque HSV-Color by mixing Hue \p h, Saturation \p s and Value \p v.
 * \return the constructed color
 */
inline color_t color_mixHSV(float h, float s, float v)
{
    return color_mixHSVA(h, s, v, 0);
}

/*! Saturated-Add to color \p a the HSV-color given by the arguments (\p hx, \p sx, \p vx).
 * \return the changed color
 */
color_t color_saddHSV(color_t a, float hx, float sx, float vx);

/*! Indication Higlighting of \p color using the normalized progress \p norm_prg [0...1].
 * This can takes some time but happens only for one (the indicated)
 * object at a time.
 */
inline color_t color_highlightInd(color_t a, float norm_prg)
{
    return color_saddHSV(a,
                         0.0f,		 /* Hue Shift */
                         0.0f,		 /* Saturation Shift */
                         0.3f * norm_prg); /* Value Shift */
}

void colorarray_highlightInd(color_t * x, size_t n, float norm_prg);

/*! Print Color \p a in the format (r,g,b,a).
 * Example: Opaque White is printed as (255,255,255,0).
 */
inline void color_fprint(FILE * stream, color_t a)
{
    fprintf(stream, "(%d,%d,%d,%d)",
            color_getR(a), color_getG(a), color_getB(a), color_getA(a));
}

/*! Color Space Conversion from HSV to RGB.
 *
 * HSV stands for (Hue, Saturation, Value)
 * RGB stands for (Red, Green, Blue)
 *
 * \param[in] h Hue in range [0..1]
 * \param[in] s Saturation in range [0..1]
 * \param[in] v Value in range [0..1]

 * \param[out] r Red in range [0..1]
 * \param[out] g Green in range [0..1]
 * \param[out] b Blue in range [0..1]
 *
 * \see Computer Graphics, Chapter 15, p 578-9
 * \see www.wikipedia.org on "HSV color space"
 */
void float_convert_HSV2RGB(float h, float s, float v, float *r, float *g, float *b);

/*! Used as a return value in Hue part to indicate that no hue-value could be
 * defined in the conversion.
 */
#define COLOR_NO_HUE (-1)

/*! Color Space Conversion from RGB to HSV.
 *
 * RGB stands for (Red, Green, Blue)
 * HSV stands for (Hue, Saturation, Value)

 * \param[in] r Red in range [0..1]
 * \param[in] g Green in range [0..1]
 * \param[in] b Blue in range [0..1]
 *
 * \param[out] h Hue in range [0..1]
 * \param[out] s Saturation in range [0..1]
 * \param[out] v Value in range [0..1]
 *
 * \see Computer Graphics, Chapter 15, p 578
 * \see www.wikipedia.org on "HSV color space"
 *
 * \return 0 on success or -1 if no corresponding Hue (H) could be found.
 */
int float_convert_RGB2HSV(float r, float g, float b, float *h, float *s, float *v);

/*! Return the Elementwise Saturated Sum of the two colors \p a and \p b.
 * Each color component value is saturated to the maximum value of 255.
 * This is a special case of alpha-blending with the alpha factor set to zero.
 */
inline color_t color_sadd(color_t a, color_t b)
{
    uint8_t red = uint8_sadd(color_getR(a), color_getR(b));
    int green = uint8_sadd(color_getG(a), color_getG(b));
    int blue = uint8_sadd(color_getB(a), color_getB(b));
    return color_genRGB(red, green, blue);
}

/*! Set the \em Intensity of \p a to \p i between 0 and 255. */
inline color_t color_scaleGREY(color_t a, uint8_t i)
{
    int r = color_getR(a), g = color_getG(a), b = color_getB(a);
    const int m = (r + g + b) / 3; /* use arithmetic mean */
    r = pnw::clamp<int>(0, r * i / m, COLOR_COMP_MAX);
    g = pnw::clamp<int>(0, g * i / m, COLOR_COMP_MAX);
    b = pnw::clamp<int>(0, b * i / m, COLOR_COMP_MAX);
    const int x = color_genRGB(r, g, b);
    return x;
}

/*! Change Intensity of \p a with \p i.
 * \param[in] i should lie between - \c COLOR_COMP_MAX and \c COLOR_COMP_MAX.
 */
inline color_t color_saddGREY(color_t a, int i)
{
    int r = color_getR(a), g = color_getG(a), b = color_getB(a);
    int m = (r + g + b) / 3; /* use arithmetic mean */
    m = std::max<int>(m, 1);	    /* for black (r=0, g=0, b=0) set m to 1 */
    r = pnw::clamp<int>(0, r + i * r / m, COLOR_COMP_MAX);
    g = pnw::clamp<int>(0, g + i * g / m, COLOR_COMP_MAX);
    b = pnw::clamp<int>(0, b + i * b / m, COLOR_COMP_MAX);
    const int x = color_genRGB(r, g, b);
    return x;
}

/*! \em Linearly \em Interpolate from \p src to \p dst at progress \p prg. */
inline color_t
linear_interpolate(const color_t src, float prg, const color_t dst)
{
    const int sR = color_getR(src);
    const int sG = color_getG(src);
    const int sB = color_getB(src);
    const int sA = color_getA(src);
    const int dR = color_getR(dst);
    const int dG = color_getG(dst);
    const int dB = color_getB(dst);
    const int dA = color_getA(dst);
    prg = pnw::clamp<float>(0, prg, 1); /* colors must be forced within range */
    return color_genRGBA((int)(sR + ((float)dR - sR) * prg),
                         (int)(sG + ((float)dG - sG) * prg),
                         (int)(sB + ((float)dB - sB) * prg),
                         (int)(sA + ((float)dA - sA) * prg));
}

/*! \em Linearly \em Interpolate from \p src through \p mid to \p dst at progress \p prg. */
inline color_t
linear_interpolate3(const color_t src, const color_t mid, const color_t dst, float prg)
{
    const int sR = color_getR(src);
    const int sG = color_getG(src);
    const int sB = color_getB(src);
    const int sA = color_getA(src);

    const int mR = color_getR(mid);
    const int mG = color_getG(mid);
    const int mB = color_getB(mid);
    const int mA = color_getA(mid);

    const int dR = color_getR(dst);
    const int dG = color_getG(dst);
    const int dB = color_getB(dst);
    const int dA = color_getA(dst);

    prg = pnw::clamp<float>(0, prg, 1); /* colors must be forced within range */
    if (prg < 0.5f) {		/* first half */
        return color_genRGBA((int)(sR + ((float)mR - sR) * prg),
                             (int)(sG + ((float)mG - sG) * prg),
                             (int)(sB + ((float)mB - sB) * prg),
                             (int)(sA + ((float)mA - sA) * prg));
    } else {			/* second half */
        return color_genRGBA((int)(mR + ((float)dR - mR) * prg),
                             (int)(mG + ((float)dG - mG) * prg),
                             (int)(mB + ((float)dB - mB) * prg),
                             (int)(mA + ((float)dA - mA) * prg));
    }
}

/*! Create a Color Ramp in \p of length \p l from \p first to \p last. */
void colorarray_ramp(color_t * c, int l,
                     color_t first, color_t last);
/*! Create a Set of Color Ramps in \p of length \p l using the \p ctrlN number of control colors in \p ctrls. */
void colorarray_ramps(color_t * c, int l,
                      color_t * ctrls, int ctrlN);

/*! Color Palettes. */
typedef enum
{
    CPAL_GRAY,
    CPAL_HOT,
    CPAL_COOL,
    CPAL_OCEAN,
    CPAL_SPECTRUM,
    CPAL_CYCLIC_SPECTRUM,
    CPAL_TRAFFIC_LIGHTS,
    CPAL_RGBR,
    CPAL_GOTT_OCH_BLANDAT,
    CPAL_PASTELLS_MEDIUM,
    CPAL_RAINBOW,
    CPAL_NICE_ALPHA,
    CPAL_COLORS_ON_LIGHT,

    CPAL_NUM
} CPAL_t;

#define CPAL_FIRST (CPAL_GRAY) /**< First Palette Index. */
#define CPAL_LAST (CPAL_NUM-1) /**< Last Palette Index. */

/*! Generate a Color Palette at \p c of type \p cpal scaled to length \p l. */
void CPAL_gen(color_t * c, int l, CPAL_t cpal);
