#include <stdio.h>
#include <stdlib.h>

int
yuv2rgb(int y, int u, int v)
{
  unsigned int pixel32;
  unsigned char *pixel = (unsigned char *) &pixel32;
  int r, g, b;
#if 0
  /*
     One formula I found:  (not the right one)
     R = 1.164(Y - 16) + 1.596(Cr - 128)
     G = 1.164(Y - 16) - 0.813(Cr - 128) - 0.391(Cb - 128)
     B = 1.164(Y - 16)                   + 2.018(Cb - 128)
   */

  r = (1.164 * (y - 16))
    + (2.018 * (v - 128));
  g = (1.164 * (y - 16))
    - (0.813 * (u - 128))
    - (0.391 * (v - 128));
  b = (1.164 * (y - 16))
    + (1.596 * (u - 128));
#else
  /*
     Another formula I found:  (seems to work)
     R = Y + 1.370705 (V-128)
     G = Y - 0.698001 (V-128) - 0.337633 (U-128)
     B = Y + 1.732446 (U-128)
   */
  r = y + (1.370705 * (v - 128));
  g = y - (0.698001 * (v - 128)) - (0.337633 * (u - 128));
  b = y + (1.732446 * (u - 128));
#endif
  // Even with proper conversion, some values still need clipping.
  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  // Values only go from 0-220..  Why?
  pixel[0] = r * 220 / 256;
  pixel[1] = g * 220 / 256;
  pixel[2] = b * 220 / 256;
  pixel[3] = 0;

  /* Debug
     printf("yuv2rgb(%i, %i, %i) -> %i, %i, %i  (0x%x)\n",
     y, u, v,
     pixel[0], pixel[1], pixel[2],
     pixel32);
   */

  return pixel32;
}
