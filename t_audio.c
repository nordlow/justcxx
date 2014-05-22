#include "../xwin.h"
#include "../xplots.h"
#include "../OSSio.h"
#include "../filters.h"
#include "../endian_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <float.h>

/* ========================================================================= */

#define CHANNELS (1)

/* 4000, 8000, 16000, 32000 */
#define RATE (20000)

/*!
 * Sampling Format
 *
 * AFMT_U8
 * AFMT_S8
 * AFMT_S16_LE
 * AFMT_S16_BE
 * AFMT_U16_LE
 * AFMT_U16_BE
 */
#define AFMT (AFMT_S16_LE)

/* unlimited number of fragments of size 2^11 = 256 bytes */
#define FRAG (0x7fff000b)

#define FRAG_LEN (1024)
#define FRAG_SIZE (CHANNELS * 2 * FRAG_LEN)

/* ---------------------------- Group Separator ---------------------------- */

#include <X11/keysym.h>

int
main(int argc, char *argv[])
{
  int f;
  int fd = OSS_open("/dev/audio", CHANNELS, RATE, AFMT, FRAG);
  if (fd < 0) {
    perror("OSS_open");
    return -1;
  }

  box2d taxx, faxx;
  short buf[FRAG_LEN];
  double xdat[FRAG_LEN];
  double ydat[FRAG_LEN];

  XDpy dpy;
  XAnim xanim;
  XFall fall;
  uint w = 800, h = 600;	/* width, height */
  XEvent event;

  x11_openDpy(&dpy);
  x11_anim_open(&dpy, &xanim, w, h, "audio");
  x11_mkFall(&dpy, &xanim.win, &fall,
             w, h - 2*h/3,
             FRAG_LEN, h/2);
  x11_setFont(&dpy, &xanim.win, dpy.nrm_fnt);
  x11_setBG(&dpy, &xanim.win, dpy.bg);
  x11_setFG(&dpy, &xanim.win, dpy.fg);

  /* generate palette */
  int pal_len = 512;
  color_t * pal = malloc(pal_len * sizeof(color_t));
  CPAL_gen(pal, pal_len, CPAL_OCEAN);

  int done = FALSE;
  for (f = 0; ! done; f++) {
    if (read(fd, buf, FRAG_SIZE) != FRAG_SIZE) {
      perror("read");
      close(fd);
      return -1;
    }

    int i;

    /* ============ analyze ============= */

    /* calculate signal sum and energy */
    double sum = 0;
    double energy = 0;
    for (i = 0; i < FRAG_LEN; i++) {
      xdat[i] = (double) i / RATE;
      ydat[i] = INT16_TOGGLE_LE(buf[i]);
      double val = ydat[i];
      sum += val;
      energy += std::sqr(val);
    }

    /* remove DC-level */
    int DCflag = TRUE;
    if (DCflag) {
      double dc = sum / FRAG_LEN;
      for (i = 0; i < FRAG_LEN; i++) {
        ydat[i] -= dc;
      }
    }

    /* extremes */
    double min = DBL_MAX;
    double max = DBL_MIN;
    for (i = 0; i < FRAG_LEN; i++) {
      double val = ydat[i];
      min = MIN2(min, val);
      max = MAX2(max, val);
    }

    /* clear backbuffer */

    x11_anim_clearback(&dpy, &xanim);

    /* plot */

    x11_fplot(&dpy, &xanim, &fall,
              pal, pal_len,
              0, 0, w, h,
              xdat, ydat, FRAG_LEN,
              RATE,
              "time", "value",
              "frequency", "energy",
              FALSE,
              &taxx, &faxx, f);

    /* showback */
    x11_anim_showback(&dpy, &xanim);

    while ((!done) &&
           XCheckWindowEvent(dpy.dpy, xanim.win.win,
                             xanim.win.event_mask, &event)) {

      switch (event.type) {
      case MapNotify:
        break;
      case Expose:
        break;
      case ConfigureNotify:
        w = event.xconfigure.width;
        h = event.xconfigure.height;
        x11_rszFall(&dpy, &xanim.win, &fall, w, h - 2*h/3);
        break;
      case KeyPress:
        switch (XKeycodeToKeysym(dpy.dpy, event.xkey.keycode, 0)) {
        case XK_q:
          done = TRUE;
          break;
        }
        break;
      default:
        break;
      }
    }
  }

  free(pal);

  x11_clearFall(&dpy, &fall);
  x11_anim_close(&dpy, &xanim);
  x11_closeDpy(&dpy);

  close(fd);

  return 0;
}
