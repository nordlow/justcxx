/* grab.c
 * 19-9-2001, a.martone@retepnet.it
 * grab a TV frame (via video4linux) in TGA-uncompressed-RGB24 format
 * info:     thanks to Gerd Knorr's xawtv sources (www.bytesex.org/xawtv)
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev.h>

int
main(int argc, const char *argv[])
{
  int size;

  /* open video device */

  int dev;
  dev = open("/dev/video0", O_RDONLY);	/* open video device */
  if (dev < 0) {
    perror("open");
  }

  struct video_mmap vmmap;
  vmmap.format = VIDEO_PALETTETYPE_RGB24;	/* GREY, RGB555, RGB565, RGB24, RGB32 */
  vmmap.frame = 0;		/* 0 or 1 (buffer 0 or buffer 1) */
  vmmap.width = 768;		/* from 32 to 924 (sometimes limited to 768) */
  vmmap.height = 576;		/* from 32 to 576 */
  size = vmmap.width * vmmap.height * 3;	/* rgb24:3, rgb32:4, rgb565:2, rgb555:2 */

  /* map device to memory */

  void *area;
  area = mmap(0, size, PROT_READ, MAP_SHARED, dev, 0);	/* select memory-map */
  if (area == MAP_FAILED) {
    perror("mmap");
  }

  /* get frequency */

  ulong freq;
  if (ioctl(dev, VIDIOCGFREQ, &freq) < 0) {
    perror("VIDIOCGFREQ");
  }
  printf("freq: %ld\n", freq);

  /* capability */

  int xmin, xmax, ymin, ymax;
  struct video_capability cap;
  if (ioctl(dev, VIDIOCGCAP, &cap) < 0) {
    perror("VIDIOCGCAP");
  }
  xmin = cap.minwidth, xmax = cap.maxwidth;
  ymin = cap.minheight, ymax = cap.maxheight;
  printf("capture sizes: (%dx%d)-->(%dx%d)\n", xmin, ymin, xmax, ymax);

  /* buffer */

  struct video_buffer vbuf;
  if (ioctl(dev, VIDIOCGFBUF, &vbuf) < 0) {
    perror("VIDIOCGFBUF");
  }
  printf("base:%p, size:(%dx%d), depth:%d, bpl=%d\n", vbuf.base,
	 vbuf.width, vbuf.height, vbuf.depth, vbuf.bytesperline);

  /* picture */

  struct video_picture vpic;
  if (ioctl(dev, VIDIOCGPICT, &vpic) < 0) {
    perror("VIDIOCGPICT");
  }
  printf("brightness:%d, hue:%d, color=%d, contrast=%d\n"
	 "whiteness:%d, depth:%d, palette[%d]\n", vpic.brightness,
	 vpic.hue, vpic.colour, vpic.contrast, vpic.whiteness,
	 vpic.depth, vpic.palette);

  while (1) {
    /* start capturing a frame */
    if (ioctl(dev, VIDIOCMCAPTURE, &vmmap) < 0) {
      perror("VIDIOCMCAPTURE");
    }

    /* wait for end of frame */
    if (ioctl(dev, VIDIOCSYNC, &vmmap.frame) < 0) {
      perror("VIDIOCSYNC");
    }

    printf("got frame\n");
  }

  return 0;
}
