#include "xdraw.h"
#include "fractals.h"

void
x11_fTextA(const XDpy * dpy, XAnim * anim,
	 int x, int y, const char *text, size_t textlen,
	 char xadj, char yadj, bool img_flag)
{
  int direction_return;
  int font_ascent_return, font_descent_return;

  XCharStruct overall;

  XTextExtents(anim->win.current_fnt,
	       text, textlen,
	       &direction_return,
	       &font_ascent_return, &font_descent_return, &overall);

  /* use overall bounds in x adjustments */
  switch (xadj) {
  case 'l': x -= 0; break;	/* left */
  case 'c': x -= overall.width / 2; break; /* centered */
  case 'r': x -= overall.width; break; /* right */
  default: break;
  }

  /* use maximum bounds in y adjustments */
  switch (yadj) {
  case 'b': y -= anim->win.current_fnt->max_bounds.descent; break; /* bottom */
  case 'c': y += (anim->win.current_fnt->max_bounds.ascent - /* centered */
		  anim->win.current_fnt->max_bounds.descent) / 2;
    break;
  case 't': y += overall.ascent; break;	/* top */
  default:
    break;
  }

  if (img_flag) {
    x11_dIText(dpy, anim, x, y, text, textlen);
  } else {
    x11_fText(dpy, anim, x, y, text, textlen);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_dRect3D(const XDpy * dpy, XAnim * anim,
	  int x, int y, uint w, uint h, int raised)
{
  x11_setFG(dpy, &anim->win, raised ? dpy->upNW : dpy->upSE);
  x11_dLine(dpy, anim, x, y, x + w - 1, y);
  x11_dLine(dpy, anim, x, y + h - 1, x, y);
  x11_setFG(dpy, &anim->win, raised ? dpy->upSE : dpy->upNW);
  x11_dLine(dpy, anim, x + w - 1, y, x + w - 1, y + h - 1);
  x11_dLine(dpy, anim, x + w - 1, y + h - 1, x, y + h - 1);
}

void
x11_dRect3Dve(const XDpy * dpy, XAnim * anim,
	     const vec2s * start, const ext2s * size, int raised)
{
  x11_setFG(dpy, &anim->win, raised ? dpy->upNW : dpy->upSE);
  x11_dLine(dpy, anim, start->x, start->y, start->x + size->w - 1, start->y);
  x11_dLine(dpy, anim, start->x, start->y + size->h - 1, start->x, start->y);

  x11_setFG(dpy, &anim->win, raised ? dpy->upSE : dpy->upNW);
  x11_dLine(dpy, anim, start->x + size->w - 1, start->y,
	  start->x + size->w - 1, start->y + size->h - 1);
  x11_dLine(dpy, anim, start->x + size->w - 1, start->y + size->h - 1,
	  start->x, start->y + size->h - 1);
}

void
x11_dPBar(const XDpy * dpy, XAnim * anim,
	int x, int y, uint w, uint h,
	uint p,
	uint pmax)
{
  x11_dRect3D(dpy, anim, x, y, w, h, FALSE);
  x11_setFG(dpy, &anim->win, dpy->color);
  if (p > pmax) {		/* upper truncate of progress */
    p = pmax;
  }
  x11_fRect(dpy, anim, x + 1, y + 1, p * (w - 2) / pmax, h - 2);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Draw a progress bar (discrete).
 */
/*  void x11_dPBarD(XAnim * anim, int x, int y, */
/*  				  int progress, int nticks) */
/*  { */
/*      int i; */
/*      const int tick_w = 6, tick_h = 8; */
/*      xwin_draw_3drectangle(win, x, y, */
/*  		       nticks*tick_w+2*(nticks-1)+4, tick_h+4, FALSE); */
/*      xwin_set_foreground(win, color_pixel); */
/*      if (progress > nticks) */
/*  	progress = nticks; */
/*      for (i = 0; i < progress; i++) */
/*  	xwin_fill_rectangle(win, x+2+i*(tick_w+2), y+2, tick_w, tick_h); */
/*  } */

/*!
 * Draw a progress bar (discrete2).
 */
/*  void x11_dPBarD2(XAnim * anim, int x, int y, */
/*  			  uint w, uint h, f32 progress) */
/*  { */
/*      int i; */
/*      xwin_draw_3drectangle(win, x, y, w, h, FALSE); */
/*      xwin_set_foreground(win, color_pixel); */
/*      for (i = 0; i < F_ROUND(32*progress/100); i++) */
/*  	xwin_fill_rectangle(win, x+2+i*(w-4)/32, y+2, (w-4)/32-1, h-4); */
/*  } */

/* ---------------------------- Group Separator ---------------------------- */

void
x11_dPBarAlt(const XDpy * dpy, XAnim * anim,
	   const vec2s * start, const ext2s * size, int p, int max)
{
  x11_dRect3Dve(dpy, anim, start, size, FALSE);
  x11_setFG(dpy, &anim->win, dpy->color);
  if (p > max) {
    p = max;
  }
  x11_fRect(dpy, anim, start->x + 1, start->y + 1, p * size->w / max - 2, size->h - 2);
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_dGridReg(const XDpy * dpy, XAnim * anim,
	   int x, int y, uint w, uint h, int nx, int ny)
{
  int i;
  for (i = 0; i < nx + 1; i++) {
    x11_dLine(dpy, anim, x + w * i, y, x + w * i, y + h * ny);
  }
  for (i = 0; i < ny + 1; i++) {
    x11_dLine(dpy, anim, x, y + h * i, x + w * nx, y + h * i);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_dGradX(const XDpy * dpy, XAnim * anim,
	 int x, int y, uint w, uint h,
	 color_t col1, color_t col2)
{
  uint j;
  color_t cols[w];

  colorarray_ramp(cols, w, col1, col2);

  for (j = 0; j < w; j++) {
    x11_setFG(dpy, &anim->win, cols[j]);
    x11_dLine(dpy, anim, x + j, y, x + j, y + (h - 1));
  }
}

void
x11_dGradY(const XDpy * dpy, XAnim * anim,
	 int x, int y, uint w, uint h,
	 color_t col1, color_t col2)
{
  uint j;
  color_t cols[h];

  colorarray_ramp(cols, h, col1, col2);

  for (j = 0; j < h; j++) {
    x11_setFG(dpy, &anim->win, cols[j]);
    x11_dLine(dpy, anim, x, y + j, x + (w - 1), y + j);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_dGradXN(const XDpy * dpy, XAnim * anim,
	  int x, int y, uint w, uint h, CPAL_t cpal)
{
  uint j;
  color_t cols[w];

  CPAL_gen(cols, w, cpal);

  for (j = 0; j < w; j++) {
    x11_setFG(dpy, &anim->win, cols[j]);
    x11_dLine(dpy, anim, x + j, y, x + j, y + (h - 1));
  }
}

void
x11_dGradYN(const XDpy * dpy, XAnim * anim,
	  int x, int y, uint w, uint h, CPAL_t cpal)
{
  uint j;
  color_t cols[h];

  CPAL_gen(cols, h, cpal);

  for (j = 0; j < h; j++) {
    x11_setFG(dpy, &anim->win, cols[j]);
    x11_dLine(dpy, anim, x, y + j, x + (w - 1), y + j);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_dMandelbrotf(const XDpy * dpy, XImg * const img,
	      float xmin, float xmax, float ymin, float ymax)
{
  uint w = img->img->width;
  uint h = img->img->height;

  float xstep = (xmax - xmin) / w;
  float ystep = (ymax - ymin) / h;

  float x0, y0;

  int c, cmax = 128;		/* count, and count maximum */
  color_t cols[cmax];
  color_t col;
  uint px, py;			/* point(x, y) */

  CPAL_gen(cols, cmax, CPAL_HOT);

  for (py = 0, y0 = ymin; py < h; py++, y0 += ystep) {
    for (px = 0, x0 = xmin; px < w; px++, x0 += xstep) {
      complexf z = complexf_init(x0, y0);
      c = complexf_mandelbrot_at(z, 4.0, cmax);

      if (c < cmax) {
	col = cols[c];
      } else {
	col = COLOR_BLACK;
      }

      x11_ximg_setAt_xy(img, px, py, col);
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Draw a recursively defined sierpinksi triangle.
 *
 * fixme: Generalize using recursive linear transformations off coordinates.
 *
 * origin[] = {0.0, 0.0,
 *            1.0, 0.0,
 *            0.5, 0.86602540378444};
 *
 * transform[] = {1.0, 0.0, 0.0,
 *                0.5, 0.5, 0.0,
 *                0.5, 0.0, 0.5};
 */
void
x11_dSierpinski3D(XDpy * dpy, XAnim * anim,
	       int x, int y, uint w, uint h,
	       double x1, double y1,
	       double x2, double y2, double x3, double y3, uint level)
{
  int wm1 = w - 1;
  int hm1 = h - 1;
  static XPoint points[3];	/* faster to put this as static */

  if (level == 0) {
    points[0].x = x + (short) (x1 * wm1);
    points[0].y = y + (short) hm1 - (y1 * hm1);
    points[1].x = x + (short) (x2 * wm1);
    points[1].y = y + (short) hm1 - (y2 * hm1);
    points[2].x = x + (short) (x3 * wm1);
    points[2].y = y + (short) hm1 - (y3 * hm1);

    XFillPolygon(dpy->dpy, anim->xback, anim->win.gc,
		 points, 3, Convex, CoordModeOrigin);
  } else {
    double x1x2 = (x1 + x2) / 2, x2x3 = (x2 + x3) / 2, x3x1 = (x3 + x1) / 2;
    double y1y2 = (y1 + y2) / 2, y2y3 = (y2 + y3) / 2, y3y1 = (y3 + y1) / 2;

    x11_dSierpinski3D(dpy, anim, x, y, w, h,
		   x1, y1, x1x2, y1y2, x3x1, y3y1, level - 1);
    x11_dSierpinski3D(dpy, anim, x, y, w, h,
		   x1x2, y1y2, x2, y2, x2x3, y2y3, level - 1);
    x11_dSierpinski3D(dpy, anim, x, y, w, h,
		   x3x1, y3y1, x2x3, y2y3, x3, y3, level - 1);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_dMtx(XDpy * dpy, XAnim * anim, const short **in,
       int x0, int y0, int w, int h)
{
  char str[10];
  int x, y, cell_w, cell_h;

  x11_setFont(dpy, &anim->win, dpy->nrm_fnt);
  cell_h = 15;
  x11_setBG(dpy, &anim->win, dpy->bg);
  x11_setFG(dpy, &anim->win, dpy->fg);

  /* set cell_w according to type */
  cell_w = 20;

  /* draw sorrounding grid */
  x11_dGridReg(dpy, anim, x0, y0, cell_w, cell_h, w, h);

  /* draw numbers */
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      snprintf(str, sizeof(str), "%*d", (int)(sizeof(str) - 1), in[y][x]);
      x11_dIText(dpy, anim,
	       x0 + cell_w * x + cell_w / 4,
	       y0 + cell_h * y + 3 * cell_h / 4, str, strlen(str));
    }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
x11_waterfall(const XDpy * dpy, const XMap * pxm,
	    int x, int y, int w, int h, double *water, CPAL_t cpal)
{
  color_t cols[h];
  CPAL_gen(cols, h, cpal);
}
