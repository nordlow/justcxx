#include "tga.h"

#include <stdio.h>
#include <string.h>

/* TGA uncompressed RGB24 */
void
fwriteTGA_UC_RGB24(FILE * file, int width, int height, char *img, int sz)
{
  short hdr[9] = {
    0, 2, 0, 0, 0, 0,
    6666, 7777,			/* are later set to width and height  */
    8216
  };

  hdr[6] = width;
  hdr[7] = height;		/* update targa header for frame */
  memcpy(hdr, hdr, 9 * sizeof(short));

  fwrite(hdr, sizeof(hdr), 1, file);
  fwrite(img, sz, 1, file);
}
