/*!
 * \file xdraw.h
 * \brief Drawing routines for X11 wrappers.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "xwin.h"

#include "vec2s.h"
#include "ext2s.h"
#include "box2s.h"
#include "rect2s.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Draw a point.
 */
static inline void
x11_dPnt(const XDpy * dpy, XAnim * anim, int x, int y)
{
  XDrawPoint(dpy->dpy, anim->xback, anim->win.gc, x, y);
}

/*!
 * Draw a line.
 */
static inline void
x11_dLine(const XDpy * dpy, XAnim * anim,
	  int x1, int y1, int x2, int y2)
{
  XDrawLine(dpy->dpy, anim->xback, anim->win.gc,
	    x1, y1, x2, y2);
}

/*!
 * Draw a line between the points \p p and \p q.
 */
static inline void
x11_dLine_vec2s(const XDpy * dpy, XAnim * anim,
		const vec2s * p, const vec2s * q)
{
  XDrawLine(dpy->dpy, anim->xback, anim->win.gc,
	    p->x, p->y, q->x, q->y);
}

/*!
 * Draw \p np number of lines at \p p.
 */
static inline void
x11_dLines(const XDpy * dpy, XAnim * anim,
	   vec2s * p, int np)
{
  XDrawLines(dpy->dpy, anim->xback, anim->win.gc,
	     (XPoint *)p, np, CoordModeOrigin);
}

/*!
 * Draw \p np number of line segments at \p p.
 */
static inline void
x11_dSegs(const XDpy * dpy, XAnim * anim,
	  box2s * p, int np)
{
  XDrawSegments(dpy->dpy, anim->xback, anim->win.gc,
		(XSegment*)p, np);
}

/*!
 * Draw a Rectangle.
 * \NOTE Draws a rectangle with the dimensions \p w+1 x \p h+1.
 */
static inline void
x11_dRect(const XDpy * dpy, XAnim * anim,
	  int x, int y, uint w, uint h)
{
  XDrawRectangle(dpy->dpy, anim->xback, anim->win.gc, x, y, w, h);
}

/*!
 * Draw a Rectangle.
 *
 * \NOTE Fills a rectangle with the dimensions w x h.
 */
static inline void
x11_fRect(const XDpy * dpy, XAnim * anim,
	  int x, int y, uint w, uint h)
{
  XFillRectangle(dpy->dpy, anim->xback, anim->win.gc, x, y, w, h);
}

/*! Fill a box exactly inside the screen box \p sbox. */
static inline void
x11_fBox(const XDpy * dpy, XAnim * anim, const box2s * sbox)
{
  x11_fRect(dpy, anim,
	    sbox->l.x,
	    sbox->l.y,
	    sbox->u.x - sbox->l.x + 1,
	    sbox->u.y - sbox->l.y + 1);
}

/*! Draw a box exactly inside the screen box \p sbox. */
static inline void
x11_dBox(const XDpy * dpy, XAnim * anim, const box2s * sbox)
{
  x11_dRect(dpy, anim,
	    sbox->l.x,
	    sbox->l.y,
	    /* No adjustment needed here because upper limit minus lower limit
	     * automatically become one pixel too short which is what
	     * x11_dRect() and in turn XDrawRectangle() wants. */
	    sbox->u.x - sbox->l.x,
	    sbox->u.y - sbox->l.y);
}

/*!
 * Draw a Set of Rectangles.
 *
 * \NOTE Draws a rectangle with the dimensions \p w+1 x \p h+1.
 */
static inline void
x11_dRects(const XDpy * dpy, XAnim * anim, rect2s * rects, int nrects)
{
  XDrawRectangles(dpy->dpy, anim->xback, anim->win.gc,
		  (XRectangle *) rects, nrects);
}

/*!
 * Fill a Set of Rectangles.
 *
 * \NOTE Draws a rectangle with the dimensions \p w+1 x \p h+1.
 */
static inline void
x11_fRects(const XDpy * dpy, XAnim * anim, rect2s * rects, int nrects)
{
  XFillRectangles(dpy->dpy, anim->xback, anim->win.gc,
		  (XRectangle *) rects, nrects);
}

/*!
 * Draw a Square.
 *
 * \NOTE Draws a square with the dimensions \p l+1 x \p l+1.
 */
static inline void
x11_dSquare(const XDpy * dpy, XAnim * anim, int x, int y, int l)
{
  XDrawRectangle(dpy->dpy, anim->xback, anim->win.gc, x, y, l, l);
}

/*!
 * Fill a Square.
 *
 * \NOTE Fills a square with the dimensions \p l x \p l.
 */
static inline void
x11_fSquare(const XDpy * dpy, XAnim * anim, int x, int y, int l)
{
  XFillRectangle(dpy->dpy, anim->xback, anim->win.gc, x, y, l, l);
}

/*!
 * \name Arcs
 */
/* @{ */

static inline void
x11_dArc(const XDpy * dpy, XAnim * anim,
	 int x, int y, uint w, uint h, int a1, int a2)
{
  XDrawArc(dpy->dpy, anim->xback, anim->win.gc, x, y, w, h, a1, a2);
}

static inline void
x11_fArc(const XDpy * dpy, XAnim * anim,
	 int x, int y, uint w, uint h, int a1, int a2)
{
  XFillArc(dpy->dpy, anim->xback, anim->win.gc, x, y, w, h, a1, a2);
}

static inline void
x11_dArcs(const XDpy * dpy, XAnim * anim, arc2s * arcs, int narcs)
{
  XDrawArcs(dpy->dpy, anim->xback, anim->win.gc, (XArc *) arcs, narcs);
}

static inline void
x11_fArcs(const XDpy * dpy, XAnim * anim, arc2s * arcs, int narcs)
{
  XFillArcs(dpy->dpy, anim->xback, anim->win.gc, (XArc *) arcs, narcs);
}

/* @} */

/*!
 * \name Ellipses
 */
/* @{ */

static inline void
x11_dEll(const XDpy * dpy, XAnim * anim,
	 int x, int y, uint w, uint h)
{
  XDrawArc(dpy->dpy, anim->xback, anim->win.gc, x, y, w, h, 0 * 64, 360 * 64);
}

static inline void
x11_fEll(const XDpy * dpy, XAnim * anim,
	 int x, int y, uint w, uint h)
{
  XFillArc(dpy->dpy, anim->xback, anim->win.gc, x, y, w, h, 0 * 64, 360 * 64);
}

/* @} */

/*!
 * Circles.
 */
/* @{ */

static inline void
x11_dCirc(const XDpy * dpy, XAnim * anim, int x, int y, uint d)
{
  XDrawArc(dpy->dpy, anim->xback, anim->win.gc, x, y, d, d, 0 * 64, 360 * 64);
}

static inline void
x11_fCirc(const XDpy * dpy, XAnim * anim, int x, int y, uint d)
{
  XFillArc(dpy->dpy, anim->xback, anim->win.gc, x, y, d, d, 0 * 64, 360 * 64);
}

/* @} */

/*!
 * Polygons.
 */
/* @{ */

static inline void
x11_fPoly(const XDpy * dpy, XAnim * anim, vec2s * points, int points_num)
{
  XFillPolygon(dpy->dpy, anim->xback, anim->win.gc,
	       (XPoint*)points,
	       points_num,
	       Convex,
	       CoordModeOrigin);
}

static inline void
x11_dPoly(const XDpy * dpy, XAnim * anim, vec2s * points, int points_num)
{
  vec2s points2[points_num + 1];
  memcpy(points2, points, points_num * sizeof(vec2s));
  points2[points_num] = points[0];
  XDrawLines(dpy->dpy, anim->xback, anim->win.gc,
	     (XPoint*)points2,
	     points_num+1,
	     CoordModeOrigin);
}

/* @} */

/*!
 * Text
 */
/* @{ */

/*!
 * Draw text.
 */
static inline void
x11_fText(const XDpy * dpy, XAnim * anim, int x, int y,
	  const char *text, int len)
{
  XDrawString(dpy->dpy, anim->xback, anim->win.gc, x, y, text, len);
}

/*!
 * Draw Image Text.
 */
static inline void
x11_dIText(const XDpy * dpy, XAnim * anim, int x, int y,
	   const char *text, int len)
{
  XDrawImageString(dpy->dpy, anim->xback, anim->win.gc, x, y, text, len);
}

/*!
 * Draw Text Adjusted by \p xadj and \p yadj.
 */
void x11_fTextA(const XDpy * dpy, XAnim * anim,
		int x, int y, const char *text, size_t textlen,
		char xadj, char yadj,
		bool img_flag);

/* @} */

/*!
 * 3D-shapes
 */

/*!
 * Draw a Standard Four-Color Shaded Outlined-3D-Rectangle.
 */
void x11_dRect3D(const XDpy * dpy, XAnim * anim,
		 int x, int y, uint w, uint h, int raised);

/*!
 * Draw a Standard Four-Color Shaded Outlined-3D-Rectangle.
 */
void x11_dRect3Dve(const XDpy * dpy, XAnim * anim,
		   const vec2s * start, const ext2s * size, int raised);

/*!
 * Draw a Standard Four-Color Shaded Outlined-3D-Rectangle.
 */
static inline void x11_dRect3D_box2s(const XDpy * dpy, XAnim * anim,
				     const box2s * sbox, int raised)
{
  x11_setFG(dpy, &anim->win, raised ? dpy->upNW : dpy->upSE);
  x11_dLine(dpy, anim, sbox->l.x, sbox->l.y, sbox->u.x, sbox->l.y);
  x11_dLine(dpy, anim, sbox->l.x, sbox->u.y, sbox->l.x, sbox->l.y);
  x11_setFG(dpy, &anim->win, raised ? dpy->upSE : dpy->upNW);
  x11_dLine(dpy, anim, sbox->u.x, sbox->l.y, sbox->u.x, sbox->u.y);
  x11_dLine(dpy, anim, sbox->u.x, sbox->u.y, sbox->l.x, sbox->u.y);
}

/*!
 * Draw a 3D-frame.
 *
 * \param[in] raised indicates whether frame is raised (non-zero) or
 *            lowered (zero).
 */
static inline void
x11_dFrame3D(const XDpy * dpy, XAnim * anim,
	     int x, int y, uint w, uint h,
	     int raised)
{
  x11_dRect3D(dpy, anim, x, y, w, h, raised);
  x11_dRect3D(dpy, anim, x + 1, x + 1, w - 2, h - 2, !raised);
}

/*!
 * Draw a 3D-frame.
 *
 */
static inline void
x11_dFrame3D_box2s(const XDpy * dpy, XAnim * anim,
		   const box2s * sbox)
{
  int raised = TRUE;
  x11_dRect3D_box2s(dpy, anim, sbox, raised);
  box2s sb2 = { { sbox->l.x+1, sbox->l.y+1 }, { sbox->u.x-1, sbox->u.y-1 } };
  x11_dRect3D_box2s(dpy, anim, &sb2, !raised);
}

/*!
 * Draw a ProgressBar.
 *
 * \param[in] p Progress
 * \param[in] pmax Progress Max
 */
void x11_dPBar(const XDpy * dpy, XAnim * anim,
	       int x, int y, uint w, uint h,
	       uint p, uint pmax);

void x11_dPBarAlt(const XDpy * dpy, XAnim * anim,
		  const vec2s * start, const ext2s * size, int p, int max);

/*!
 * Draw a Regular Grid.
 */
void x11_dGridReg(const XDpy * dpy, XAnim * anim,
		  int x, int y, uint w, uint h, int nx, int ny);

void x11_dGradX(const XDpy * dpy, XAnim * anim,
		int x, int y, uint w, uint h,
		color_t col1, color_t col2);

void x11_dGradY(const XDpy * dpy, XAnim * anim,
		int x, int y, uint w, uint h,
		color_t col1, color_t col2);

void x11_dGradXN(const XDpy * dpy, XAnim * anim,
		 int x, int y, uint w, uint h, CPAL_t cpal);

void x11_dGradYN(const XDpy * dpy, XAnim * anim,
		 int x, int y, uint w, uint h, CPAL_t cpal);

void x11_dMandelbrotf(const XDpy * dpy, XImg * const img,
		      float xmin, float xmax, float ymin, float ymax);

void x11_dSierpinski3D(XDpy * dpy, XAnim * anim,
		       int x, int y, uint w, uint h,
		       double x1, double y1,
		       double x2, double y2,
		       double x3, double y3, uint level);

void x11_dMtx(XDpy * dpy, XAnim * anim, const short **in,
	      int x0, int y0, int w, int h);

#ifdef __cplusplus
}
#endif
