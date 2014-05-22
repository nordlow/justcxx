#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "color.hpp"
#include "color_constants.hpp"
#include "xwin.h"
#include "ximg.h"
#include "pixels.h"
#include "xplots.h"
#include "xdraw.h"
#include "sleepers.h"
#include "rangerand.h"

/*!
 * Illustrates how to access the root window on XWindows.
 */
int
test_xrootwin(void)
{
  Display *dpy;
  Window win;
  int scrnum;
  GC gc;
  const char *name;
  ulong black_pixel, white_pixel;
  XEvent event;
  long mask;
  uint bordw, depth;
  Window rwin;
  Pixmap pxm;

  int len;
  int i;
  double *xp, *yp;
  double *xv, *yv;
  double xpmin, xpmax, ypmin, ypmax;
  int x, y, w, h;
  XArc *pnt, *opnt;

  void update(void)
  {
    puts("update");

    for (i = 0; i < len; i++) {
      opnt[i].x = pnt[i].x;
      opnt[i].y = pnt[i].y;

      if (xp[i] + xv[i] < xpmin || xp[i] + xv[i] > xpmax) {
	xv[i] *= -1;
      }
      xp[i] += xv[i];

      if (yp[i] + yv[i] < ypmin || yp[i] + yv[i] > ypmax) {
	yv[i] *= -1;
      }
      yp[i] += yv[i];

      pnt[i].x = F_ROUND((xp[i] - xpmin) * (w - 1) / (xpmax - xpmin));
      pnt[i].y = F_ROUND((yp[i] - ypmin) * (h - 1) / (ypmax - ypmin));

    }

  }

  void redraw(void)
  {
    puts("redraw");

    XSetForeground(dpy, gc, black_pixel);
    XFillArcs(dpy, pxm, gc, opnt, len);

    XSetForeground(dpy, gc, white_pixel);
    XFillArcs(dpy, pxm, gc, pnt, len);

    event.xexpose.x = 0;
    event.xexpose.y = 0;
    event.xexpose.width = w;
    event.xexpose.height = h;

    event.type = Expose;
    XSendEvent(dpy, win, True, ExposureMask, &event);
  }

  name = getenv("DISPLAY");
  if (name == NULL) {
    name = ":0.0";
  }
  if (!(dpy = XOpenDisplay(name))) {
    leprintf("cannot open display '%s'\n", XDisplayName(name));
    return -1;
  }

  scrnum = DefaultScreen(dpy);
  gc = DefaultGC(dpy, scrnum);
  win = DefaultRootWindow(dpy);
  mask = ExposureMask;
  XSelectInput(dpy, win, mask);

  black_pixel = BlackPixel(dpy, scrnum);
  white_pixel = WhitePixel(dpy, scrnum);

  XGetGeometry(dpy, win, &rwin, &x, &y, &w, &h, &bordw, &depth);
  XClearArea(dpy, win, 0, 0, w, h, False);

  pxm = XCreatePixmap(dpy, win, w, h, DefaultDepth(dpy, scrnum));
  XSetForeground(dpy, gc, black_pixel);
  XFillRectangle(dpy, pxm, gc, 0, 0, w, h);

  len = 32;

  xp = darray_malloc(len);
  yp = darray_malloc(len);
  xv = darray_malloc(len);
  yv = darray_malloc(len);
  pnt = malloc(len * sizeof(XArc));
  opnt = malloc(len * sizeof(XArc));

  xpmin = 0;
  xpmax = 1;
  ypmin = 0;
  ypmax = 1;

  for (i = 0; i < len; i++) {
    xp[i] = double_rangerand(xpmin, xpmax);
    yp[i] = double_rangerand(ypmin, ypmax);
    xv[i] = double_rangerand(-(xpmax - xpmin) / 32, (xpmax - xpmin) / 32);
    yv[i] = double_rangerand(-(ypmax - ypmin) / 32, (ypmax - ypmin) / 32);

    pnt[i].width = 7;
    pnt[i].height = 7;
    pnt[i].angle1 = 0;
    pnt[i].angle2 = 64 * 360;

    opnt[i].width = 7;
    opnt[i].height = 7;
    opnt[i].angle1 = 0;
    opnt[i].angle2 = 64 * 360;
  }

  while (1) {
    pnw_usleep(1000 * 1000 / 60);

    update();
    redraw();

    while (XCheckWindowEvent(dpy, win, ExposureMask | KeyPressMask, &event)) {

      switch (event.type) {
      case Expose:
	puts("Expose");
	XCopyArea(dpy, pxm, win, gc, 0, 0, w, h, 0, 0);
	break;
      case KeyPress:
	goto Ldone;
	break;
      }
    }
  }

Ldone:
  XClearArea(dpy, win, 0, 0, w, h, True);

  XFreePixmap(dpy, pxm);

  free(xp);
  free(yp);
  free(xv);
  free(yv);
  free(pnt);
  free(opnt);

  return 0;
}
