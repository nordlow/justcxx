#include "pixels.h"

pure
const char *
PXF_getName(PXF_t a)
{
  static const char *const names[PXF_UNKNOWN + 1] = {
    "PXF_RGB555",
    "PXF_RGB565",
    "PXF_RGB24",
    "PXF_RGB32",
    "PXF_BGR32",
    "PXF_RGBA32",
    "PXF_BGRA32",
    "PXF_G8",
    "PXF_UNKNOWN"
  };
  if (a >= PXF_UNKNOWN) {
    a = PXF_UNKNOWN;
  }
  return names[a];
}

void
color_pixel_test(void)
{
  color_t c = COLOR_RGB(64, 128, 192);
  printf("color:%08x to urgb32:%08x\n", c, color_to_URGB32(c));
  printf("color:%08x to rgb555:%08x\n", c, color_to_RGB555(c));
  printf("color:%08x to rgb565:%08x\n", c, color_truncate_to_RGB565(c));
}
