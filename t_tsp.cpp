#include "xwin.h"
#include "reorg.h"
#include "qsort.h"
#include "crm2enum.h"
#include "xplots.h"

#include <float.h>
#include <stdlib.h>

void
darray_RectGrid2D(double *x, double *y,
		double xoffs, double yoffs,
		double xstep, double ystep, int m, int n)
{
  int i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      x[i * n + j] = xoffs + (double) j *xstep;
      y[i * n + j] = yoffs + (double) i *ystep;
    }
  }
}

/*!
 * Travelling Salesman Problem Solver
 */
void
tspsolve(void)
{
  XDpy dpy;
  XWin win;
  uint w = 256, h = 256, p = 0;	/* width, height, padding */

  int i;

  int popsz = 128;		/* population size */
  int **pop;			/* population */
  int **impop;			/* intermediate population */

  int datlen;
  double *xdat, *ydat;
  double *xdat_tmp, *ydat_tmp;

  double *vals;
  double prev_minval;
  double minval, maxval;

  double *fits;
  double tfsum;

  int *idx;
  int *goodidx;

  int gencnt;

  XEvent event;

  void ga_init(void)
  {
    int side = 16;

    prev_minval = DBL_MAX;

    datlen = side * side;

    xdat = darray_malloc(datlen);
    ydat = darray_malloc(datlen);

/*          darray_rangerand(xdat, datlen, 0, 1); darray_rangerand(ydat, datlen, 0, 1); */

    darray_RectGrid2D(xdat, ydat, 0, 0, 1, 1, side, side);
/*          darray_Polygon2D(xdat, ydat, 0, 0, 1, 1, datlen); */
/*          darray_flowerspiral(xdat, ydat, datlen, 1.0, 8.0, 0.0); */

    xdat_tmp = darray_malloc(datlen);
    ydat_tmp = darray_malloc(datlen);

    pop = intAA_malloc(popsz, datlen);
    impop = intAA_malloc(popsz, datlen);
    for (i = 0; i < popsz; i++) {
/*              intarray_random_permutation(pop[i], datlen); */
      intarray_ramp(pop[i], datlen);
      darray_qtidx(pop[i], xdat, ydat, datlen);
    }

    vals = darray_malloc(popsz);
    fits = darray_malloc(popsz);
    idx = intarray_malloc(popsz);
    goodidx = intarray_malloc(popsz);
    intarray_ramp(idx, popsz);

    gencnt = 0;
  }

  void ga_clear(void)
  {
    free(xdat);
    free(ydat);

    free(xdat_tmp);
    free(ydat_tmp);

    intmatrix_free(pop);
    intmatrix_free(impop);

    free(vals);
    free(fits);
    free(idx);
    free(goodidx);
  }

  void ga_calcvals1(void)
  {
    int i;
    prev_minval = minval;
    minval = DBL_MAX;
    maxval = DBL_MIN;
    for (i = 0; i < popsz; i++) {
      vals[i] = darray_RTDist2D(xdat, ydat, pop[i], datlen);
      minval = MIN2(minval, vals[i]);
      maxval = MAX2(maxval, vals[i]);
    }
  }

  void ga_calcfits1(void)
  {
    int i;
    tfsum = 0;
    for (i = 0; i < popsz; i++) {
      tfsum += (fits[i] = std::sqr(maxval - vals[i]));
    }
  }

  void ga_calcfits2(void)
  {
    int i;
    tfsum = 0;
    for (i = 0; i < popsz; i++) {
      tfsum += (fits[i] = std::sqr(vals[i] - minval));
    }
  }

  void ga_shufflegoodidx(void)
  {
    intarray_shuffle(goodidx, popsz);
  }

  void ga_copy(void)
  {
    int i;
    for (i = 0; i < popsz; i++) {
      intarray_copy(impop[i], pop[goodidx[i]], datlen);
    }
  }

  void ga_sortfits(void)
  {
    intarray_ramp(idx, popsz);
    darray_iqsort(fits, idx, popsz);
  }

  /* Mutation rate should typically < 1 %. */
  void ga_mutate(int **pop, int ci)
  {
    if ((rand() % 200) == 0) {
      intarray_partial_shuffle(pop[ci], datlen, 1);
    }
    if ((rand() % 10) == 0) {
      intarray_rsflip(pop[ci], datlen);
    }
    if ((rand() % 10) == 0) {
      intarray_crsswap(pop[ci], datlen);
    }
  }

  void ga_fastmate(void)
  {
    for (i = 0; i < popsz / 2; i++) {
      int ret;
      int pi0, pi1, ci;		/* two parents, one child */

      ci = idx[i];

      pi0 = idx[popsz / 2 + rand() % (popsz / 2)];
      pi1 = idx[popsz / 2 + rand() % (popsz / 2)];

      ret = intarray_crm2enum(pop[ci], pop[pi0], pop[pi1], datlen);

      ga_mutate(pop, ci);
    }
  }

  void ga_susmate(void)
  {
    darray_sus(goodidx, popsz, fits, tfsum);
    ga_shufflegoodidx();
    ga_copy();

    for (i = 0; i < popsz / 4; i++) {
      int ret;
      int pi0, pi1, ci0, ci1;	/* two parents, two childs */

      ci0 = idx[2 * i + 0];
      ci1 = idx[2 * i + 1];

      pi0 = goodidx[2 * i + 0];
      pi1 = goodidx[2 * i + 1];

      ret = intarray_crm2enum(pop[ci0], impop[pi0], impop[pi1], datlen);

      ret = intarray_crm2enum(pop[ci1], impop[pi0], impop[pi1], datlen);

      ga_mutate(pop, ci0);
      ga_mutate(pop, ci1);
    }
  }

  void ga_evolve(void)
  {

    ga_calcvals1();
    ga_calcfits1();
    ga_sortfits();

/*         printf("\n"); */
/*         printf("fits\n"); */
/*         darray_fprint(stdout, fits, popsz); */
/*         printf("pop\n"); */
/*         intmatrix_fprint(stdout, pop, popsz, datlen); */
/*         printf("impop\n"); */
/*         intmatrix_fprint(stdout, impop, popsz, datlen); */

/*          ga_fastmate(); */
    ga_susmate();

    gencnt++;
  }

  void Redraw(void)
  {
    char buf[64];

    darray_perm(xdat_tmp, xdat, pop[idx[0]], datlen);
    darray_perm(ydat_tmp, ydat, pop[idx[0]], datlen);
    x11_plot(&dpy, &win, p, p, w - 2 * p, h - 2 * p,
	   xdat_tmp, ydat_tmp, datlen, "X", "Y");

    snprintf(buf, sizeof(buf),
	     "G: %d  min: %.3f  max: %.3f", gencnt, minval, maxval);
    x11_setFont(&dpy, &win, dpy.bld_fnt);
    x11_setFG(&dpy, &win, dpy.white);
    x11_fText(&dpy, &win, 48, 16, buf, strlen(buf));

    XFlush(dpy.dpy);
  }

  x11_openDpy(&dpy);
  x11_openwin(&dpy, &win, w, h, "x_mating");
  x11_setFont(&dpy, &win, dpy.nrm_fnt);
  x11_setBG(&dpy, &win, dpy.bg);
  x11_setFG(&dpy, &win, dpy.fg);

  ga_init();

  while (TRUE) {
    ga_evolve();

    if (minval < prev_minval) {
      Redraw();
    }

    while (XCheckWindowEvent(dpy.dpy, win.win, win.event_mask, &event)) {

      switch (event.type) {
      case MotionNotify:
	x11_skipMotions(&dpy, &win, &event);
	break;
      case ButtonPress:
	Redraw();
	break;
      case ButtonRelease:
	Redraw();
	break;
      case MapNotify:
	break;
      case Expose:
	Redraw();
	break;
      case ConfigureNotify:
	w = event.xconfigure.width;
	h = event.xconfigure.height;
	break;
      case KeyPress:
	switch (XKeycodeToKeysym(dpy.dpy, event.xkey.keycode, 0)) {
	case XK_q:
	  goto done;
	  break;
	case XK_r:
	  Redraw();
	default:
	  break;
	}
	break;
      default:
	break;
      }
    }
  }
done:

  ga_clear();

  x11_closewin(&dpy, &win);
  x11_closeDpy(&dpy);
}

int
main(int argc, char *argv[])
{
  tspsolve();
  return 0;
}
