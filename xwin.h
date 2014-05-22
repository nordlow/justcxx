/*!
 * \file xwin.h
 * \brief Various XWindows utilities.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"
#include "pixels.h"
#include "color.hpp"
#include "color_constants.h"
#include "ximg.h"
#include "CLAY.hpp"

/* Definitions for the X window system likely to be used by applications */
#include <X11/X.h>

/* Header definition and support file for the C subroutine
 * interface library (Xlib) to the X Window System Protocol (V11).
 * Structures and symbols starting with "_" are private to the library.
 */
#include <X11/Xlib.h>

/* struct: XSizeHints, XWMHints, XTextProperty, XVisualInfo, XStandardColormap,
 * XSetStandardProperties
 * functions: XCreateImage
 */
#include <X11/Xutil.h>

#include <X11/Intrinsic.h>

/* This file is a component of an X Window System-specific implementation
 * of Xcms based on the TekColor Color Management System.
 * structs: XcmsRGB, XcmsColor, XcmsPerScrnInfo
 */
#include <X11/Xcms.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! Display */
typedef struct
{
  char        *name;            /**< Name */
  Display     *dpy;             /**< Display. */
  Visual      *vis;             /**< Visual. */
  Colormap     cmap;            /**< Colormap. */
  Window       rwin;            /**< Root Window. */
  Screen      *scr;             /**< Screen */

  int          scr_num;         /**< Number of Screens. */
  int          scr_depth;       /**< Screen Depth. */
  int          scr_bpp;         /**< Screen Bits Per Pixel (X -fbbpp). */
  int          scr_depthsN;     /**< Number of Screens Depths. */
  int         *scr_depth_list;  /**< Array of Screen Depths. */

  XFontStruct *hdr_fnt;         /**< Header Font. */
  XFontStruct *sml_fnt;         /**< Small Font. */
  XFontStruct *nrm_fnt;         /**< Normal Font. */
  XFontStruct *bld_fnt;         /**< Bold Font. */
  XFontStruct *tlt_fnt;         /**< Teletype Font. */

  PConv        pconv;           /**< Pixel Conversion. */

  color_t      color;           /**< Color. */

  color_t      bg;              /**< Background Color. */
  color_t      fg;              /**< Foreground Color. */
  color_t      upNW;            /**< Up North West */
  color_t      upSE;            /**< Up South East */

  /** For completion event handling of shared memory-stuff. */
  XExtCodes *ext_info;
} XDpy;

/* ---------------------------- Group Separator ---------------------------- */

/*! Window. */
typedef struct
{
  Window win;                   /**< Window */
  GC gc;			/**< Graphics Context */
  XFontStruct *current_fnt;     /**< Current Font. */
  long event_mask;              /**< Event Mask */
  XSetWindowAttributes *attr;   /**< Attributes. */
  XSizeHints size_hints;        /**< Size Hints. */
  XWMHints *wm_hints;           /**< Window Manager (WM) Hints */
  XVisualInfo *visualinfo;    /**< Visualinfo */
} XWin;

int
x11_openwin_visual(const XDpy * dpy, XWin * win,
                   uint w, uint h, const char *name,
                   XVisualInfo * visualinfo);

int
x11_openwin_GL(const XDpy * dpy, XWin * win,
               uint w, uint h, const char *name,
               XVisualInfo * visualinfo);

/*! Open Window. */
int x11_openwin(const XDpy * dpy, XWin * win,
                uint w, uint h, const char *name);

/*! Close Window. */
int x11_closewin(const XDpy * dpy, XWin * win);

/*! Clear Window. */
void x11_clearArea(const XDpy * dpy, XWin * win,
                   int x, int y, uint w, uint h, int exposures);

/*! Clear Window. */
void x11_clearwin(const XDpy * dpy, XWin * win);

/*! Get window geometry.
 * \param[out] w points to the place where the width is written.
 * \param[out] h points to the place where the height is written.
 */
static inline void
x11_getGeom(const XDpy * dpy, XWin * win,
            int *x, int *y, uint *w, uint *h)
{
  Window root;
  uint border_width;
  uint depth;

  XGetGeometry(dpy->dpy, win->win, &root, x, y, w, h, &border_width, &depth);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Lookup a string name for the type of the event. */
const char *x11_event2str(const XEvent * eve);

/*! Query keysym pressing state.
 * \return > 0 if keysym is currently being pressed.
 */
int x11_qKeyP(Display * dpy, KeySym keysym);

/* Convert key_event to a string, placing at most len bytes of them in buf . */
void x11_key2str(XKeyEvent * key_event, char * buf, int len);

/*! Look at X11 keystate to detect repeat mode.
 * While X11 says the key is actually held down, we'll ignore
 * KeyRelease/KeyPress pairs.
 * \return 1 if keycode is a keyrepeat, 0 otherwise, -1 if an error occurred.
 */
int x11_isKeyCodeRepeat(Display * dpy, KeyCode keycode);

/* ---------------------------- Group Separator ---------------------------- */

/*! Lookup the pixel Conversion Structure of display \p dpy. */
PConv x11_gpconv(Display * dpy);

/* ---------------------------- Group Separator ---------------------------- */

/*! RGB-triplet to pixel.
 * The 8 least signficant bits of the red, green and blue component are used.
 */
long x11_rgb2pix(Display * dpy, int r, int g, int b);

/*! (ASCII) Name to pixel. */
long x11_a2pix(Display * dpy, char *name);

/*! Name to color. */
color_t x11_a2col(const XDpy * dpy, const char *name);

/* ---------------------------- Group Separator ---------------------------- */

/*! \name Setting the Foreground and Background Color.
 * These routines use its own color to pixel transformation making it very
 * fast.
 */

/*{*/

/*! Set Foreground. */
static inline void
x11_setFG(const XDpy * dpy, XWin * win, color_t color)
{
  XSetForeground(dpy->dpy, win->gc, pix_get(dpy->pconv, color));
}

/*! Set Background. */
static inline void
x11_setBG(const XDpy * dpy, XWin * win, color_t color)
{
  XSetBackground(dpy->dpy, win->gc, pix_get(dpy->pconv, color));
}

/*}*/

/* ---------------------------- Group Separator ---------------------------- */

static inline void
x11_setXor(Display * dpy, GC gc)
{
  XSetFunction(dpy, gc, GXxor);
}

static inline void
x11_setCopy(Display * dpy, GC gc)
{
  XSetFunction(dpy, gc, GXcopy);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Handling Fonts.
 */

/*{*/

/*! Load a Font. */
XFontStruct *x11_loadFont(const XDpy * dpy, const char *name);

/*! Free a Font. */
static inline void
x11_freeFont(const XDpy * dpy, XFontStruct * fnt)
{
  XFreeFont(dpy->dpy, fnt);
}

/*! Set Font. */
static inline void
x11_setFont(const XDpy * dpy, XWin * win, XFontStruct * font)
{
  if (win->current_fnt != font) {
    win->current_fnt = font;
    XSetFont(dpy->dpy, win->gc, font->fid);
  }
}

/*! Load a Set of Fonts. */
XFontStruct **
x11_loadFonts(const XDpy * dpy, const char ** fwm_names, int num);

/*! Delete a set of fonts. */
void x11_delFonts(const XDpy * dpy, XFontStruct ** xfs, int num);

/*! Maximum Width of a Font Character. */
static inline int
x11_maxw(const XFontStruct * xfs)
{
  return xfs->max_bounds.width;
}

/*! Maximum Height of a Font Character. */
static inline int
x11_maxh(const XFontStruct * xfs)
{
  return (xfs->max_bounds.ascent +
          xfs->max_bounds.descent);
}

/*!
 * Select a suitable font that fits inside the box (\p w, \p h).
 *
 * \param[out] fonts is the fonts to choose from.
 * \param[in] num is the number of fonts.
 * \param[in] is the number of characters to fit.
 * \param[in] w is the width of the box.
 * \param[in] h is the height of the box.
 * \param[in] clay is the type of fitting to use.
 *
 * \return the first (largest) font that fits, or 0 if none was found.
 */
XFontStruct * x11_fitFont(XFontStruct ** fonts, int num, int nchars,
                          int w, int h, CLay clay);

/*}*/

/* ---------------------------- Group Separator ---------------------------- */

/* Lines and GCs */

void x11_setLineAttr(const XDpy * dpy, XWin * win,
                     uint line_width,
                     int line_style, int cap_style, int join_style);

void x11_setDefaultLn(const XDpy * dpy, XWin * win);

void x11_setOnOffDash(const XDpy * dpy, XWin * win);

/* ---------------------------- Group Separator ---------------------------- */

/* display */

int x11_hasDepth(const XDpy * dpy, int depth);

/*!
 * Open display.
 */
int x11_openDpy(XDpy * dpy);

/*!
 * Close display.
 */
int x11_closeDpy(const XDpy * dpy);

/* ---------------------------- Group Separator ---------------------------- */

long x11_getStandardEventMask(void);

void x11_setStandardEventMask(const XDpy * dpy, XWin * win);

/*!
 * Print event type.
 */
void x11_printEvent(const XEvent * eve);

/*!
 * Pop Events masked by \p mask from event queue.
 *
 * \param[out] event_ret contains upon return the last event that was skipped.
 * \return number of motions that were skipped.
 */
int x11_skipEvents(Display * dpy, Window win, long mask, XEvent * event_ret);

/*!
 * Pop All Motion Events (masked PointerMotionMask) and if any store the last.
 *
 * \param[out] event_ret contains upon return the last event
 *             (XMotionEvent) that was skipped.
 * \return number of motions that were skipped.
 */
int x11_skipMotions(XDpy * dpy, XWin * win, XEvent * event_ret);

/*!
 * Print Information.
 */
void x11_printInfo(XDpy * dpy);

/* ---------------------------- Group Separator ---------------------------- */

/* X Double Buffer Extension */
#include <X11/extensions/Xdbe.h>

/*! Double Buffered (DB) Animated Window. */
typedef struct
{
  XWin win;                     /**< Window. */
  XdbeBackBuffer xback;         /**< Backbuffer. */
  XdbeSwapInfo info;            /**< Swapinfo. */
} XAnim;

/*! Open DB-Window. */
int x11_anim_open(const XDpy * dpy, XAnim * xanim,
                  uint w, uint h, const char *name);

/*! Close DB-Window. */
int x11_anim_close(const XDpy * dpy, XAnim * xanim);

/*! Close the back buffer. */
void x11_anim_clearback(const XDpy * dpy, XAnim * xanim);

/*! Copy front to back buffer. */
void x11_anim_front2back(const XDpy * dpy, XAnim * xanim);

/*! Copy back to front buffer. */
void x11_anim_back2front(const XDpy * dpy, XAnim * xanim);

/*! Animate by swapping buffers. */
void x11_anim_showback(const XDpy * dpy, XAnim * xanim);

void x11_anim_copyswap(const XDpy * dpy, XAnim * xanim);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Pixmap
 */
typedef struct
{
  Pixmap pxm;
  GC gc;			/* graphics context */
  XFontStruct *current_fnt;
} XMap;

/*! Create XMap */
void x11_mkMap(const XDpy * dpy, const XWin * win, XMap * map,
               int w, int h);

/*! Blacken XMap */
static inline void
x11_blackenMap(const XDpy * dpy, XWin * win, XMap * map,
               uint sw, uint sh)
{
  x11_setFG(dpy, win, COLOR_BLACK);
  XFillRectangle(dpy->dpy, map->pxm, win->gc, 0, 0, sw, sh);
}

/*! Clear XMap */
void x11_clearMap(const XDpy * dpy, XMap * map);

/*! Draw XMap */
void x11_dMap(const XDpy * dpy, const XAnim * xanim, const XMap * map,
              int src_x, int src_y, int w, int h, int dest_x, int dest_y);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Waterfall.
 */
typedef struct
{
  XMap map;
  ushort sw;		/**< Screen width. */
  ushort sh;		/**< Screen height. */
  ulong rw;		/**< Real fall width. */
  ulong rh;		/**< Read fall height. */
} XFall;

/*! Create Waterfall and blacken it. */
static inline void
x11_mkFall(const XDpy * dpy, XWin * win, XFall * fall,
           ushort sw, ushort sh,
           ulong rw, ulong rh)
{
  x11_mkMap(dpy, win, &fall->map, sw, sh);
  fall->sw = sw;
  fall->sh = sh;
  fall->rw = rw;
  fall->rh = rh;
  x11_blackenMap(dpy, win, &fall->map, fall->sw, fall->sh);
}

/*! Clear Waterfall. */
static inline void
x11_clearFall(const XDpy * dpy, XFall * fall)
{
  x11_clearMap(dpy, &fall->map);
}

/*! Clear Waterfall. */
static inline void
x11_rszFall(const XDpy * dpy, XWin * win, XFall * fall,
            ushort sw, ushort sh)
{
  if (fall->sw != sw ||
      fall->sh != sh) {
    x11_clearMap(dpy, &fall->map);
    x11_mkFall(dpy, win, fall, sw, sh, sw, sh);
  }
}

/*! Draw Waterfall */
static inline void
x11_dWFall(const XDpy * dpy, const XAnim * xanim,
           const XFall * fall, int x, int y)
{
  /* draw main pixmap */
  x11_dMap(dpy, xanim, &fall->map, 0, 0, fall->sw, fall->sh,
           x, y);
  /* draw other stuff on top of pixamp */
}

/*! Append a Line to the Waterfall. */
static inline void
x11_appFall(const XDpy * dpy, XAnim * xanim, XFall * fall,
            const color_t * ldat, ushort lw)
{
  uint i;

  /* scroll up one line */
  XCopyArea(dpy->dpy, fall->map.pxm, fall->map.pxm, xanim->win.gc,
            0, 1, fall->sw, fall->sh - 1,
            0, 0);

  /* draw new line */
  for (i = 0; i < fall->sw; i++) {
    int x = i;
    int y = fall->sh - 1;
    x11_setFG(dpy, &xanim->win, ldat[i * lw / fall->sw]);
    XDrawPoint(dpy->dpy, fall->map.pxm, xanim->win.gc, x, y);
  }
}

/*!
 * Test function showing how to create a timeout with Xt.
 */
void x11_timeouttest(void);

#ifdef __cplusplus
}
#endif
