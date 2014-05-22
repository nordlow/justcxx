/*!
 * \file ximg.h
 * \brief Wrapper for constructing XImages with or without the
 * X shared memory extension (XSHM).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/* Often used definitions for the X window system. */

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

/* X Double Buffer Extension */
#include <X11/extensions/Xdbe.h>

#ifdef PNW_USE_MITSHM
/* X Shared Memory Extension */
#include <X11/extensions/XShm.h>
#endif

#include "pixels.h"
#include "byterev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * X Windows Image structure.
 *
 * Hides handling of X Shared Memory Extension (XSHM) for XImages.
 */
typedef struct
{
  bool shm_flag:1;	/**< Shared memory flag. */
  uint shm_locked:1; /**< Locked (waiting for XShmCompletionEvent). */
#ifdef PNW_USE_MITSHM
  XShmSegmentInfo shminfo;
#else
#endif
  XImage *img;
  PConv pconv;
  int bytes_per_pixel;
} XImg;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Initialize (construct) image.
 *
 * \param[in] w is the width.
 * \param[in] w is the height.
 * \param[in] try_shm is non-zero if we want to try to use
 *        X Shared Memory Extension.
 *
 * \return zero upon success, otherwise -1.
 */
int x11_initImg(Display * dpy, XImg * img,
	      uint w, uint h, int try_shm);

/*!
 * Construct Sub Image of the argument img at position (x, y) with
 * dimensions w x h.
 */
XImg x11_csubImg(const XImg * img, int x, int y,
		 uint w, uint h);

/*!
 * Clear Image \p img.
 *
 * \return 1 if a clear was made, or -1 if image was locked.
 */
int x11_clearImg(Display * dpy, XImg * img);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Lock Image \p img.
 *
 * Triggered by an XShmPutImage \p img.
 */
void x11_lockImg(XImg * img);

/*!
 * Unlock Image \p img.
 *
 * Triggered by an XShmCompletionEvent.
 */
void x11_unlockImg(XImg * img);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Resize Image \p img.
 *
 * \return 1 if a resize was made, 0 if no resize was needed,
 *         or -1 if image was locked.
 */
int x11_rszImg(Display * dpy, XImg * img, uint w, uint h);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Put Image Region of size w x h at coordinate (x, y) on Window win.
 */
void x11_regputImg(Display * dpy, Drawable win, GC gc, const XImg * img,
		 int x, int y, uint w, uint h);

/*!
 * Put Image At coordinate (x, y) on Window win.
 */
void x11_putImg(Display * dpy, Drawable win, GC gc, const XImg * img,
	      int x, int y);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Set (possibly convert) Color to the given pixel type t and Set it At
 * position i in the array of elements with this pixel type.
 */
static inline void
char_setAt(char *data, PConv pconv, color_t color)
{
  uint32_t p = 0;

  switch (pconv.fmt) {
  case PXF_RGB555:
    p = color_to_RGB555(color);
    if (pconv.byteswap) {
      p = bswap_16(p);
    }
    *((uint16_t *) data) = p;
    break;
  case PXF_RGB565:
    p = color_truncate_to_RGB565(color);
    if (pconv.byteswap) {
      p = bswap_16(p);
    }
    *((uint16_t *) data) = p;
    break;
  case PXF_RGB32:
    p = color_to_URGB32(color);
    if (pconv.byteswap) {
      /* OK:   client on sparc-solaris and server x86-linux   */
      /* \TODO: server on sparc-solaris and client x86-linux   */
      p = bswap_32(p);
    } else {
      /* OK:   locally on sparc-solaris */
      /* OK:   locally on x86-linux */
    }
    *((uint32_t *) data) = p;
    break;
  default:
    leprintf("Could not handle type %s\n", PXF_getName(pconv.fmt));
    break;
  }
}

/*!
 * Set the pixel at (x, y) in the image img to the given color using pixel
 * conversion \p pconv.
 */
static inline void
x11_ximg_setAt_xy(XImg * img, uint x, uint y, color_t color)
{
  char_setAt(img->img->data +			/* data offset */
	     y * img->img->bytes_per_line +	/* row offset */
	     x * img->bytes_per_pixel,		/* column offset */
	     img->pconv, color);
}

/*!
 * Set all n the pixels starting at (x, y) in the image img
 * to the colors given in the color array cols
 * using pixel conversion \p pconv.
 */
static inline void
ximg_setLine(XImg * img, uint x, uint y,
	     color_t * colors, uint n)
{
  uint i;
  char * off = (img->img->data +	  /* data offset */
		y * img->img->bytes_per_line); /* row offset */
  for (i = 0; i < n; i++) {
    char_setAt(off +
	       (x + i) * img->bytes_per_pixel,	/* column offset */
	       img->pconv, colors[i]);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Render Gray Image \p img.
 */
void x11_grayImg(XImg * img, const uchar *data);

/*!
 * Render Rgb Image \p img.
 */
void x11_rgbImg(XImg * img, const uchar *data);

/*!
 * Render Rgb Image \p img.
 */
void x11_rgb32Img(XImg * img, const color_t *data);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Get Pixel at Position \p x, y in Image \p img.
 */
static inline ulong
x11_getPix(const XImg * img, int x, int y)
{
  return XGetPixel(img->img, x, y);
}

/*!
 * Put Pixel at position \p x, y in Image \p img.
 */
static inline void
x11_putPix(const XImg * img, int x, int y, ulong pixel)
{
  XPutPixel(img->img, x, y, pixel);
}

/*!
 * Add Pixel At all positions in the Image \p img.
 */
static inline void
x11_addPix(const XImg * img, ulong pixel)
{
  XAddPixel(img->img, pixel);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Render a Nice Gradient into the Image \p img using the index c.
 */
void x11_nicegrad(XImg * img, uint c);

/*!
 * Test function for Image Routines.
 */
void x11_testimg1(XImg * img);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Construct Image at img from a PGM-file.
 *
 * \param[in] filename is the name of PGM-file.
 * \return 0 upon success, otherwise -1.
 */
int x11_cimgPGM(Display * dpy, XImg * img, const char *filename);

/*!
 * Construct Image at img from a PPM-file.
 *
 * \param[in] filename is the name of PPM-file.
 * \return 0 upon success, otherwise -1.
 */
int x11_cimgPPM(Display * dpy, XImg * img, const char *filename);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
