// #include "dvis/frontend.hpp"
#include "font.hpp"
#include "binlog.h"
#include "pathops.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static DVIS_FONTSET_t g_curr_fontset = FONTSET_NRM;

#ifdef HAVE_LIBFREETYPE
static FT_Library g_ft_library;
int g_ft_point_size_bits = -1;
static int g_ft_point_size = -1;

/*!
 * Lookup Freetype2 \em Error \em Message as a statically allocated C
 * string.
 *
 * \return error message C string
 */
const char * FT_strerror(FT_Error error)
{
    switch (error) {
#  include <freetype/fterrdef.h>
    }
    return 0;
}
#endif

GFontSet g_serif;
GFontSet g_sans;
GFontSet g_fixed;
GFontSet g_vera;

/* ---------------------------- Group Separator ---------------------------- */

static void
gglyph_init(GGlyph * gglyph)
{
    gglyph->gidx = 0;
#ifdef HAVE_LIBFREETYPE
    gglyph->glyph = 0;
#endif
#if DVIS_USE_GL
    gglyph->texID = 0;
#endif
    gglyph->met_cnt = 0;
    gglyph->tex_cnt = 0;

    gglyph->w_binlog = 0;
    gglyph->h_binlog = 0;
}

#if DVIS_USE_GL
/*!
 * Perform the OpenGL State for loading of \p gglyph.
 */
static void
gglyph_load_prepare_OpenGL_state(GGlyph * gglyph, bool mipmap_flag)
{
    const GLenum tex_target = opengl_has_ARB_EXT_NV_texture_rectangle() ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;
    if (opengl_has_ARB_EXT_NV_texture_rectangle()) { mipmap_flag = false; }

    glBindTexture(tex_target, gglyph->texID); /* select our current texture */
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(tex_target, GL_TEXTURE_MIN_FILTER, mipmap_flag ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
    glTexParameterf(tex_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
#endif

/*!
 * Extend texture data in \p datI of width \p w and height \p h
 * OpenGL-compatible power-of-two texture.
 *
 * Must use calloc() to blacken the texture data, in order to avoid
 * texture edge artifacts.
 */
static uchar *
extendImage(const uchar * datI, uint w, uint h, uint wExt, uint hExt)
{
    uchar * datO = NULL;
    const size_t numT = (size_t)wExt * (size_t)hExt; /* number of texture pixels */
    datO = (uchar*)calloc(numT * 1, 1);
    for (uint y = 0; y < h; y++) {
        for (uint x = 0; x < w; x++) {
            datO[x + y * wExt] = datI[x + y * w];
        }
    }
    return datO;
}

static void
FT_BitmapGlyph_print(const FT_BitmapGlyph bmp_glyph)
{
    lprintf("left:%d top:%d width:%d rows:%d\n",
            bmp_glyph->left, bmp_glyph->top,
            bmp_glyph->bitmap.width, bmp_glyph->bitmap.rows);
}

static void
gglyph_tex(GGlyph * gglyph, bool mipmap_flag)
{
#if DVIS_USE_GL
    glGenTextures(1, &gglyph->texID); /* allocate a texture name */
    if (gglyph->texID == 0) { PWARN("glGenTextures gave 0 texID\n"); }
#endif

#ifdef HAVE_LIBFREETYPE
    FT_Error error;
    FT_Vector pen = { 0, 0 };
    if ((error = FT_Glyph_To_Bitmap(&gglyph->glyph, FT_RENDER_MODE_NORMAL,
                                    &pen, 0))) {
        leprintf("Could not render glyph. error: %d\n", error);
        return;
    }
#endif

    if (gglyph->glyph) {
#if DVIS_USE_GL

        const FT_Glyph glyph = gglyph->glyph;
        const FT_BitmapGlyph bmp_glyph = (const FT_BitmapGlyph)glyph;
        const FT_Bitmap bmp = bmp_glyph->bitmap;
        const uint width = bmp.width, height = bmp.rows;

        if (FALSE) { FT_BitmapGlyph_print(bmp_glyph); }

        gglyph->w_binlog = int_binlog(width);
        gglyph->h_binlog = int_binlog(height);

        uint wExt = width, hExt = height;
        if (opengl_has_ARB_EXT_NV_texture_rectangle()) {
        } else {
            wExt = (1 << gglyph->w_binlog); /* extend width to nearest power */
            hExt = (1 << gglyph->h_binlog); /* extend height to nearest power */
        }

        uchar * datT = NULL;		/* temporary buffer */
        uchar * datGL = NULL;          /* OpenGL data pointer. default data pointer to zero */

        if (width  == wExt &&         /* if no extension of width */
            height == hExt) {	/* nor height is needed */
            datGL = bmp.buffer;	/* use glyph bitmap directly */
        } else {
            datGL = datT = extendImage(bmp.buffer,
                                       width, height,
                                       wExt, hExt); /* convert, set */
        }

        gglyph_load_prepare_OpenGL_state(gglyph, mipmap_flag);

        const GLenum tex_target = opengl_has_ARB_EXT_NV_texture_rectangle() ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;
        if (opengl_has_ARB_EXT_NV_texture_rectangle()) { mipmap_flag = false; }

        /* load into an OpenGL texture */
        if (mipmap_flag) {
            gluBuild2DMipmaps(tex_target, GL_ALPHA, wExt, hExt,
                              GL_ALPHA, GL_UNSIGNED_BYTE, datGL);
        } else {
            glTexImage2D(tex_target,
                         0, GL_ALPHA, wExt, hExt, 0,
                         GL_ALPHA, GL_UNSIGNED_BYTE, datGL);
        }

        if (datT) { free(datT); }
#endif
    }
}

static void gglyph_untex(GGlyph * gglyph)
{
#if DVIS_USE_GL
    if (gglyph->texID) {
        glDeleteTextures(1, &gglyph->texID);
        gglyph->texID = 0;
    }
#endif
}

static void gglyph_unload(GGlyph * gglyph)
{
#ifdef HAVE_LIBFREETYPE
    if (gglyph->glyph) {
        FT_Done_Glyph(gglyph->glyph); gglyph->glyph = 0;
    }
#endif
}

static void gglyph_clear(GGlyph * gglyph)
{
    gglyph_unload(gglyph);
#if DVIS_USE_GL
    gglyph_untex(gglyph);
#endif
}

static int gglyph_reload(const GFont * gfont, GGlyph * gglyph, ulong val)
{
#ifdef HAVE_LIBFREETYPE
    gglyph_unload(gglyph);

    FT_Error error;

    gglyph->gidx = FT_Get_Char_Index(gfont->face, val);

    if ((error = FT_Load_Glyph(gfont->face, gglyph->gidx, FT_LOAD_DEFAULT))) {
        leprintf("Could not FT_Load_Glyph. error: %d\n", error);
        return -1;
    }

    if ((error = FT_Get_Glyph(gfont->face->glyph, &gglyph->glyph))) {
        leprintf("Could not FT_Get_Glyph. error: %d\n", error);
        return -1;
    }

    gglyph->gmet = gfont->face->glyph->metrics;

#endif
    return 0;
}

static void
gglyph_load(GFont * gfont, GGlyph * gglyph, ulong val)
{
#ifdef HAVE_LIBFREETYPE
    gglyph->gidx = FT_Get_Char_Index(gfont->face, val);
    gglyph->glyph = 0;		/* zero handle */
#endif
    gglyph_reload(gfont, gglyph, val);
}

const GGlyph *
gglyph_lookup(TSTYLE_t tstyle, ulong val)
{
    const GFont * gfont = gfont_lookup(tstyle);
    if (gfont) {
        return &gfont->greg[val];
    } else {
        PWARN("Could not lookup font\n");
        return NULL;
    }
}

int gglyph_lookup_Bitmap_W_H(const GGlyph * gglyph,
			     uint * width, uint * height)
{
#ifndef NDEBUG
    if (gglyph == 0) { PERR("gglyph NULL\n"); return 0; }
#endif
    int ret = 0;
    const FT_Glyph glyph = gglyph->glyph;
    const FT_BitmapGlyph bmp_glyph = (const FT_BitmapGlyph)glyph;
    const FT_Bitmap bmp = bmp_glyph->bitmap;
    *width = bmp.width;
    *height = bmp.rows;
    ret = 1;
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Locate file filename on filesystem.
 *
 * Called as const char * face_path = locate_file("times.ttf");
 */
const char * locate_file(const char * filename)
{
    struct stat stat_ret;

    if (stat(filename, &stat_ret) == 0) {
        return strdup(filename);
    } else {
        lperror("stat");
    }

    return strdup(filename);
}

static void
gfont_init(GFont * gfont, const char * face_filename)
{
    char * face_path = cstr_pathadd_atHOME("pnw/fonts", face_filename);
    /* PTEST("face_path: %s\n", face_path); */

    FT_Error error = FT_New_Face(g_ft_library, face_path,
                                 0, &gfont->face);
    if (error) {
        if (error == FT_Err_Unknown_File_Format) {
            leprintf("Font format is not supported\n");
        } else {
            leprintf("Could not load face from file %s\n", face_filename);
        }
    } else {
        FT_Select_Charmap(gfont->face, FT_ENCODING_UNICODE);
        uint bits = 6;	/**< Means 2^7 point size */
        g_ft_point_size_bits = bits + 6;
        g_ft_point_size = (1 << g_ft_point_size_bits);
        if ((error = FT_Set_Char_Size(gfont->face,
                                      0L,
                                      g_ft_point_size,
                                      0,
                                      0))) {
            leprintf("Could not set face point size to %d\n", g_ft_point_size);
        }
        if (0) { gfont_print(gfont); }
    }

    free(face_path);

    /* allocate pointers to glyphs */
    gfont->gregN = GFONT_MAX;
    gfont->greg = malloc(gfont->gregN * sizeof(GGlyph));

    /* init glyphs */
    for (size_t i = 0; i < gfont->gregN; i++) {
        gglyph_init(&gfont->greg[i]);
    }
}

static void
gfont_clear(GFont * gfont)
{
    /* clear glyphs */
    for (size_t i = 0; i < gfont->gregN; i++) {
        if (gfont->greg[i].glyph) {
            gglyph_clear(&gfont->greg[i]);
        }
    }

    free(gfont->greg);
    gfont->greg = NULL;
    gfont->gregN = 0;
    FT_Done_Face(gfont->face);
}

GFont *
gfont_lookup(TSTYLE_t tstyle)
{
    if (tstyle == 0) {		/* most common case first */
        if (tstyle & TSTYLE_FIXED) {
            return &g_fixed.r;
        } else if (tstyle & TSTYLE_SERIF) {
            return &g_serif.r;
        } else {
            return &g_sans.r;
        }
    } else if (tstyle & TSTYLE_BOLD && /* bold */
               !(tstyle & TSTYLE_ITALIC)) { /* not italic */
        if (tstyle & TSTYLE_FIXED) {
            return &g_fixed.b;
        } else if (tstyle & TSTYLE_SERIF) {
            return &g_serif.b;
        } else {
            return &g_sans.b;
        }
    } else if (!(tstyle & TSTYLE_BOLD) && /* not bold */
               tstyle & TSTYLE_ITALIC) { /* italic */
        if (tstyle & TSTYLE_FIXED) {
            return &g_fixed.b;
        } else if (tstyle & TSTYLE_SERIF) {
            return &g_serif.i;
        } else {
            return &g_sans.i;
        }
    } else if (tstyle & (TSTYLE_BOLD | TSTYLE_ITALIC)) {	/* bold and italic */
        if (tstyle & TSTYLE_FIXED) {
            return &g_fixed.b;
        } else if (tstyle & TSTYLE_SERIF) {
            return &g_serif.bi;
        } else {
            return &g_sans.bi;
        }
        return &g_serif.bi;
    } else {			/* unknown */
        if (tstyle & TSTYLE_FIXED) {
            return &g_fixed.b;
        } else if (tstyle & TSTYLE_SERIF) {
            return &g_serif.r;    /* default to regular */
        } else {
            return &g_sans.r;    /* default to regular */
        }
    }
}

static void
gfontset_init(GFontSet * gfontset,
	      const char * face_filename_r,
	      const char * face_filename_b,
	      const char * face_filename_i,
	      const char * face_filename_bi)
{
    gfont_init(&gfontset->r, face_filename_r);
    gfont_init(&gfontset->b, face_filename_b);
    gfont_init(&gfontset->i, face_filename_i);
    gfont_init(&gfontset->bi, face_filename_bi);
}

static void
gfontset_clear(GFontSet * gfontset)
{
    gfont_clear(&gfontset->r);
    gfont_clear(&gfontset->b);
    gfont_clear(&gfontset->i);
    gfont_clear(&gfontset->bi);
}

void gfont_loadMet(TSTYLE_t tstyle, ulong val)
{
    GFont * gfont = gfont_lookup(tstyle);
    if (gfont->greg[val].glyph == 0) { /* if not yet loaded (by other vGlyph) */
        gglyph_load(gfont, &gfont->greg[val], val); /* load it */
    }
    gfont->greg[val].met_cnt++;
}

void gfont_unloadMet(TSTYLE_t tstyle, ulong val)
{
    GFont * gfont = gfont_lookup(tstyle);
    if (gfont->greg[val].met_cnt == 0) {
        PERR("met_cnt already 0\n");
    } else {
        gfont->greg[val].met_cnt--;
    }

    if (gfont->greg[val].met_cnt == 0) { /* if not used */
        gglyph_clear(&gfont->greg[val]);
    }
}

void gfont_loadTex(TSTYLE_t tstyle, ulong val, bool mipmap_flag)
{
    GFont * gfont = gfont_lookup(tstyle);
    if (gfont->greg[val].tex_cnt == 0) { /* if not yet textured */
        gglyph_tex(&gfont->greg[val], mipmap_flag); /* load it */
    }
    gfont->greg[val].tex_cnt++;
}

void gfont_unloadTex(TSTYLE_t tstyle, ulong val)
{
    GFont * gfont = gfont_lookup(tstyle);
    if (gfont->greg[val].tex_cnt == 0) {
        PERR("tex_cnt already 0\n");
    } else {
        gfont->greg[val].tex_cnt--;
    }

    if (gfont->greg[val].tex_cnt == 0) { /* if not used */
        gglyph_untex(&gfont->greg[val]);
    }
}

/* ---------------------------- Group Separator ---------------------------- */

int
dvis_fitFont(DVIS_FONTSET_t fontset, int nchars, int w, int h, CLay clay,
	    VFont * font_ret)
{
#if DVIS_USE_XLIB
    VFont ret;
    switch (fontset) {
    case FONTSET_NRM:
        ret.xfs = g_xdpy.nrm_fnt;
        break;
    case FONTSET_VWM:
        g_curr_fonts = vwm_fonts;
        g_curr_fonts_N = VWM_NUM;
        ret.xfs = x11_fitFont(g_curr_fonts, g_curr_fonts_N, nchars, w, h, clay);
        break;
    case FONTSET_FWM:
        g_curr_fonts = fwm_fonts;
        g_curr_fonts_N = FWM_NUM;
        ret.xfs = x11_fitFont(g_curr_fonts, g_curr_fonts_N, nchars, w, h, clay);
        break;
    case FONTSET_FWMI:
        g_curr_fonts = fwmi_fonts;
        g_curr_fonts_N = FWMI_NUM;
        ret.xfs = x11_fitFont(g_curr_fonts, g_curr_fonts_N, nchars, w, h, clay);
        break;
    case FONTSET_FWB:
        g_curr_fonts = fwb_fonts;
        g_curr_fonts_N = FWB_NUM;
        ret.xfs = x11_fitFont(g_curr_fonts, g_curr_fonts_N, nchars, w, h, clay);
        break;
    default:
        ret.xfs = g_xdpy.nrm_fnt;	/* default to normal font */
        break;
    }
    *font_ret = ret;
    return font_ret->xfs != 0;
#else
    return 0;
#endif
}

/*!
 * Lookup name of \p encoding.
 *
 * \return the looked-up name or NULL if encoding was unknown.
 */
const char * FT_Encoding_getName(FT_Encoding encoding)
{
    const char * str = NULL;
    switch (encoding) {
    case FT_ENCODING_NONE: str = "None"; break;
    case FT_ENCODING_MS_SYMBOL: str = "Microsoft Symbol"; break;
    case FT_ENCODING_UNICODE: str = "Unicode"; break;

    case FT_ENCODING_SJIS: str = "Japanese SJIS"; break;
    case FT_ENCODING_GB2312: str = "Simplified Chinese"; break;
    case FT_ENCODING_BIG5: str = "Traditional Chinese"; break;
    case FT_ENCODING_WANSUNG: str = "Korean Wansung"; break;
    case FT_ENCODING_JOHAB:
        str = "Korean standard character set (KS C-5601-1992)";
        break;

    case FT_ENCODING_ADOBE_STANDARD: str = "Adobe Standard"; break;
    case FT_ENCODING_ADOBE_EXPERT: str = "Adobe Expert"; break;
    case FT_ENCODING_ADOBE_CUSTOM: str = "Adobe Custom"; break;

    case FT_ENCODING_ADOBE_LATIN_1: str = "Adobe Latin-1"; break;

    case FT_ENCODING_OLD_LATIN_2: str = "Old Latin-2 (deprecated)"; break;

    case FT_ENCODING_APPLE_ROMAN: str = "Apple roman"; break;
    default:
        PWARN("Unknown Encoding: %d\n", encoding);
        str = "Unknown";
        break;
    }
    return str;
}

void
gfont_print(GFont * gfont)
{
#ifdef HAVE_LIBFREETYPE
    printf("# Family: %s\n", gfont->face->family_name);
    printf("# Style: %s\n", gfont->face->style_name);

    printf("- Face Kerning: %d\n", (int)FT_HAS_KERNING(gfont->face));

    printf("- Unscaled Global Glyph Metrics\n"
           "  - Units per EM: %d\n"
           "  - Ascender: %d\n"
           "  - Descender: %d\n"
           "  - Height: %d\n"
           "  - Max Advance Width: %d\n"
           "  - Max Advance Height: %d\n"
           "  - Underline Position: %d\n"
           "  - Underline Thickness: %d\n"
           "  - Charmap Encoding: %s\n",
           gfont->face->units_per_EM,
           gfont->face->ascender,
           gfont->face->descender,
           gfont->face->height,

           gfont->face->max_advance_width,
           gfont->face->max_advance_height,

           gfont->face->underline_position,
           gfont->face->underline_thickness,
           FT_Encoding_getName(gfont->face->charmap->encoding));

    printf("- Scaled Global Glyph Metrics\n"
           "  - Ascender: %ld\n"
           "  - Descender: %ld\n"
           "  - Height: %ld\n"
           "  - Max Advance: %ld\n",
           gfont->face->size->metrics.ascender,
           gfont->face->size->metrics.descender,
           gfont->face->size->metrics.height,
           gfont->face->size->metrics.max_advance);

    printf("\n");
#endif
}

/* ---------------------------- Group Separator ---------------------------- */

void
dvis_loadFonts(void)
{
#ifdef HAVE_LIBFREETYPE
    FT_Error error = FT_Init_FreeType(&g_ft_library);
    if (error) {
        leprintf("Could not initialize FreeType2\n");
    }

    if (g_ft_library) {
        gfontset_init(&g_serif,
                      "times.ttf", "timesbd.ttf", "timesi.ttf", "timesbi.ttf");
        gfontset_init(&g_sans,
                      "arial.ttf", "arialbd.ttf", "ariali.ttf", "arialbi.ttf");
        gfontset_init(&g_fixed,
                      "cour.ttf", "courbd.ttf", "couri.ttf", "courbi.ttf");
        gfontset_init(&g_vera,
                      "Vera.ttf", "VeraBd.ttf", "VeraIt.ttf", "VeraBI.ttf");
    }

#endif
#if DVIS_USE_XLIB
    vwm_fonts = x11_loadFonts(&g_xdpy, vwm_names, VWM_NUM);
    vwb_fonts = x11_loadFonts(&g_xdpy, vwb_names, VWB_NUM);
    fwm_fonts = x11_loadFonts(&g_xdpy, fwm_names, FWM_NUM);
    fwmi_fonts = x11_loadFonts(&g_xdpy, fwmi_names, FWMI_NUM);
    fwb_fonts = x11_loadFonts(&g_xdpy, fwb_names, FWB_NUM);
    g_fixed_font = x11_loadFont(&g_xdpy, "9x15");
#endif
}

void
dvis_freeFonts(void)
{
#ifdef HAVE_LIBFREETYPE
    gfontset_clear(&g_serif);
    gfontset_clear(&g_sans);
    gfontset_clear(&g_fixed);
    gfontset_clear(&g_vera);
    FT_Done_FreeType(g_ft_library);
#endif
#if DVIS_USE_XLIB
    x11_delFonts(&g_xdpy, vwm_fonts, VWM_NUM);
    x11_delFonts(&g_xdpy, vwb_fonts, VWB_NUM);
    x11_delFonts(&g_xdpy, fwm_fonts, FWM_NUM);
    x11_delFonts(&g_xdpy, fwmi_fonts, FWMI_NUM);
    x11_delFonts(&g_xdpy, fwb_fonts, FWB_NUM);
    x11_freeFont(&g_xdpy, g_fixed_font);
#endif
}

void
dvis_pickFontset(DVIS_FONTSET_t fontset)
{
    g_curr_fontset = fontset;
#if DVIS_USE_XLIB
    switch (fontset) {
    case FONTSET_NRM:
        g_curr_fonts = &g_xdpy.nrm_fnt;
        x11_setFont(&g_xdpy, &g_xanim.win, g_xdpy.nrm_fnt);
        break;
    case FONTSET_VWM:
        g_curr_fonts = vwm_fonts;
        g_curr_fonts_N = VWM_NUM;
        break;
    case FONTSET_FWM:
        g_curr_fonts = fwm_fonts;
        g_curr_fonts_N = FWM_NUM;
        break;
    case FONTSET_FWMI:
        g_curr_fonts = fwmi_fonts;
        g_curr_fonts_N = FWMI_NUM;
        break;
    case FONTSET_FWB:
        g_curr_fonts = fwb_fonts;
        g_curr_fonts_N = FWB_NUM;
        break;
    default:
        break;
    }
    if (g_curr_fonts) {
        g_curr_font = g_curr_fonts[0];
    }
#endif
}
