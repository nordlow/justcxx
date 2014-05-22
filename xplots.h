/*!
 * \file xplots.h
 * \brief XWindows Plotting and Drawing functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "xwin.h"
#include "bitvec.h"
#include "box2d.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

void x11_plotsimple(const XDpy * dpy, XAnim * anim,
		  int x, int y, uint w, uint h,
		  double *xdata, double *ydata, int l);

/*!
 * Draw a Normal Plot.
 *
 * \TODO: The topmost grid line doesn't reach the topmost plot are line.
 */
void x11_plot(const XDpy * dpy, XAnim * anim,
	    int x, int y, uint w, uint h,
	    double *xdat, double *ydat, int l,
	    double xmin, double xmax,
	    double ymin, double ymax,
	    const char *xtitle, const char *ytitle);

void x11_plot_autoaxis(const XDpy * dpy, XAnim * anim,
		     int x, int y, uint w, uint h,
		     double *xdata, double *ydata, int l,
		     const char *xtitle, const char *ytitle);

/*!
 * Draw a Complex Valued Plot.
 *
 * \TODO: The topmost grid line doesn't reach the topmost plot are line.
 */
void x11_cplot(const XDpy * dpy, XAnim * anim,
	     int x, int y, uint w, uint h,
	     double *adata, double *rdata, int l,
	     const char *atitle, const char *rtitle);

/*!
 * Draw a Frequency Plot.
 *
 * \param[in] taxx is non-zero if user defined time axis-extents are given
 * \param[in] faxx is non-zero if user defined frequency axis-extents are given
 * \param[in] axxhint is the axis extents hint relevant only if taxx is non-zero
 *
 * \TODO: The topmost grid line doesn't reach the topmost plot are line.
 */
void
x11_fplot(const XDpy * dpy, XAnim * anim, XFall * fall,
	color_t * pal, unsigned pal_len,
	int x, int y, uint w, uint h,
	double *xdat, double *ydat, uint l,
	double samp_rate,
	const char *xtitle, const char *ytitle,
	const char *fx_title, const char *fy_title,
	int is_log,
	box2d * taxx, box2d * faxx, uint axxhint);

/*!
 * Draw Some Bars.
 *
 * \TODO: The topmost grid line doesn't reach the topmost plot are line.
 */
void x11_bars1d(const XDpy * dpy, XAnim * anim,
	      int x, int y, uint w, uint h, double *b, int l);

/*!
 * Draw a BitVector of length(\p w * \p h) as a 2D-image(\p w x \p h).
 */
void x11_btv2d(const XDpy * dpy, XAnim * anim,
	     int x, int y, uint w, uint h, Bitvec * btv);

#ifdef __cplusplus
}
#endif
