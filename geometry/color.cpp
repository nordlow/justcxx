#include <math.h>
#include <string.h>
#include "color.hpp"
#include "color_constants.hpp"
#include "extremes.h"

void
float_convert_HSV2RGB(float h, float s, float v, float *r, float *g, float *b)
{
    float aa, bb, cc, f;
    if (s == 0) {			/* zero saturation means grayscale */
        *r = *g = *b = v;
    } else {			/* otherwise we have color */
        if (h == 1.0) { h = 0; }
        h *= 6.0;
        int i = (int) floor(h);
        f = h - i;
        aa = v * (1 - s);
        bb = v * (1 - (f * s));
        cc = v * (1 - ((1 - f) * s));
        switch (i) {
        case 0:
            *r = v;
            *g = cc;
            *b = aa;
            break;
        case 1:
            *r = bb;
            *g = v;
            *b = aa;
            break;
        case 2:
            *r = aa;
            *g = v;
            *b = cc;
            break;
        case 3:
            *r = aa;
            *g = bb;
            *b = v;
            break;
        case 4:
            *r = cc;
            *g = aa;
            *b = v;
            break;
        case 5:
            *r = v;
            *g = aa;
            *b = bb;
            break;
        }
    }
}

int
float_convert_RGB2HSV(float r, float g, float b, float *h, float *s, float *v)
{
    const float max = std::max(r, g, b), min = std::min(r, g, b);
    const float delta = max - min;
    int ret = 0;

    /* value V */
    if (v) { *v = max; }		/* if Value pointer is given set it */

    /* saturation S */
    float s0 = (max != 0.0) ? delta / max : 0.0;
    if (s) { *s = s0; }		/* is Saturation pointer is given set it */

    /* hue H */
    if (s0 == 0.0) {
        if (h) { *h = COLOR_NO_HUE; }
        ret = -1;
    } else {
        if (h) {			/* if Hue pointer is given */
            /* set it */
            if (r == max) {
                *h = (g - b) / delta;
            } else if (g == max) {
                *h = 2 + (b - r) / delta;
            } else if (b == max) {
                *h = 4 + (r - g) / delta;
            }

            if (*h < 0) { *h += 6.0; }  /* move to range [0.0, 6.0] */
            *h /= 6.0;		  /* normalize to range [0.0, 1.0] */
        }
    }

    return ret;
}

/*! Color Component Normalization Divisor. */
inline float color_get_normd(void)
{
    return (COLOR_COMP_MAX);
}

/*! Color Component Normalization Factor. */
inline float color_get_normf(void)
{
    return (1.0f / COLOR_COMP_MAX);
}

float
color_getH(color_t a)
{
    const float normf = color_get_normf();

    float r = color_getR(a) * normf; /* normalize red */
    float g = color_getG(a) * normf; /* normalize green */
    float b = color_getB(a) * normf; /* normalize blue */

    float h;			  /* temporary HSV */
    float_convert_RGB2HSV(r, g, b, &h, NULL, NULL); /* convert */
    return h;
}

float
color_getS(color_t a)
{
    const float normf = color_get_normf();

    float r = color_getR(a) * normf; /* normalize red */
    float g = color_getG(a) * normf; /* normalize green */
    float b = color_getB(a) * normf; /* normalize blue */

    float s;			/* temporary HSV */
    float_convert_RGB2HSV(r, g, b, NULL, &s, NULL); /* convert */
    return s;
}

float
color_getV(color_t a)
{
    const float normf = color_get_normf();

    float r = color_getR(a) * normf; /* normalize red */
    float g = color_getG(a) * normf; /* normalize green */
    float b = color_getB(a) * normf; /* normalize blue */

    float v;			/* temporary HSV */
    float_convert_RGB2HSV(r, g, b, NULL, NULL, &v); /* convert */
    return v;
}

float
color_getV_alt(color_t a)
{
    const float normf = color_get_normf();

    float r = color_getR(a) * normf; /* normalize red */
    float g = color_getG(a) * normf; /* normalize green */
    float b = color_getB(a) * normf; /* normalize blue */

    return (0.3f * r + 0.59f * g + 0.11f * b);
}

int
color_rdHSV(color_t a, float * h, float * s, float * v)
{
    const float normf = color_get_normf();

    float r = color_getR(a) * normf; /* normalize red */
    float g = color_getG(a) * normf; /* normalize green */
    float b = color_getB(a) * normf; /* normalize blue */
    return float_convert_RGB2HSV(r, g, b, h, s, v);
}

color_t
color_modH(color_t a, float h)
{
    const float normd = color_get_normd();
    const float normf = color_get_normf();

    /* initialize temporary RGB */
    float r = (float)color_getR(a) * normf; /* normalize red */
    float g = (float)color_getG(a) * normf; /* normalize green */
    float b = (float)color_getB(a) * normf; /* normalize blue */

    float s, v;					/* temporary HSV */
    float_convert_RGB2HSV(r, g, b, NULL, &s, &v);	/* convert */
    float_convert_HSV2RGB(h, s, v, &r, &g, &b);	/* convert back */

    int r8 = r * normd;
    int g8 = g * normd;
    int b8 = b * normd;

    return color_genRGBA(r8, g8, b8, color_getA(a));
}

color_t
color_modV(color_t a, float v)
{
    const float normd = color_get_normd();
    const float normf = color_get_normf();

    /* initialize temporary RGB */
    float r = (float)color_getR(a) * normf; /* normalize red */
    float g = (float)color_getG(a) * normf; /* normalize green */
    float b = (float)color_getB(a) * normf; /* normalize blue */

    float h, s;					/* temporary HSV */
    float_convert_RGB2HSV(r, g, b, &h, &s, NULL); /* convert */
    float_convert_HSV2RGB(h, s, v, &r, &g, &b);	/* convert back */

    int r8 = r * normd;
    int g8 = g * normd;
    int b8 = b * normd;

    return color_genRGBA(r8, g8, b8, color_getA(a));
}

color_t
color_modS(color_t a, float s)
{
    const float normd = color_get_normd();
    const float normf = color_get_normf();

    /* initialize temporary RGB */
    float r = (float)color_getR(a) * normf; /* normalize red */
    float g = (float)color_getG(a) * normf; /* normalize green */
    float b = (float)color_getB(a) * normf; /* normalize blue */

    float h, v;					/* temporary HSV */
    float_convert_RGB2HSV(r, g, b, &h, NULL, &v); /* convert */
    float_convert_HSV2RGB(h, s, v, &r, &g, &b);	/* convert back */

    int r8 = r * normd;
    int g8 = g * normd;
    int b8 = b * normd;

    return color_genRGBA(r8, g8, b8, color_getA(a));
}

color_t
color_modHS(color_t a, float h, float s)
{
    const float normd = color_get_normd();
    const float normf = color_get_normf();

    /* initialize temporary RGB */
    float r = (float)color_getR(a) * normf; /* normalize red */
    float g = (float)color_getG(a) * normf; /* normalize green */
    float b = (float)color_getB(a) * normf; /* normalize blue */

    float v;			    /* temporary HSV */
    float_convert_RGB2HSV(r, g, b, NULL, NULL, &v); /* convert */
    float_convert_HSV2RGB(h, s, v, &r, &g, &b);	    /* convert back */

    int r8 = r * normd;
    int g8 = g * normd;
    int b8 = b * normd;

    return color_genRGBA(r8, g8, b8, color_getA(a));
}

color_t
color_mixHSVA(float h, float s, float v, uint8_t a)
{
    const float normd = color_get_normd();
    float r, g, b;
    float_convert_HSV2RGB(h, s, v, &r, &g, &b); /* convert back */
    uint r8 = r * normd;
    uint g8 = g * normd;
    uint b8 = b * normd;
    return color_genRGBA(r8, g8, b8, a);
}

color_t
color_saddHSV(color_t a, float hx, float sx, float vx)
{
    const float normd = color_get_normd();
    const float normf = color_get_normf();
    float r = (float)color_getR(a) * normf; /* normalize red */
    float g = (float)color_getG(a) * normf; /* normalize green */
    float b = (float)color_getB(a) * normf; /* normalize blue */
    float h, s, v;		/* temporary HSV */
    float_convert_RGB2HSV(r, g, b, &h, &s, &v);	/* convert */
    h = float_clamp(0.0, h + hx, 1.0); /* shift hue */
    s = float_clamp(0.0, s + sx, 1.0); /* shift saturation */
    v = float_clamp(0.0, v + vx, 1.0); /* shift value */
    float_convert_HSV2RGB(h, s, v, &r, &g, &b);	/* convert back */
    int r8 = r * normd;
    int g8 = g * normd;
    int b8 = b * normd;
    return color_genRGBA(r8, g8, b8, color_getA(a));
}

void
colorarray_ramp(color_t* c, int l, color_t first, color_t last)
{
    int i, l1 = l - 1;
    int dr = color_getR(last) - color_getR(first);	/* delta red */
    int dg = color_getG(last) - color_getG(first);	/* delta green */
    int db = color_getB(last) - color_getB(first);	/* delta blue */
    if (l1 < 1) {
        return;
    }
    for (i = 0; i < l; i++) {
        c[i] = color_genRGB((int) color_getR(first) + i * dr / l1,
                            (int) color_getG(first) + i * dg / l1,
                            (int) color_getB(first) + i * db / l1);
    }
}

void
colorarray_ramps(color_t* c, int l, color_t* ctrl, int ctrlN)
{
    int i, begin = 0, end;
    int nramps;
    if (l < 1) {
        return;
    }
    nramps = ctrlN - 1;
    for (i = 0; i < nramps; i++) {
        end = (i + 1) * l / nramps;
        colorarray_ramp(c + begin, end - begin, ctrl[i], ctrl[i + 1]);
        begin = end - 1;		/* -1 to avoid duplication at ctrl points */
        if (begin < 0) {
            begin = 0;
        }
    }
}

void
CPAL_gen(color_t* c, int l, CPAL_t cpal)
{
    switch (cpal) {
    case CPAL_GRAY: {
        /* see: /usr/share/octave/2.1.34/m/image/gray.m */
        color_t ctrl[] = { COLOR_BLACK,
                           COLOR_WHITE };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        break;
    }
    case CPAL_HOT: {
        /* Borrowed from Matlab */
        color_t ctrl[] = { COLOR_BLACK,
                           COLOR_RED,
                           COLOR_YELLOW,
                           COLOR_WHITE };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        break;
    }
    case CPAL_COOL: {
        /* Borrowed from Matlab */
        color_t ctrl[] = { COLOR_BLACK,
                           COLOR_BLUE,
                           COLOR_CYAN,
                           COLOR_WHITE };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        ;
        break;
    }
    case CPAL_OCEAN: {
        /* Borrowed from /usr/share/octave/2.1.34/m/image/ocean.m */
        color_t ctrl[] = { COLOR_BLACK,
                           COLOR_RGB(0, 0, 255/3),
                           COLOR_RGB(0, 255/2, 2*255/3),
                           COLOR_RGB(255, 255, 255) };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        break;
    }
    case CPAL_SPECTRUM: {
        color_t ctrl[] = { COLOR_BLUE,
                           COLOR_CYAN,
                           COLOR_GREEN,
                           COLOR_YELLOW,
                           COLOR_RED
        };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        break;
    }
    case CPAL_CYCLIC_SPECTRUM: {
        color_t ctrl[] = { COLOR_BLUE,
                           COLOR_CYAN,
                           COLOR_GREEN,
                           COLOR_YELLOW,
                           COLOR_RED,
                           COLOR_DARK_MAGENTA,
                           COLOR_BLUE
        };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        break;
    }
    case CPAL_TRAFFIC_LIGHTS: {
        color_t ctrl[] = { COLOR_GREEN,
                           COLOR_YELLOW,
                           COLOR_RED
        };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        break;
    }
    case CPAL_RGBR: {
        color_t ctrl[] = { COLOR_RED,
                           COLOR_GREEN,
                           COLOR_BLUE,
                           COLOR_RED
        };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        break;
    }
    case CPAL_GOTT_OCH_BLANDAT: {
        color_t ctrl[] = { COLOR_RED,
                           COLOR_MEDIUM_SEA_GREEN,
                           COLOR_ORANGE,
                           color_genRGB(192,192,0) };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        break;
    }
    case CPAL_PASTELLS_MEDIUM: {
        color_t ctrl[] = { color_genRGB(244, 176, 176),
                           color_genRGB(234, 215, 159),
                           color_genRGB(244, 243, 154),
                           color_genRGB(176, 244, 209),
                           color_genRGB(176, 215, 244),
                           color_genRGB(192, 176, 244),
                           color_genRGB(239, 196, 235) };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
    }
    case CPAL_RAINBOW: {
        color_t ctrl[] = { PURE_RED,
                           PURE_ORANGE,
                           PURE_YELLOW,
                           PURE_GREEN,
                           PURE_CYAN,
                           PURE_BLUE,
                           PURE_VIOLET };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
    }
    case CPAL_NICE_ALPHA: {
        color_t ctrl[] = { LIGHT_YELLOW,
                           NAVAJO_WHITE,
                           color_genRGB(188, 232, 174) };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
    }
    case CPAL_COLORS_ON_LIGHT: {
        color_t ctrl[] = { DARK_RED,
                           FOREST_GREEN,
                           ROYAL_BLUE,
                           LIGHT_YELLOW2,
                           LIGHT_CYAN2,
                           DARK_MAGENTA,
                           DARK_ORANGE2,
                           VIOLET_RED2,
                           MEDIUM_PURPLE2 };
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
    }
    default: {
        color_t ctrl[] = { COLOR_BLACK,
                           COLOR_WHITE };
        leprintf("Unknown color palette %d. Defaulting to GRAY.\n", cpal);
        colorarray_ramps(c, l, ctrl, ARRL(ctrl));
        break;
    }
    }
}

void colorarray_randRGB(color_t* x, size_t n)
{
    for (size_t i = 0; i < n; i++) { x[i] = color_randRGB(); }
}
void colorarray_randRGBA(color_t* x, size_t n)
{
    for (size_t i = 0; i < n; i++) { x[i] = color_randRGBA(); }
}

void colorarray_highlightInd(color_t* x, size_t n, float norm_prg)
{
    for (size_t i = 0; i < n; i++) { x[i] = color_highlightInd(x[i], norm_prg); }
}

void colorarray_modA(color_t* x, size_t n, uint8_t alpha)
{
    for (size_t i = 0; i < n; i++) { x[i] = color_modA(x[i], alpha); }
}

void colorarray_scaleT(color_t* x, size_t n, uint8_t prg)
{
    for (size_t i = 0; i < n; i++) { x[i] = color_scaleT(x[i], prg); }
}
