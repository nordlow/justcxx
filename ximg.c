#include "utils.h"
#include "convert.h"
#include "ximg.h"
#include "pnmio.h"
#include "color.hpp"
#include "color_constants.hpp"
#include "xwin.h"
#include <stdlib.h>

/* ---------------------------- Group Separator ---------------------------- */

#ifdef PNW_USE_MITSHM
#define __USE_XOPEN		/* prevent error message */
#include <sys/types.h>
#include <sys/ipc.h>		/* needed by MIT-SHM */
#include <sys/shm.h>		/* needed by MIT-SHM */
#include <unistd.h>		/* needed by MIT-SHM */
#endif

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \NOTE At last I got this beast to work!.
 * When doing \c XShmCreateImage (compared to \c XCreateImage) the client
 * cannot decide the number of bytes per line the images is stored but
 * its sets its member \c bytes_per_line itself.
 *
 * On my Athlon XP for example the quantum of this scanline is 4 bytes even
 * at a 16-bit depth.
 */
int
x11_initImg(Display * dpy, XImg * img, uint w, uint h,
	  int try_shm)
{
  int pad;     /* bitmap pad, quantum of a scanline (8, 16, or 32). */
  int bsz;			/* byte size of image data */
  int show = FALSE;

  img->pconv = x11_gpconv(dpy);

  int scr_num = DefaultScreen(dpy);
  /* Screen *scr = ScreenOfDisplay(dpy, scr_num); */
  int scr_depth = DefaultDepth(dpy, scr_num);
  Visual *vis = DefaultVisual(dpy, scr_num);

  switch (scr_depth) {
  case 8:
    pad = 8;
    bsz = w * h * 1;
    break;
  case 15:			/* probably RGB555 */
  case 16:			/* probably RGB565 */
    pad = 16;
    bsz = w * h * 2;
    break;
  case 24:
  case 32:
    pad = 32;
    if (XBitmapUnit(dpy) == 24) {
      bsz = w * h * 3;
    } else if (XBitmapUnit(dpy) == 32) {
      bsz = w * h * 4;
    } else {
      leprintf("Cannot handle bitmap unit %d.\n", XBitmapUnit(dpy));
      return -1;
    }
    break;
  default:
    leprintf("Cannot handle screen depth %d.\n", scr_depth);
    return -1;
    break;
  }

  int image_depth = scr_depth;

  /* If XYBitmap format is used, the depth of the image must be one, or
     a BadMatch error(in XPutImage) occurs.  The foreground pixel in
     the GC defines the source for the one bits in the image, and the
     background pixel defines the source for the zero bits. */

  /* For XYPixmap and ZPixmap, the depth of the image must match the
   * depth of the drawable, or a BadMatch error(in XPutImage) occurs. */
  int format = image_depth == 1 ? XYBitmap : ZPixmap;

#ifdef PNW_USE_MITSHM

  int ret;
  if (try_shm &&
      XQueryExtension(dpy, "MIT-SHM", &ret, &ret, &ret) &&
      XShmQueryExtension(dpy)) {
    img->shm_flag = TRUE;	/* OK, we have shared memory */
    img->shm_locked = 0;	/* so initialize to unlocked */

    /***
     * Create It
     *
     * Note, however, that there are no offset, bitmap_pad,
     * or bytes_per_line arguments. These quantities will be defined
     * by the server itself, and your code needs to abide by them.
     *
     * Unless you have already allocated the shared memory segment
     * (see below), you should pass in NULL for the ``data'' pointer.
     */
    img->img = XShmCreateImage(dpy, vis, image_depth, format, 0, /* no data yet */
			       &img->shminfo, w, h);
    if (!img->img) {
      perror("XShmCreateImage");
      return -1;
    }

    /* \NOTE we must use this measure instead of bsz!!!! */
    int imgsz = img->img->bytes_per_line * img->img->height;

    /* allocate shared memory segment for the image data */

    img->shminfo.shmid = shmget(IPC_PRIVATE,	/* key */
				imgsz,	/* number of bytes */
				0777 |	/* permissions */
				IPC_CREAT |	/* create it */
				IPC_EXCL); /* ensure failure if segments already exists */
    if (img->shminfo.shmid == -1) {
      perror("shmget");
      return -1;
    }

    /* inform the server about the shared memory segment */

    img->shminfo.shmaddr = img->img->data = shmat(img->shminfo.shmid, 0, 0);
    img->shminfo.readOnly = False;

    ret = XShmAttach(dpy, &img->shminfo);
    if (!ret) {
      perror("XShmAttach");
      return -1;
    }

    /* mark this shm segment for deletion at once. the segment will
     * automatically become released when both the server and this
     * client have detached from it. process termination automagically
     * detaches shm segments. */
    ret = shmctl(img->shminfo.shmid, IPC_RMID, 0);
    if (ret == -1) {
      perror("shmctl");
    }

    XSync(dpy, False);

    /* use the shared memory XImage, much like a normal one */

    if (show) {
      lprintf("shared memory XImage");
    }
  } else
#endif
  {
    img->shm_flag = FALSE;	/* we cannot use shared memory */
    img->shm_locked = 0;	/* still initialize to unlocked */

    /* number of pixels to ignore at the beginning of the scanline. */
    int offset = 0;

      /***
       * Number of bytes per line in the client image.
       *
       * If you pass a zero value Xlib assumes that the
       * scanlines are contiguous in memory and calculates the value of
       * bpl itself.
       */
    int bpl = 0;

    img->img = XCreateImage(dpy, vis, image_depth, format, offset, 0,	/* no data yet */
			    w, h, pad, bpl);

    if (!img->img) {
      perror("XCreateImage");
      return -1;
    }

    /* \NOTE we must use this measure instead of bsz!!!! */
    int imgsz = img->img->bytes_per_line * img->img->height;

    /* allocate image data */

    img->img->data = malloc(imgsz);

    if (show) {
      lprintf("normal XImage");
    }
  }

  /* read stuff that is common to shared and non-shared memory images */

  /* got this from file "gdkimage-x11.c" in sources of gtk+-2.4.0 */
  img->bytes_per_pixel = (img->img->bits_per_pixel + 7) / 8;

  if (show) {
    printf(" with bpl=%d width=%d height=%d bpp=%d width*bpp=%d\n",
	   img->img->bytes_per_line,
	   img->img->width,
	   img->img->height,
	   img->bytes_per_pixel, img->img->width * img->bytes_per_pixel);
  }

  return 0;
}

XImg
x11_csubImg(const XImg * img, int x, int y, uint w, uint h)
{
  XImg sub;

  PTEST("Added initialization of sub's members"
	   " shm_locked, bytes_per_pixel, pconv.byteswap, pconv.fmt");

  sub.shm_flag = FALSE;
  sub.shm_locked = FALSE;
  sub.bytes_per_pixel = img->bytes_per_pixel;
  sub.pconv.byteswap = img->pconv.byteswap;
  sub.pconv.fmt = img->pconv.fmt;

  sub.img = XSubImage(img->img, x, y, w, h);

  return sub;
}

#ifdef PNW_USE_MITSHM
static inline void
shm_release(Display * dpy, XImg * img)
{
  XSync(dpy, False);
  XShmDetach(dpy, &img->shminfo);
  shmdt(img->shminfo.shmaddr);
  shmctl(img->shminfo.shmid, IPC_RMID, NULL);
}
#endif

int
x11_clearImg(Display * dpy, XImg * img)
{
#ifdef PNW_USE_MITSHM
  if (img->shm_flag) {
    if (! img->shm_locked) {
      shm_release(dpy, img);
      XDestroyImage(img->img);	/* frees img->img->data and img->img */
      return 1;
    } else {
      /* image is locked so do nothing */
      return -1;
    }
  } else
#endif
  {
    XDestroyImage(img->img);	/* frees img->img->data and img->img */
    return 1;
  }
}

void
x11_lockImg(XImg * img)
{
#ifdef PNW_USE_MITSHM
  if (img->shm_flag &&
      ! img->shm_locked) {
    img->shm_locked = 1;
    /* lprintf("img:%p\n", img); */
  }
#endif
}

void
x11_unlockImg(XImg * img)
{
#ifdef PNW_USE_MITSHM
  if (img->shm_flag &&
      img->shm_locked) {
    img->shm_locked = 0;
    /* lprintf("img:%p\n", img); */
  }
#endif
}

int
x11_rszImg(Display * dpy, XImg * img, uint w, uint h)
{
  if (w != (uint)img->img->width ||
      h != (uint)img->img->height) {
    if (img->shm_flag &&
	img->shm_locked) {
      /* leprintf("LOCKED\n"); */
      return -1;
    } else {
      if (FALSE) {
	lprintf("Image resize from w:%d X h:%d to w:%d X h:%d\n",
		img->img->width, img->img->height, w, h);
      }
      x11_clearImg(dpy, img);
      x11_initImg(dpy, img, w, h, img->shm_flag);
      return 1;
    }
  } else {
    return 0;
  }
}

void
x11_regputImg(Display * dpy, Drawable win, GC gc, const XImg * img,
              int x, int y, uint w, uint h)
{
#ifdef PNW_USE_MITSHM
  if (img->shm_flag) {
    /* if the last argument is set to True the server will generate a
     * "completion" event when the image write is complete; thus your
     * program can know when it is safe to begin manipulating the shared
     * memory segment again.
     *
     * XShmGetEventBase(Display * dpy) + ShmCompletion (defined in XShm.h)
     * gets this completion event value.
     *
     * See the event XShmCompletionEvent.
     */
    XShmPutImage(dpy, win, gc, img->img, 0, 0, x, y, w, h, True);
    x11_lockImg((XImg*)img);
  } else
#endif
  {
    XPutImage(dpy, win, gc, img->img, 0, 0, x, y, w, h);
  }
}

void
x11_putImg(Display * dpy, Drawable win, GC gc, const XImg * img, int x, int y)
{
  x11_regputImg(dpy, win, gc, img, x, y, img->img->width, img->img->height);
}

void
x11_grayImg(XImg * img, const uchar *data)
{
  uint x, y;
  const uint w = img->img->width;
  const uint h = img->img->height;

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      uint i = x + y * w;
      x11_ximg_setAt_xy(img, x, y, COLOR_GRAY(data[i]));
    }
  }
}

void
x11_rgbImg(XImg * img, const uchar *data)
{
  uint x, y;
  const uint w = img->img->width;
  const uint h = img->img->height;

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      uint i = x + y * w;
      x11_ximg_setAt_xy(img, x, y,
		    color_genRGB(data[3 * i + 0],
			      data[3 * i + 1],
			      data[3 * i + 2]));
    }
  }
}

void
x11_colorImg(XImg * img, const color_t *data)
{
  uint x, y;
  const uint w = img->img->width;
  const uint h = img->img->height;

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      uint i = x + y * w;
      x11_ximg_setAt_xy(img, x, y, data[i]);
    }
  }
}

void
x11_nicegrad(XImg * img, uint c)
{
  uint x, y;
  uint w = img->img->width;
  uint h = img->img->height;

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      x11_ximg_setAt_xy(img, x, y, color_genRGB(2 * (x + c) + y, x, 3 * x - y));
    }
  }
}

void
x11_testimg1(XImg * img)
{
  uint n = img->img->width * img->img->height;
  uint i;
  uchar data[n];

  for (i = 0; i < n; i++) {
    data[i] = i % img->img->width;
  }

  x11_grayImg(img, data);
}

int
x11_cimgPGM(Display * dpy, XImg * img, const char *filename)
{
  uint w, h;
  uchar *data;

  FILE *file;

  file = fopen(filename, "rb");
  if (file == NULL) {
    leprintf("could open file %s\n", filename);
    return -1;
  }

  freadPGM(&w, &h, &data, file);

  x11_initImg(dpy, img, w, h, TRUE);
  x11_grayImg(img, data);

  free(data);
  fclose(file);

  return 0;
}

int
x11_cimgPPM(Display * dpy, XImg * img, const char *filename)
{
  uint w, h;
  uchar *data;
  FILE *file;

  file = fopen(filename, "rb");
  if (file == NULL) {
    leprintf("Could open file %s\n", filename);
    return -1;
  }

  freadPPM(&w, &h, &data, file);

  x11_initImg(dpy, img, w, h, TRUE);
  x11_rgbImg(img, data);

  free(data);
  fclose(file);

  return 0;
}
