/*!
 * \file font.hpp
 * \brief Font Handling.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

/* ========================================================================= */

#pragma once

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if DVIS_USE_GL
/* #include <GL/gl.h> */
#endif

#include "TSTYLE.h"
#include "CLAY.hpp"

#ifdef HAVE_LIBFREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
/*! Lookup Freetype2 Error Message as a statically allocated C string.
 * \return error message C string
 */
const char * FT_strerror(FT_Error error);
#endif

/* ---------------------------- Group Separator ---------------------------- */

class GGlyph {
public:
#ifdef HAVE_LIBFREETYPE
    FT_UInt          gidx;	/**< Glyph \em Index. */
    FT_Glyph         glyph;	/**< \em Handle (Pointer) to glyph slot. */
    FT_Glyph_Metrics gmet;
#endif

#if DVIS_USE_GL
    uint             texID;	/**< \em Texture ID. */
#endif
    uint8_t          w_binlog, h_binlog; /**< \em Dimensions Binary Logarithm */

    size_t           met_cnt;   /**< Number of objects using this glyph metrics. */
    size_t           tex_cnt;   /**< Number of objects showing this texture. */
};

/* ---------------------------- Group Separator ---------------------------- */

#ifdef HAVE_LIBFREETYPE

extern int g_ft_point_size_bits;

#define GFONT_MAX (1024)		/**< Maximum number of Glyphs in Font */

/*!
 * Font.
 */
class GFont {
public:
    FT_Face face;                 ///< Freetype Face.
    GGlyph * greg;		/**< Glyphs Registry. */
    size_t gregN;			/**< Glyphs Registry Counter. */
};

/*!
 * Gathers all styles for a specific font.
 */
class GFontSet {
public:
    GFont r;			/**< \em Regular Font. */
    GFont b;			/**< \em Bold Font. */
    GFont i;			/**< \em Italic Font. */
    GFont bi;			/**< \em Bold Italic Font. */
};

extern GFontSet g_serif;	/**< Serif Font. */
extern GFontSet g_sans;		/**< Sans Serif Font. */
extern GFontSet g_fixed;	/**< Fixed Font. */
extern GFontSet g_vera;		/**< Vera Font. */

#endif

/* ---------------------------- Group Separator ---------------------------- */

/*! Lookup suitable font matching tstyle and return a pointer to the font. */
GFont * gfont_lookup(TSTYLE_t tstyle);

/*! Use (possibly load) glyph representing \p val. */
void gfont_loadMet(TSTYLE_t tstyle, ulong val);
/*! Unuse (possibly unload) glyph representing \p val. */
void gfont_unloadMet(TSTYLE_t tstyle, ulong val);

/*! Load glyph representing \p val of text style \p tstyle. */
void gfont_loadTex(TSTYLE_t tstyle, ulong val, bool mipmap_flag);

/*! Unload glyph representing \p val of text style \p tstyle. */
void gfont_unloadTex(TSTYLE_t tstyle, ulong val);

/* ---------------------------- Group Separator ---------------------------- */

/*! Lookup glyph representing \p val and return it. */
const GGlyph * gglyph_lookup(TSTYLE_t tstyle, ulong val);

int gglyph_lookup_Bitmap_W_H(const GGlyph * gglyph,
			     uint * width, uint * height);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * FontSet Types.
 */
typedef enum {
    FONTSET_NRM,
    FONTSET_VWM,
    FONTSET_FWM,
    FONTSET_FWMI,
    FONTSET_FWB
} __attribute__ ((packed)) DVIS_FONTSET_t;

/*!
 * Font.
 */
typedef struct {
#if DVIS_USE_XLIB
    XFontStruct * xfs;
#else
    int dummy;
#endif
} VFont;

/*! Fit font. */
int
dvis_fitFont(DVIS_FONTSET_t fontset, int nchars, int w, int h, CLay clay,
	    VFont * font_ret);


inline int
dvis_isFittedFont(VFont font)
{
#if DVIS_USE_XLIB
    return font.xfs != 0;
#else
    return 0;
#endif
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Print face.
 */
void gfont_print(GFont * gfont);

/*!
 * Load fonts.
 */
void dvis_loadFonts(void);

/*!
 * Free fonts.
 */
void dvis_freeFonts(void);

/*!
 * Pick a set of fonts.
 */
void dvis_pickFontset(DVIS_FONTSET_t fontset);
