#include "xwin.h"
#include "pnmio.h"
#include "fft.h"
#include "convert.h"
#include "fractals.h"
#include "color.h"
#include "color_constants.hpp"
#include "utils.h"
#include "extremes.h"
#include "endian_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* needed by MIT-SHM */
#ifdef PNW_USE_MITSHM
#  define __USE_XOPEN		/* prevent error message */
#  include <sys/ipc.h>
#  include <sys/shm.h>
#endif

/* ========================================================================= */

/* events */

/* 10.1 Event Types

   - Event Category --- Event Type
#############################

   - Keyboard events - KeyPress, KeyRelease

   - Pointer events - ButtonPress, ButtonRelease, MotionNotify

   - Window crossing events - EnterNotify, LeaveNotify

   - Input focus events - FocusIn, FocusOut

   - Keymap state notification event - KeymapNotify

   - Exposure events - Expose, GraphicsExpose, NoExpose
   A naive client application usually redraws the entire window.
   A more sophisticated client application redraws only the
   exposed region.

   - Structure control events - CirculateRequest, ConfigureRequest,
   MapRequest, ResizeRequest

   - Window state notification events - CirculateNotify, ConfigureNotify,
   CreateNotify, DestroyNotify, GravityNotify, MapNotify, MappingNotify,
   ReparentNotify, UnmapNotify, VisibilityNotify

   - Colormap state notification event - ColormapNotify

   - Client communication events - ClientMessage, PropertyNotify,
   SelectionClear, SelectionNotify, SelectionRequest
*/

static const char *x_event_names[] = {
  "KeyPress", "KeyRelease",
  "ButtonPress", "ButtonRelease", "MotionNotify",
  "EnterNotify", "LeaveNotify",
  "FocusIn", "FocusOut", "KeymapNotify",
  "Expose", "GraphicsExpose", "NoExpose", "VisibilityNotify",
  "CreateNotify", "DestroyNotify", "UnmapNotify", "MapNotify", "MapRequest",
  "ReparentNotify", "ConfigureNotify", "ConfigureRequest",
  "GravityNotify", "ResizeRequest", "CirculateNotify", "CirculateRequest",
  "PropertyNotify", "SelectionClear", "SelectionRequest", "SelectionNotify",
  "ColormapNotify", "ClientMessage", "MappingNotify", "LASTEvent"
};

/* ---------------------------- Group Separator ---------------------------- */

PConv
x11_gpconv(Display * dpy)
{
  PConv pconv;
  int show = FALSE;

  pconv.fmt = PXF_UNKNOWN;

  int scr_num = DefaultScreen(dpy);
  int scr_depth = DefaultDepth(dpy, scr_num);

  ENDIAN_t end = get_ENDIAN();

  if (end == CPU_LE) {
    if (show) {
      lprintf("little endian cpu\n");
    }
    pconv.byteswap = (XImageByteOrder(dpy) == MSBFirst);
  } else {
    if (show) {
      lprintf("big endian cpu\n");
    }
    pconv.byteswap = (XImageByteOrder(dpy) == LSBFirst);
  }

  pconv.fmt = PXF_UNKNOWN;

  switch (scr_depth) {
  case 15:
    pconv.fmt = PXF_RGB555;
    break;
  case 16:
    pconv.fmt = PXF_RGB565;
    break;
  case 24:
  case 32:
    if (XBitmapUnit(dpy) == 24) {
      pconv.fmt = PXF_RGB24;
    } else if (XBitmapUnit(dpy) == 32) {
      pconv.fmt = PXF_RGB32;
    } else {
      leprintf("Cannot handle bitmap unit %d.\n", XBitmapUnit(dpy));
    }
    break;
  default:
    leprintf("Cannot handle screen depth %d.\n", scr_depth);
    break;
  }

  return pconv;
}

long
x11_rgb2pix(Display * dpy, int r, int g, int b)
{
  XColor color;

  color.red = MIN2(255, r) << 8;
  color.green = MIN2(255, g) << 8;
  color.blue = MIN2(255, b) << 8;

  color.flags = DoRed | DoGreen | DoBlue;

  Colormap cmap = DefaultColormap(dpy, DefaultScreen(dpy));

  if (!XAllocColor(dpy, cmap, &color)) {
    leprintf("Could not allocate color\n");
    return 0xffffffff;		/* default to white */
  }

  return color.pixel;
}

long
x11_a2pix(Display * dpy, char *name)
{
  XColor color;

  int scr_num = DefaultScreen(dpy);
  /*     Screen *scr = ScreenOfDisplay(dpy, scr_num); */

  Colormap cmap = DefaultColormap(dpy, scr_num);

  if (!XParseColor(dpy, cmap, name, &color)) {
    leprintf("XParseColor: Unknown color \"%s\"\n", name);
  }
  if (!XAllocColor(dpy, cmap, &color)) {
    leprintf("XAllocColor: Could not allocate color \"%s\"\n", name);
  }

  return color.pixel;
}

color_t
x11_a2col(const XDpy * dpy, const char *name)
{
  color_t col;
  XColor xcol;

  if (!XParseColor(dpy->dpy, dpy->cmap, name, &xcol))
    leprintf("XParseColor: Unknown color \"%s\"\n", name);
  if (!XAllocColor(dpy->dpy, dpy->cmap, &xcol))
    leprintf("XAllocColor: Could not allocate color \"%s\"\n", name);

  col = color_genRGB(xcol.red >> 8,
		     xcol.green >> 8,
		     xcol.blue >> 8);

  return col;
}

/* fonts */

XFontStruct *
x11_loadFont(const XDpy * dpy, const char *name)
{
  XFontStruct *font;

  /* querying fonts is slow avoid it as much as possible */
  if ((font = XLoadQueryFont(dpy->dpy, name)) == NULL) {
    name = "fixed";
    lprintf("can't find %s. loading fixed instead.\n", name);
    font = XLoadQueryFont(dpy->dpy, name);
  }

  return font;
}

XFontStruct **
x11_loadFonts(const XDpy * dpy, const char ** fwm_names, int num)
{
  int i;
  XFontStruct ** xfs;
  int show = FALSE;
  xfs = malloc(num * sizeof(XFontStruct*));
  for (i = 0; i < num; i++) {
    xfs[i] = XLoadQueryFont(dpy->dpy, fwm_names[i]);
    if (xfs[i]) {
      if (show) {
	lprintf("%s with a max dim of %dx%d\n",
		fwm_names[i],
		x11_maxw(xfs[i]),
		x11_maxh(xfs[i]));
      }
    } else {
      leprintf("can't find %s. loading \"fixed\" instead\n",
	       fwm_names[i]);
      xfs[i] = XLoadQueryFont(dpy->dpy, "fixed");
    }
  }
  return xfs;
}

void
x11_delFonts(const XDpy * dpy, XFontStruct ** xfs, int num)
{
  int i;
  for (i = 0; i < num; i++) {
    XFreeFont(dpy->dpy, xfs[i]);
  }
  free(xfs);
}

XFontStruct *
x11_fitFont(XFontStruct ** fonts, int num, int nchars, int w, int h,
            CLay clay)
{
  int i;
  XFontStruct * font = NULL;

  for (i = num; i >= 1; i--) {
    if (clay.fit == CLAY_FIT_INSIDE) {
      if ((x11_maxw(fonts[i-1]) * nchars) <= w && /* width fits */
	  (x11_maxh(fonts[i-1])         ) <= h) { /* height fits */
	font = fonts[i-1];
	return font;
      }
    } else if (clay.fit == CLAY_FIT_HEIGHT) {
      if (x11_maxh(fonts[i-1]) <= h) { /* height fits */
	font = fonts[i-1];
	return font;
      }
    }
  }

  return font;
}

/* lines and GCs */

void
x11_setLineAttr(const XDpy * dpy, XWin * win,
                uint line_width,
                int line_style, int cap_style, int join_style)
{
  XSetLineAttributes(dpy->dpy, win->gc, line_width, line_style,
		     cap_style, join_style);
}

void
x11_setDefaultLn(const XDpy * dpy, XWin * win)
{
  /* \NOTE pixels can be set outside of endpoints when drawing lines if
   * wrong arguments are set! */
  /*        XSetLineAttributes(dpy->dpy, win->gc, */
  /*                            1, LineSolid, CapNotLast, JoinMiter); */
}

void
x11_setOnOffDash(const XDpy * dpy, XWin * win)
{
  /* \NOTE pixels can be set outside of endpoints when drawing lines if
   * wrong arguments are set! */
  /*      XSetLineAttributes(dpy->dpy, win->gc, */
  /*                          1, LineOnOffDash, CapNotLast, JoinMiter); */
}

int
x11_openDpy(XDpy * dpy)
{
  int show = FALSE;
  /* this function must be the first Xlib function a multi-threaded program
     calls, and it must complete before any other Xlib call is made */
  if (XInitThreads() == 0) {
    leprintf("XInitThreads failed\n");
    return -1;
  }

  /* name */
  dpy->name = getenv("DISPLAY");
  if (dpy->name) {
    dpy->name = strdup(dpy->name); /* duplicate statically allocated string */
  } else {
    dpy->name = strdup(":0.0");
  }
  dpy->dpy = XOpenDisplay(dpy->name);
  if (! dpy->dpy) {
    leprintf("cannot open DISPLAY \"%s\"\n", XDisplayName(dpy->name));
    return -1;
  }

  /* pxf */
  dpy->pconv = x11_gpconv(dpy->dpy);
  if (show) {
    lprintf("display has pixelformat %s and byteswap need is %d\n",
	    PXF_getName(dpy->pconv.fmt), dpy->pconv.byteswap);
  }

  /* defaults */
  dpy->scr_num = DefaultScreen(dpy->dpy);
  dpy->scr = ScreenOfDisplay(dpy->dpy, dpy->scr_num);

  /* depths */
  dpy->scr_depth = DefaultDepth(dpy->dpy, dpy->scr_num);
  switch (dpy->scr_depth) {
  case 8:
    dpy->scr_bpp = 8;
    break;
  case 15:
    dpy->scr_bpp = 16;
    break;
  case 16:
    dpy->scr_bpp = 16;
    break;
  case 24:
  case 32:
    dpy->scr_bpp = 32;
    break;
  }

  /* list of depths available on screen */
  dpy->scr_depth_list = XListDepths(dpy->dpy, dpy->scr_num,
				    &dpy->scr_depthsN);

  dpy->vis = DefaultVisual(dpy->dpy, dpy->scr_num);
  dpy->rwin = DefaultRootWindow(dpy->dpy);

  dpy->cmap = DefaultColormap(dpy->dpy, dpy->scr_num);

  /* win32 style colors */
  dpy->color = x11_a2col(dpy, "#00a0d0");
  dpy->bg = COLOR_MSWIN_SURFACE;
  dpy->fg = COLOR_BLACK;
  dpy->upNW = COLOR_WHITE;
  dpy->upSE = COLOR_GRAY(0x80);

  /* load fonts */
  dpy->hdr_fnt =
    x11_loadFont(dpy, "-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-*-*");
  dpy->sml_fnt =
    x11_loadFont(dpy, "-adobe-helvetica-medium-r-*-*-8-*-*-*-*-*-*-*");
  dpy->nrm_fnt =
    x11_loadFont(dpy, "-adobe-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*");
  dpy->bld_fnt =
    x11_loadFont(dpy, "-adobe-helvetica-bold-r-*-*-10-*-*-*-*-*-*-*");

  dpy->tlt_fnt = x11_loadFont(dpy, "-misc-fixed-medium-r-*-*-10-*-*-*-*-*-*-*");

  dpy->ext_info = XInitExtension(dpy->dpy, "MIT-SHM");
  if (show) {
    lprintf("first_event: %d\n", dpy->ext_info->first_event);
  }

  return 0;
}

int
x11_hasDepth(const XDpy * dpy, int depth)
{
  int i;
  for (i = 0; i < dpy->scr_depthsN; i++) {
    if (dpy->scr_depth_list[i] == depth) {
      return TRUE;
    }
  }
  return FALSE;
}

int
x11_closeDpy(const XDpy * dpy)
{
  /* name */
  if (dpy->name) {
    free(dpy->name);
  }

  /* free depth list */
  XFree(dpy->scr_depth_list);

  /* free fonts */
  x11_freeFont(dpy, dpy->hdr_fnt);
  x11_freeFont(dpy, dpy->sml_fnt);
  x11_freeFont(dpy, dpy->nrm_fnt);
  x11_freeFont(dpy, dpy->bld_fnt);
  x11_freeFont(dpy, dpy->tlt_fnt);

  /* close display */
  XCloseDisplay(dpy->dpy);

  return 0;
}

long
x11_getStandardEventMask(void)
{
  /* NOTE: WARNING: WARNING: WARNING: SOME OF THE MASKS REALLY MESSES
     UP THINGS IN A VERY WEIRD WAY! */
  long risky_event_mask =
    (PointerMotionHintMask |
     Button1MotionMask | Button2MotionMask | Button3MotionMask |
     Button4MotionMask | Button5MotionMask | ButtonMotionMask |
     KeymapStateMask |

     VisibilityChangeMask |

     ResizeRedirectMask | /* WARNING: Messes up resize events! */
     SubstructureNotifyMask |
     SubstructureRedirectMask | /* WARNING: Messes up? */
     FocusChangeMask |
     PropertyChangeMask |
     ColormapChangeMask |
     OwnerGrabButtonMask);
  risky_event_mask = 0;
  return (StructureNotifyMask |
          ExposureMask |
          ButtonPressMask | ButtonReleaseMask |
          PointerMotionMask |
          KeyPressMask | KeyReleaseMask |
          EnterWindowMask | LeaveWindowMask);
}

void
x11_setStandardEventMask(const XDpy * dpy, XWin * win)
{
  win->event_mask = x11_getStandardEventMask();
  XSelectInput(dpy->dpy, win->win, win->event_mask);
}

/* Fixme: Dangerous function! May differ between different X.h files */
const char *
x11_event2str(const XEvent * eve)
{
  return x_event_names[eve->type - 2];
}

void
x11_printEvent(const XEvent * eve)
{
  lprintf("%s", x11_event2str(eve));
}

int
x11_skipEvents(Display * dpy, Window win, long mask, XEvent * event_ret)
{
  int i;			/* skip counter */
  XEvent tmp;
  for (i = 0; TRUE; i++) {
    if (XCheckWindowEvent(dpy, win, mask, &tmp)) {
      *event_ret = tmp;		/* redefine event */
      continue;
    } else {
      break;
    }
  }
  return i;
}

int
x11_skipMotions(XDpy * dpy, XWin * win, XEvent * event_ret)
{
  int i;			/* skip counter */
  XEvent tmp;
  for (i = 0; TRUE; i++) {
    /*          if (XCheckTypedEvent(dpy.dpy, MotionNotify, &tmp))
                { */
    if (XCheckWindowEvent(dpy->dpy, win->win, PointerMotionMask, &tmp)) {
      *event_ret = tmp;		/* redefine event */
      continue;
    } else {
      break;
    }
  }
  /*      lprintf("Skipped: %d MotionNotifies\n", i); */
  return i;
}

int x11_qKeyP(Display * dpy, KeySym keysym)
{
  KeyCode code;
  uint byte, bit, mask;
  char keys_return[32];

  code = XKeysymToKeycode(dpy, keysym);
  byte = code / 8;
  bit = code - byte * 8;
  mask = 1 << bit;
  XQueryKeymap(dpy, keys_return);

  return keys_return[byte] & mask;
}

void x11_key2str(XKeyEvent * key_event, char * buf, int len)
{
  KeySym keysym;
  XComposeStatus status_in_out;
  XLookupString(key_event, buf, len, &keysym, &status_in_out);
}

int
x11_isKeyCodeRepeat(Display * dpy, KeyCode keycode)
{
  char keys[32];
  XQueryKeymap(dpy, keys); /* Look at X11 keystate to detect repeat mode */

  if (keycode < 256)	{ /* XQueryKeymap is limited to 256 keycodes */
    if (keys[keycode >> 3] & (1 << (keycode % 8))) {
      return 1;
    } else {
      return 0;
    }
  } else {
    PWARN("KeyCode %d to large!\n", keycode);
    return -1;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

static const char *x_visclass_names[] = {
  "StaticGray ",
  "GrayScale  ",
  "StaticColor",
  "PseudoColor",
  "TrueColor  ",
  "DirectColor"
};

void
x11_printInfo(XDpy * dpy)
{
  XPixmapFormatValues *values;
  int i, j;
  int count_return;

  printf("Visual Info\n"
	 "===========\n\n"
	 "  ID  Classname(num)    R Mask   G Mask   B Mask    Bits/RGB  Cmap Entries\n"
	 "-------------------------------------------------------------------------\n");

  for (i = 0; i < dpy->scr->ndepths; i++) {
    printf("Depth %d has the visuals:\n", dpy->scr->depths[i].depth);
    for (j = 0; j < dpy->scr->depths[i].nvisuals; j++) {
      printf("  %2ld  %s(%i)  %#8lx %#8lx %#8lx     %i         %i\n",
	     dpy->scr->depths[i].visuals[j].visualid,
	     x_visclass_names[dpy->scr->depths[i].visuals[j].class],
	     dpy->scr->depths[i].visuals[j].class,
	     dpy->scr->depths[i].visuals[j].red_mask,
	     dpy->scr->depths[i].visuals[j].green_mask,
	     dpy->scr->depths[i].visuals[j].blue_mask,
	     dpy->scr->depths[i].visuals[j].bits_per_rgb,
	     dpy->scr->depths[i].visuals[j].map_entries);
    }
  }

  printf("\nDefault visual:\n"
	 "  %2ld  %s(%i)  %#8lx %#8lx %#8lx  %i         %i\n",
	 dpy->vis->visualid,
	 x_visclass_names[dpy->vis->class],
	 dpy->vis->class,
	 dpy->vis->red_mask,
	 dpy->vis->green_mask,
	 dpy->vis->blue_mask, dpy->vis->bits_per_rgb, dpy->vis->map_entries);

  values = XListPixmapFormats(dpy->dpy, &count_return);

  printf("\n");

  printf("Image Format Info\n" "=================\n\n");

  for (i = 0; i < count_return; i++) {
    printf("Pixmap: depth=%i bits_per_pixel=%i scanline_pad=%i\n",
	   values[i].depth, values[i].bits_per_pixel, values[i].scanline_pad);
  }

  printf("\nDisplay Geometry: %ix%i\n",
	 DisplayWidth(dpy->dpy, dpy->scr_num),
	 DisplayHeight(dpy->dpy, dpy->scr_num));

  printf("BitmapUnit: %i\n", BitmapUnit(dpy->dpy));
  printf("ImageByteOrder: ");
  if (XImageByteOrder(dpy->dpy) == LSBFirst) {
    printf("%s\n", "LSBFirst");
  } else {
    printf("%s\n", "MSBFirst");
  }
  printf("BitmapPad: %i\n", BitmapPad(dpy->dpy));

  printf("\n");
}

/* ---------------------------- Group Separator ---------------------------- */

int
x11_openwin_visual(const XDpy * dpy, XWin * win,
                   uint w, uint h, const char *name,
                   XVisualInfo * visualinfo)
{
  int class;
  ulong attrmask;
  uint w_inc, h_inc, border_width;
  XEvent event;

  /* create graphics context */

  win->gc = XCreateGC(dpy->dpy, dpy->rwin, 0, NULL);
  x11_setBG(dpy, win, dpy->bg);
  x11_setFG(dpy, win, dpy->fg);

  /*XTextProperty * text_property = NULL; */
  /*char** string_list; */

  class = InputOutput;		/* InputOutput or InputOnly */
  border_width = 0;

  win->current_fnt = 0;		/* need to initialize this to zero */
  x11_setFont(dpy, win, dpy->nrm_fnt);

  win->attr = malloc(sizeof(XSetWindowAttributes));
  win->attr->background_pixmap = None;
  win->attr->event_mask = 0x1ffffff;
  win->attr->backing_store = Always;	/* NotUseful WhenMapped Always */
  win->attr->background_pixel = pix_get(dpy->pconv, dpy->bg);
  win->attr->save_under = True;	/* Are bits under(popups)  saved? */
  win->attr->cursor = None;
  attrmask = CWEventMask | CWBackPixmap | CWBackPixel | CWSaveUnder;

  win->visualinfo = visualinfo;

  win->win = XCreateWindow(dpy->dpy, dpy->rwin, 0, 0, w, h,
			   border_width,
			   visualinfo ? visualinfo->depth : dpy->scr_depth,
			   class,
			   visualinfo ? visualinfo->visual : dpy->vis,
			   attrmask, win->attr);

  /* color_map = XCreateColormap(dpy->dpy, win->win, visual, AllocNone); */
  /* XSetWindowColormap(dpy->dpy, win->win, color_map); */

  /* set properties for window manager(always before mapping!) */

  w_inc = 1;
  h_inc = 1;

  /*      win->size_hints.flags = PSize | PMinSize | PMaxSize | PResizeInc; */
  win->size_hints.flags = PSize;

  win->size_hints.width = w / w_inc * w_inc;
  win->size_hints.height = h / h_inc * h_inc;

  win->size_hints.width_inc = w_inc;
  win->size_hints.height_inc = w_inc;

  win->size_hints.min_width = w / 2 / w_inc * w_inc;
  win->size_hints.min_height = h / 2 / h_inc * h_inc;

  win->size_hints.max_width = w * 2;
  win->size_hints.max_height = h * 2;

  XSetWMNormalHints(dpy->dpy, win->win, &win->size_hints);

  /* window manager hints */
  win->wm_hints = XAllocWMHints();
  win->wm_hints->flags = 0;
  XSetWMHints(dpy->dpy, win->win, win->wm_hints);

  XStoreName(dpy->dpy, win->win, name);
  XSetIconName(dpy->dpy, win->win, name);

  x11_setStandardEventMask(dpy, win);

  /* request the window to appear on the screen */
  XMapWindow(dpy->dpy, win->win);

  XFlush(dpy->dpy);

  /* wait for window mapping */
  do {
    XNextEvent(dpy->dpy, &event);
  } while (event.type != MapNotify);

  /*      x11_clearwin(&dpy, &win); */

  return 0;
}

int
x11_openwin(const XDpy * dpy, XWin * win,
            uint w, uint h, const char *name)
{
  return x11_openwin_visual(dpy, win,
                            w, h, name,
                            0);
}

int
x11_openwin_GL(const XDpy * dpy, XWin * win,
	       uint w, uint h, const char *name,
	       XVisualInfo * visualinfo)
{
  int class;
  ulong attrmask;
  uint w_inc, h_inc, border_width;
  XEvent event;

  /* create graphics context */

  win->gc = XCreateGC(dpy->dpy, dpy->rwin, 0, NULL);
  x11_setBG(dpy, win, dpy->bg);
  x11_setFG(dpy, win, dpy->fg);

  /*XTextProperty * text_property = NULL; */
  /*char** string_list; */

  class = InputOutput;		/* InputOutput or InputOnly */
  border_width = 0;

  win->current_fnt = 0;		/* need to initialize this to zero */
  x11_setFont(dpy, win, dpy->nrm_fnt);

  win->attr = malloc(sizeof(XSetWindowAttributes));
  /* Prepare the attributes for our window */
  win->visualinfo = visualinfo;
  win->attr->colormap = XCreateColormap(dpy->dpy, dpy->rwin,
					visualinfo->visual, AllocNone);
  win->attr->background_pixmap = None;
  win->attr->event_mask = x11_getStandardEventMask();

  win->attr->backing_store = Always;	/* NotUseful WhenMapped Always */
  win->attr->background_pixel = pix_get(dpy->pconv, dpy->bg);
  win->attr->save_under = True;	/* Are bits under(popups)  saved? */
  win->attr->cursor = None;
  attrmask = CWColormap | CWBorderPixel | CWEventMask;

  win->win = XCreateWindow(dpy->dpy, dpy->rwin, 0, 0, w, h,
			   border_width,
			   visualinfo ? visualinfo->depth : dpy->scr_depth,
			   class,
			   visualinfo ? visualinfo->visual : dpy->vis,
			   attrmask, win->attr);
  win->event_mask = win->attr->event_mask;
  XSelectInput(dpy->dpy, win->win, win->event_mask);

  /* color_map = XCreateColormap(dpy->dpy, win->win, visual, AllocNone); */
  /* XSetWindowColormap(dpy->dpy, win->win, color_map); */

  /* set properties for window manager(always before mapping!) */

  w_inc = 1;
  h_inc = 1;

  /*      win->size_hints.flags = PSize | PMinSize | PMaxSize | PResizeInc; */
  win->size_hints.flags = PSize;

  win->size_hints.width = w / w_inc * w_inc;
  win->size_hints.height = h / h_inc * h_inc;

  win->size_hints.width_inc = w_inc;
  win->size_hints.height_inc = w_inc;

  win->size_hints.min_width = w / 2 / w_inc * w_inc;
  win->size_hints.min_height = h / 2 / h_inc * h_inc;

  win->size_hints.max_width = w * 2;
  win->size_hints.max_height = h * 2;

  XSetWMNormalHints(dpy->dpy, win->win, &win->size_hints);

  /* window manager hints */
  win->wm_hints = XAllocWMHints();
  win->wm_hints->flags = 0;
  XSetWMHints(dpy->dpy, win->win, win->wm_hints);

  XStoreName(dpy->dpy, win->win, name);
  XSetIconName(dpy->dpy, win->win, name);

  x11_setStandardEventMask(dpy, win);

  /* request the window to appear on the screen */
  XMapWindow(dpy->dpy, win->win);

  XFlush(dpy->dpy);

  /* wait for window mapping */
  do {
    XNextEvent(dpy->dpy, &event);
  } while (event.type != MapNotify);

  return 0;
}

int
x11_closewin(const XDpy * dpy, XWin * win)
{
  XFree(win->wm_hints);
  free(win->attr);
  XFreeGC(dpy->dpy, win->gc);
  XDestroyWindow(dpy->dpy, win->win);
  XFree(win->visualinfo);
  return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_mkMap(const XDpy * dpy, const XWin * win, XMap * map, int w, int h)
{
  map->pxm = XCreatePixmap(dpy->dpy, win->win, w, h, dpy->scr_depth);
}

void
x11_clearMap(const XDpy * dpy, XMap * map)
{
  XFreePixmap(dpy->dpy, map->pxm);
}

void
x11_dMap(const XDpy * dpy, const XAnim * xanim,
         const XMap * map,
         int src_x, int src_y, int w, int h, int dest_x, int dest_y)
{
  XCopyArea(dpy->dpy, map->pxm, xanim->xback, xanim->win.gc,
	    src_x, src_y, w, h, dest_x, dest_y);
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_clearArea(const XDpy * dpy, XWin * win,
              int x, int y, uint w, uint h, int exposures)
{
  XClearArea(dpy->dpy, win->win, x, y, w, h, exposures);
}

void
x11_clearwin(const XDpy * dpy, XWin * win)
{
  XClearWindow(dpy->dpy, win->win);
}

/* ---------------------------- Group Separator ---------------------------- */

int
x11_anim_open(const XDpy * dpy, XAnim * xanim,
              uint w, uint h, const char *name)
{
  int ret = x11_openwin(dpy, &xanim->win, w, h, name);

  XdbeSwapAction swap_action = XdbeUndefined;
  xanim->xback = XdbeAllocateBackBufferName(dpy->dpy,
                                            xanim->win.win, swap_action);

  switch (xanim->xback) {
  case BadAlloc: lperror("BadAlloc"); break;
  case BadIDChoice: lperror("BadIDChoice"); break;
  case BadMatch: lperror("BadMatch"); break;
  case BadWindow: lperror("BadWindow"); break;
  case BadValue: lperror("BadValue"); break;
  }

  xanim->info.swap_window = xanim->win.win;

  return ret;
}

int
x11_anim_close(const XDpy * dpy, XAnim * xanim)
{
  XdbeDeallocateBackBufferName(dpy->dpy, xanim->xback);
  return x11_closewin(dpy, &xanim->win);
}

void
x11_anim_clearback(const XDpy * dpy, XAnim * xanim)
{
  int x, y;
  uint w, h;
  x11_getGeom(dpy, &xanim->win, &x, &y, &w, &h);

  x11_setFG(dpy, &xanim->win, dpy->bg);
  XFillRectangle(dpy->dpy, xanim->xback, xanim->win.gc, 0, 0, w, h);
}

void
x11_anim_front2back(const XDpy * dpy, XAnim * xanim)
{
  int x, y;
  uint w, h;
  x11_getGeom(dpy, &xanim->win, &x, &y, &w, &h);

  XCopyArea(dpy->dpy, xanim->win.win, xanim->xback,
	    xanim->win.gc, 0, 0, w, h, 0, 0);
}

void
x11_anim_back2front(const XDpy * dpy, XAnim * xanim)
{
  int x, y;
  uint w, h;
  x11_getGeom(dpy, &xanim->win, &x, &y, &w, &h);

  XCopyArea(dpy->dpy, xanim->xback, xanim->win.win,
	    xanim->win.gc, 0, 0, w, h, 0, 0);
}

void
x11_anim_showback(const XDpy * dpy, XAnim * xanim)
{
  xanim->info.swap_action = XdbeUndefined;

  Status status = XdbeSwapBuffers(dpy->dpy, &xanim->info, 1);
  switch (status) {
  case BadMatch:
    lperror("BadMatch");
    break;
  case BadWindow:
    lperror("BadWindow");
    break;
  case BadValue:
    lperror("BadValue");
    break;
  }
}

void
x11_anim_copyswap(const XDpy * dpy, XAnim * xanim)
{
  xanim->info.swap_action = XdbeCopied;

  Status status = XdbeSwapBuffers(dpy->dpy, &xanim->info, 1);
  switch (status) {
  case BadMatch:
    lperror("BadMatch");
    break;
  case BadWindow:
    lperror("BadWindow");
    break;
  case BadValue:
    lperror("BadValue");
    break;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

static void
callback(XtPointer closure, XtIntervalId * id)
{
}

void
x11_timeouttest(void)
{
  XtAppContext app_context = 0;
  XtIntervalId interval_id;
  interval_id = XtAppAddTimeOut(app_context, 1000, callback, NULL);
  XtRemoveTimeOut(interval_id);
}
