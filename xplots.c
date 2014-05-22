#include "xplots.h"
#include "fft.h"
#include "xdraw.h"
#include "axticks.h"
#include "filters.h"
#include "binlog.h"
#include "meman.h"

#include <stdlib.h>
#include <math.h>

void
x11_plotsimple(const XDpy * dpy, XAnim * anim,
	     int x, int y, uint w, uint h,
	     double *xdat, double *ydat, int l)
{
  int j;
  int wm1, hm1;			/* actual dimensions of plot area */
  double xmin, xmax;
  double ymin, ymax;
  vec2s p[l];

  wm1 = w - 1;
  hm1 = h - 1;

  x11_setFG(dpy, &anim->win, COLOR_BLACK);
  x11_fRect(dpy, anim, x, y, w, h);

  if (l < 2) {
    return;
  }

  darray_extremes(xdat, l, &xmin, &xmax);
  double xspan = xmax - xmin;
  darray_extremes(ydat, l, &ymin, &ymax);
  double yspan = ymax - ymin;
  for (j = 0; j < l; j++) {
    p[j].x = x + wm1 * (xdat[j] - xmin) / xspan;
    p[j].y = y + hm1 - hm1 * (ydat[j] - ymin) / yspan;
  }
  x11_setFG(dpy, &anim->win, COLOR_GREEN);
  x11_dLines(dpy, anim, p, l);
}

void
x11_plot(const XDpy * dpy, XAnim * anim,
	 int x, int y, uint w, uint h,
	 double *xdat, double *ydat, int l,
	 double xmin, double xmax,
	 double ymin, double ymax,
	 const char *xtitle, const char *ytitle)
{
  int j;
  int wm1, hm1;			/* actual dimensions of plot area */
  int lm1;			/* actual span of x-data is(l-1 - 0) */
  vec2s p[l];
  double gstep, gfirsti, glasti;	/* y grid step, first and last */
  /* use integer indexing to prevent arithmetic roundoff in loop */
  uint gi;		/* y grid line index */
  uint gn;		/* number of y grid lines */
  uint pad;
  int use_tmpx;
  color_t grid;

  /* looks cool with a black background */
  x11_setFG(dpy, &anim->win, COLOR_BLACK);
  x11_fRect(dpy, anim, x, y, w, h);

  pad = 48;

  /* draw titles */
  x11_setFG(dpy, &anim->win, COLOR_WHITE);
  x11_setFont(dpy, &anim->win, dpy->hdr_fnt);
  x11_fTextA(dpy, anim, x + w / 2, y + h - pad / 2,
	   xtitle, strlen(xtitle), 'c', 'c', 0);
  x11_fTextA(dpy, anim, x + pad / 2, y + pad / 2,
	   ytitle, strlen(ytitle), 'l', 'c', 0);

  x += pad;
  y += pad;
  if (w < 2 * pad) {
    w = 0;
  } else {
    w -= 2 * pad;
  }
  if (h < 2 * pad) {
    h = 0;
  } else {
    h -= 2 * pad;
  }

  wm1 = w - 1;
  hm1 = h - 1;
  lm1 = l - 1;

  /* draw plotarea frame */
  grid = COLOR_GRAY(128);
  x11_setFG(dpy, &anim->win, grid);
  x11_setDefaultLn(dpy, &anim->win);
  x11_dRect(dpy, anim, x, y, wm1, hm1);

  if (l < 2) {
    return;
  }

  /* if necessary, create temporary x buffer */
  if (xdat == NULL) {
    use_tmpx = TRUE;
    xdat = (double*)malloc(l * sizeof(double));
    darray_ramp(xdat, l, 0.0, 1.0);	/* default data to [0, 1, ..., l-1] */
  } else {
    use_tmpx = FALSE;
  }

  /* grid settings */
  x11_setFont(dpy, &anim->win, dpy->nrm_fnt);

  double xspan = xmax - xmin;
  double yspan = ymax - ymin;

  /* calculate and draw x grid */
  gen_axticksd(&gstep, &gfirsti, &glasti, &gn, xmin, xmax);
  for (gi = 0; gi < gn; gi++) {
    char buf[32];
    /* rounding is needed because of round-off error in expression */
    double val = ((double) gi + gfirsti) * gstep;
    int pos = x + (int) ROUND((double) wm1 * ((val - xmin) / xspan));

    snprintf(buf, sizeof(buf), "%g", val);
    x11_setFG(dpy, &anim->win, COLOR_WHITE);
    x11_fTextA(dpy, anim, pos, y + hm1 + 4,
	     buf, strlen(buf), 'c', 't', 0);

    x11_setFG(dpy, &anim->win, grid);
    x11_dLine(dpy, anim, pos, y, pos, y + hm1);
  }

  /* calculate and draw y grid */
  gen_axticksd(&gstep, &gfirsti, &glasti, &gn, ymin, ymax);
  for (gi = 0; gi < gn; gi++) {
    char buf[32];
    /* rounding is needed because of round-off error in expression */
    double val = ((double) gi + gfirsti) * gstep;
    int pos = y + hm1 - (int) ROUND((double) hm1 * ((val - ymin) / yspan));

    snprintf(buf, sizeof(buf), "%g", val);
    x11_setFG(dpy, &anim->win, COLOR_WHITE);
    x11_fTextA(dpy, anim, x - 3, pos,
	     buf, strlen(buf), 'r', 'c', 0);

    x11_setFG(dpy, &anim->win, grid);
    x11_dLine(dpy, anim, x, pos, x + wm1, pos);
  }

  /* calculate plot */
  for (j = 0; j < l; j++) {
    /* round is needed because of roundoff-errors operations */
    int xpos, ypos;

    xpos = (int) ROUND((double) wm1 * (xdat[j] - xmin) / xspan);
    p[j].x = x + xpos;

    /*  p[j].x = x + j * wm1 / lm1; */
    ypos = (int) ROUND((double) hm1 * (ydat[j] - ymin) / yspan);
    p[j].y = y + hm1 - ypos;
  }

  if (use_tmpx) {
    free(xdat);
  }

  /* draw plot */
  x11_setFG(dpy, &anim->win, COLOR_GREEN);
  x11_setDefaultLn(dpy, &anim->win);
  x11_dLines(dpy, anim, p, l);

}

void
x11_plot_autoaxis(const XDpy * dpy, XAnim * anim,
		int x, int y, uint w, uint h,
		double *xdat, double *ydat, int l,
		const char *xtitle, const char *ytitle)
{
  double xmin, xmax;
  double ymin, ymax;

  /* get extremes and spans */
  darray_extremes(xdat, l, &xmin, &xmax);
  darray_extremes(ydat, l, &ymin, &ymax);

  x11_plot(dpy, anim, x, y, w, h, xdat, ydat, l,
	 xmin, xmax, ymin, ymax, xtitle, ytitle);
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_cplot(const XDpy * dpy, XAnim * anim,
	int x, int y, uint w, uint h,
	double *adata, double *rdata, int l,
	const char *atitle, const char *rtitle)
{
  int j;
  int wm1, hm1;			/* actual dimensions of plot area */
  int lm1;			/* actual span of x-data is(l-1 - 0) */
  double amin, amax, aspan;
  double rmin, rmax, rspan;
  vec2s p[l];
  double gstep, gfirsti, glasti;	/* r grid step, first and last */
  /* use integer indexing to prevent arithmetic roundoff in loop */
  uint gi;		/* r grid line index */
  uint gn;		/* number of r grid lines */
  uint pad;
  color_t grid;

  /* looks cool with a black background */
  x11_setFG(dpy, &anim->win, COLOR_BLACK);
  x11_fRect(dpy, anim, x, y, w, h);

  pad = 48;

  /* draw titles */
  x11_setFG(dpy, &anim->win, COLOR_WHITE);
  x11_setFont(dpy, &anim->win, dpy->hdr_fnt);
/*      x11_fTextA(dpy, anim, x+w/2, y+h-pad/2, atitle, 'c', 'c', 0); */
/*      x11_fTextA(dpy, anim, x+pad/2, y+pad/2, rtitle, 'l', 'c', 0); */

  x += pad;
  y += pad;
  if (w < 2 * pad) {
    w = 0;
  } else {
    w -= 2 * pad;
  }
  if (h < 2 * pad) {
    h = 0;
  } else {
    h -= 2 * pad;
  }

  wm1 = w - 1;
  hm1 = h - 1;
  lm1 = l - 1;

  /* draw plotarea frame */
  grid = COLOR_GRAY(128);
  x11_setFG(dpy, &anim->win, grid);
  x11_setDefaultLn(dpy, &anim->win);
  x11_dRect(dpy, anim, x, y, wm1, hm1);

  if (l < 2) {
    return;
  }

  /* get extremes and spans */
  darray_extremes(adata, l, &amin, &amax);
  aspan = amax - amin;
  darray_extremes(rdata, l, &rmin, &rmax);
  rspan = rmax - rmin;

  /* grid settings */
  x11_setFont(dpy, &anim->win, dpy->nrm_fnt);

  /* calculate and draw a grid */
  gstep = M_2PI / 16;
  gfirsti = 0.0;
  glasti = 0.0;
  gn = 16;
  for (gi = 0; gi < gn; gi++) {
    char buf[32];
    /* rounding is needed because of round-off error in expression */
    double val = (double) gi * gstep;
    int xi, yi;
    int x1, y1;
    int x2, y2;

    xi = (int) ROUND((double) wm1 * cos(val));
    yi = -(int) ROUND((double) hm1 * sin(val));
    x1 = x + (wm1 + 0) / 2;
    y1 = y + (hm1 + 0) / 2;
    x2 = x + (wm1 + xi) / 2;
    y2 = y + (hm1 + yi) / 2;

    x11_setFG(dpy, &anim->win, grid);
    x11_dLine(dpy, anim, x1, y1, x2, y2);

    xi = (int) ROUND((double) (23 + wm1) * cos(val));
    yi = -(int) ROUND((double) (23 + hm1) * sin(val));
    x2 = x + (wm1 + xi) / 2;
    y2 = y + (hm1 + yi) / 2;

    snprintf(buf, sizeof(buf), "%.1f", val * 360 / M_2PI);
    x11_setFG(dpy, &anim->win, COLOR_WHITE);
    x11_fTextA(dpy, anim, x2, y2,
	     buf, strlen(buf), 'c', 'c', 0);
  }

  /* calculate and draw r grid */
  gen_axticksd(&gstep, &gfirsti, &glasti, &gn, rmin, rmax);
  for (gi = 0; gi < gn; gi++) {
    char buf[32];
    /* rounding is needed because of round-off error in expression */
    double val = ((double) gi + gfirsti) * gstep;
    int wi = (int) ROUND((double) wm1 * ((val - rmin) / rspan));
    int hi = (int) ROUND((double) hm1 * ((val - rmin) / rspan));

    x11_setFG(dpy, &anim->win, grid);
    x11_dEll(dpy, anim, x + (wm1 - wi) / 2, y + (hm1 - hi) / 2, wi, hi);

    snprintf(buf, sizeof(buf), "%g", val);
    x11_setFG(dpy, &anim->win, COLOR_WHITE);
    x11_fTextA(dpy, anim, x + (wm1 + wi) / 2, y + hm1 / 2,
	     buf, strlen(buf), 'c', 'c', 0);
  }

  /* calculate plot */
  for (j = 0; j < l; j++) {
    /* round is needed because of roundoff-errors operations */
    int xpos, ypos;

    xpos = (int) ROUND((double) wm1 * (rdata[j] - rmin) / rspan *
		       cos(adata[j]));
    p[j].x = x + 0 + (wm1 + xpos) / 2;

    ypos = (int) ROUND((double) hm1 * (rdata[j] - rmin) / rspan *
		       sin(adata[j]));
    p[j].y = y + hm1 - (hm1 + ypos) / 2;
  }

  /* draw plot */
  x11_setFG(dpy, &anim->win, COLOR_GREEN);
  x11_setDefaultLn(dpy, &anim->win);
  x11_dLines(dpy, anim, p, l);

}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_fplot(const XDpy * dpy, XAnim * anim, XFall * fall,
	color_t * pal, uint pal_len,
	int x, int y, uint w, uint h,
	double *xdat, double *ydat, uint l,
	double samp_rate,
	const char *xtitle, const char *ytitle,
	const char *fx_title, const char *fy_title,
	int is_log,
	box2d * taxx, box2d * faxx, uint axxhint)
{
  int x1, y1, w1, h1;
  int x2, y2, w2, h2;
  int x3, y3, w3, h3;

  w1 = w;
  w2 = w;
  w3 = w;

  h1 = h / 3;
  h2 = h / 3;
  h3 = h - h1 - h2;

  x1 = x;
  x2 = x;
  x3 = x;

  y1 = y;
  y2 = y + h1;
  y3 = y + 2 * h1;

  if (taxx) {
    double xmin, xmax;
    double ymin, ymax;

    /* get extremes and spans */
    darray_extremes(xdat, l, &xmin, &xmax);
    darray_extremes(ydat, l, &ymin, &ymax);

    if (axxhint == 0) {
      box2d_set(taxx, xmin, ymin, xmax, ymax);
    } else {
      /* extend it */
      taxx->l.x = MIN2(taxx->l.x, xmin);
      taxx->l.y = MIN2(taxx->l.y, ymin);
      taxx->u.x = MAX2(taxx->u.x, xmax);
      taxx->u.y = MAX2(taxx->u.y, ymax);
    }

    x11_plot(dpy, anim, x1, y1, w1, h1, xdat, ydat, l,
	   taxx->l.x, taxx->u.x,
	   taxx->l.y, taxx->u.y,
	   xtitle, ytitle);
  } else {
    x11_plot_autoaxis(dpy, anim, x1, y1, w1, h1, xdat, ydat, l,
		    xtitle, ytitle);
  }

  double *xi = darray_calloc(l);	/* zeros */
  double *tre = darray_malloc(l);	/* real part of time domain data */
  double *tim = darray_malloc(l);	/* imag part of time domain data */
  double *abs = darray_malloc(l);

  uint lo2 = l / 2;
  double *fscale = darray_malloc(lo2);
  darray_ramp(fscale, lo2, 0, samp_rate / 2 / lo2);

  double *ydat_f = darray_malloc(l);

  uint i;

  /* windowing filter */
  for (i = 0; i < l; i++) {
    ydat_f[i] = ydat[i] * double_blackman(i, l);
  }

  if (IS_BINPOW(l)) {		/* only if length is a power of two */
    /* can we perform an FFT */
    darray_FFT(tre, tim, ydat_f, xi, l);
    darray_eud2D(abs, tre, tim, l);
    if (is_log) {
      uint i;
      for (i = 0; i < l; i++) {
	abs[i] = log10(abs[i]);
      }
    }

    if (faxx) {
      double xmin, xmax;
      double ymin, ymax;

      /* get extremes and spans */
      darray_extremes(fscale, lo2, &xmin, &xmax);
      darray_extremes(abs, l, &ymin, &ymax);

      if (axxhint == 0) {
	box2d_set(faxx, xmin, ymin, xmax, ymax);
      } else {
	/* extend it */
	faxx->l.x = MIN2(faxx->l.x, xmin);
	faxx->l.y = MIN2(faxx->l.y, ymin);
	faxx->u.x = MAX2(faxx->u.x, xmax);
	faxx->u.y = MAX2(faxx->u.y, ymax);
      }

      x11_plot(dpy, anim,
	     x2, y2, w2, h2, fscale, abs, lo2,
	     faxx->l.x, faxx->u.x,
	     faxx->l.y, faxx->u.y,
	     fx_title, fy_title);
    } else {
      x11_plot_autoaxis(dpy, anim,
		      x2, y2, w2, h2, fscale, abs, lo2,
		      fx_title, fy_title);
    }

    double ymin, ymax;
    darray_extremes(abs, l, &ymin, &ymax);
    double yspan = ymax - ymin;
    color_t * cols = (color_t*)malloc(lo2 * sizeof(color_t));
    /* Waterfall */
    for (i = 0; i < lo2; i++) {
      uint val =
	uint_clamp(0,
		   (uint)((abs[i] - ymin) / yspan * (pal_len - 1)),
		   pal_len - 1);
      cols[i] = pal[val];
    }
    x11_appFall(dpy, anim, fall, cols, lo2);
    free(cols);
    x11_dWFall(dpy, anim, fall, x3, y3);

  } else {
    leprintf("%s: n=%u is not a power of two. Returning.\n", __FUNCTION__, l);
  }

  free(xi);
  free(tre);
  free(tim);
  free(abs);
  free(fscale);
  free(ydat_f);
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_bars1d(const XDpy * dpy, XAnim * anim,
	 int x, int y, uint w, uint h, double *b, int l)
{
  int j;
  int barw = 50;		/* barw width in percent of bar distance */
  double min, max, span;
  rect2s r[l];

  x11_setFG(dpy, &anim->win, COLOR_BLACK);
  x11_fRect(dpy, anim, x, y, w, h);

  if (l < 2) {
    return;
  }

  darray_extremes(b, l, &min, &max);
  span = max - min;
  for (j = 0; j < l; j++) {
    int currentx = x + j * w / l;
    int nextx = x + (j + 1) * w / l;
    int hj = (double) h * (b[j] - min) / span;
    r[j].x = currentx;
    r[j].y = y + h - hj;
    r[j].w = barw * (nextx - currentx) / 100;
    r[j].h = hj;
  }
  x11_setFG(dpy, &anim->win, COLOR_GREEN);
  x11_fRects(dpy, anim, r, l);
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_btv2d(const XDpy * dpy, XAnim * anim,
	int x, int y, uint w, uint h, Bitvec * btv)
{
  uint i, j;
  if (w * h != btv->l) {
    leprintf("Dimension mismatch.\n");
    return;
  }
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      x11_setFG(dpy, &anim->win, bitvec_get(btv, i * w + j) ?
	      COLOR_WHITE : COLOR_BLACK);
      x11_dPnt(dpy, anim, x + j, y + i);
    }
  }
}
