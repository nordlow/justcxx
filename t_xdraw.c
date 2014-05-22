#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../utils.h"
#include "../timing.h"
#include "../color.hpp"
#include "../color_constants.hpp"
#include "../xwin.h"
#include "../ximg.h"
#include "../pixels.h"
#include "../xplots.h"
#include "../xdraw.h"
#include "../pnmio.h"
#include "../stdio_x.h"
#include "../meman.h"

#include <X11/keysym.h>

/* ========================================================================= */

static int show_tmr = TRUE;

/*!
 * A test function for my X11 helper functions.
 */
void
test_many(int w, int h, int p)
{
  XDpy dpy;
  XAnim xanim;
  int l = 1 << 11;

  double *xdata;		/* buffer */
  double *ydata;		/* buffer */

  XEvent event;
  uint progress = 0;
  double begin = -1e-6, end = 1e-6;

  x11_openDpy(&dpy);
  x11_anim_open(&dpy, &xanim, w, h, GPFUN);

  xdata = darray_malloc(l);
  ydata = darray_malloc(l);

  x11_setFont(&dpy, &xanim.win, dpy.nrm_fnt);
  x11_setBG(&dpy, &xanim.win, dpy.bg);
  x11_setFG(&dpy, &xanim.win, dpy.fg);

  pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);
  int cnt = 0;

  CPAL_t palcnt = CPAL_FIRST;

  while (TRUE) {
    XNextEvent(dpy.dpy, &event);
    /*          x11_printEvent(&event); */
    switch (event.type) {
    case MapNotify:
      break;
    case Expose:
      /* x11_anim_back2front(&dpy, &xanim); */
      break;
    case ConfigureNotify:
      w = event.xconfigure.width;
      h = event.xconfigure.height;
      /* printf("%d x %d\n", w, h); */
      break;
    case KeyPress:
      x11_anim_clearback(&dpy, &xanim);

      x11_dFrame3D(&dpy, &xanim,
		 p - 2, p - 2, w - 2 * (p - 2), h - 2 * (p - 2), TRUE);

      switch (XKeycodeToKeysym(dpy.dpy, event.xkey.keycode, 0)) {
      case XK_q:
	goto done;
	break;
      case XK_c:
	break;
      case XK_p:{
	pTimer t; ptimer_init(&t, CLOCK_PROCESS_CPUTIME_ID);
	begin *= 1.2;
	end *= 1.2;
	/*                  printf("%g %g\n", begin, end); */
	darray_flowerspiral(xdata, ydata, l, 1.0, 8.0, 0.0);
	/*                  darray_coswave(ydata, l, 1.0, 1.0, 0.0); */
	/*                  darray_rangerand(xdata, l, begin, end); */
	/*                  darray_rangerand(ydata, l, begin, end); */
	/*                  darray_print(ydata, l); */
	/*                  darray_ramp(xdata, l, 0.0, 0.1); */
	/*                  darray_ramp(ydata, l, 0.0, 0.1); */
	ptimer_tic(&t);
	x11_plot_autoaxis(&dpy, &xanim, p, p, w - 2*p, h - 2*p,
			xdata, ydata, l, "X", "Y");
	ptimer_toc(&t);
	siunit_show(ptimer_get_sec(&t));
	puts("");
	break;
      }
      case XK_u:{
	pTimer t; ptimer_init(&t, CLOCK_PROCESS_CPUTIME_ID);
	int i;
	begin *= 1.2;
	end *= 1.2;
	darray_ramp(xdata, l, 0.0, M_2PI / l);
	darray_coswave(ydata, l, 4.0, 16.0, 0.0);
	ydata[0] = -10;
	for (i = 0; i < l; i++) {
	  ydata[i] += 2;
	}
	ptimer_tic(&t);
	x11_cplot(&dpy, &xanim, p, p, w - 2*p, h - 2*p,
		xdata, ydata, l, "X", "Y");
	ptimer_toc(&t);
	siunit_show(ptimer_get_sec(&t));
	puts("");
	break;
      }
      case XK_b:
	x11_setFG(&dpy, &xanim.win, dpy.fg);
	darray_rangerand(ydata, l, 0.0, 1.0);
	x11_bars1d(&dpy, &xanim, p, p, w - 2*p, h - 2*p, ydata, l);
	break;
      case XK_k:
	x11_setFG(&dpy, &xanim.win, COLOR_BLACK);
	x11_dSierpinski3D(&dpy, &xanim, p, p, w - 2*p, h - 2*p,
		       0.0, 0.0, 1.0, 0.0, 0.5, 1.0, 7);
	break;
      case XK_t:{
	uint bw = w - 2*p, bh = h - 2*p;
	Bitvec b;
	bitvec_init(&b, bw * bh);
	bitvec_rand(&b);
	x11_btv2d(&dpy, &xanim, p, p, bw, bh, &b);
	bitvec_clear(&b);
	break;
      }
      case XK_g:{
	XImg img;

	x11_initImg(dpy.dpy, &img, w, h, TRUE);
	puts("mk done");

	x11_testimg1(&img);
	puts("test done");

	pTimer t; ptimer_init(&t, CLOCK_PROCESS_CPUTIME_ID);
	ptimer_tic(&t);
	int i;
	int n = 100;
	for (i = 0; i < n; i++) {
	  x11_putImg(dpy.dpy, xanim.xback, xanim.win.gc, &img, 0, 0);
	  XSync(dpy.dpy, False);
	}
	ptimer_toc(&t);
	puts("put done");

	double secs = ptimer_get_sec(&t) / n;
	printf("%.0f us  %.1f MB/s\n",
	       secs * M_MEGA, (double) w * h * 2 / secs / M_MB);

	x11_clearImg(dpy.dpy, &img);
	puts("clr done");

	break;
      }

      case XK_i:{
	XImg img;
	if (x11_cimgPGM(dpy.dpy, &img, "images/marble2.pgm") >= 0) {
	  x11_putImg(dpy.dpy, xanim.xback, xanim.win.gc, &img, p, p);
	  x11_clearImg(dpy.dpy, &img);
	}
	break;
      }

      case XK_j:{
	XImg img;
	if (x11_cimgPPM(dpy.dpy, &img, "images/teapot.ppm") >= 0) {
	  x11_putImg(dpy.dpy, xanim.xback, xanim.win.gc, &img, p, p);
	  x11_clearImg(dpy.dpy, &img);
	}
	break;
      }

      case XK_m:{
	int mx = p, my = p, mw = w - 2*p, mh = h - 2*p;
	XImg img;

	if (mw > 0 && mh > 0) {
	  x11_initImg(dpy.dpy, &img, mw, mh, TRUE);

	  ptimer_tic(&tmr);
	  x11_dMandelbrotf(&dpy, &img, -2.25, 0.75, -1.25, 1.25);
	  ptimer_toc(&tmr);
	  if (show_tmr) {
	    printf("x11_dMandelbrotf: ");
	    ptimer_print_sec_usec9(tmr);
	    printf("s\n");
	  }

	  x11_putImg(dpy.dpy, xanim.xback, xanim.win.gc, &img, mx, my);
	}
	break;
      }

      case XK_n:{
	int mx = p, my = p, mw = w - 2*p, mh = h - 2*p;
	XImg img;

	if (mw > 0 && mh > 0) {
	  x11_initImg(dpy.dpy, &img, mw, mh, TRUE);

	  ptimer_tic(&tmr);
	  x11_nicegrad(&img, cnt);
	  ptimer_toc(&tmr);
	  if (show_tmr) {
	    printf("nicegrad: ");
	    ptimer_print_sec_usec9(tmr);
	    printf("s\n");
	  }

	  x11_putImg(dpy.dpy, xanim.xback, xanim.win.gc, &img, mx, my);

	  cnt++;
	}

	break;
      }

      case XK_o:
	XSetLineAttributes(dpy.dpy,
			   xanim.win.gc, 1, LineSolid, CapButt, JoinBevel);
	{
	  int i;
	  int n = 1000;
	  pTimer t; ptimer_init(&t, CLOCK_PROCESS_CPUTIME_ID);

	  ptimer_tic(&t);

	  for (i = 0; i < n; i++) {
	    int x0 = rand() % w, y0 = rand() % w;
	    int x1 = rand() % w, y1 = rand() % w;
	    x11_dRect(&dpy, &xanim,
		    MIN2(x0, x1), MIN2(y0, y1), ABS(x0 - x1), ABS(y0 - y1));
	  }

	  ptimer_toc(&t);
	  siunit_show(ptimer_get_sec(&t));
	  printf("\n");

	  x11_setFG(&dpy, &xanim.win, COLOR_BLACK);

	}
	break;

      case XK_r:
	x11_dPBar(&dpy, &xanim, p, p, w - 2*p, (h - 2*p) / 8, progress, 100);
	progress = (progress + 1) % 101;
	break;
      case XK_x:
	ptimer_tic(&tmr);
	x11_dGradXN(&dpy, &xanim, p, p, w - 2*p, h - 2*p, palcnt);
	palcnt++; if (palcnt == CPAL_NUM) { palcnt = CPAL_FIRST; }
	ptimer_toc(&tmr);
	siunit_show(ptimer_get_sec(&tmr)); endline();
	break;
      case XK_y:
	ptimer_tic(&tmr);
	x11_dGradYN(&dpy, &xanim, p, p, w - 2*p, h - 2*p, palcnt);
	palcnt++; if (palcnt == CPAL_NUM) { palcnt = CPAL_FIRST; }
	ptimer_toc(&tmr);
	siunit_show(ptimer_get_sec(&tmr)); endline();
	break;
      default:
	break;
      }

      x11_anim_showback(&dpy, &xanim);

      break;
    default:
      break;
    }
  }
 done:

  x11_anim_close(&dpy, &xanim);
  x11_closeDpy(&dpy);

  free(xdata);
  free(ydata);
}

int
main(int argc, char *argv[])
{
  test_many(640, 480, 5);
  return 0;
}
