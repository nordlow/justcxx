#include "jpgio.h"

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jpeglib.h>

int
freadJPG_RGB(uint *width, uint *height, uchar **rgb,
	     FILE * stream)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  uchar *ptr = NULL;
  int ipos;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, stream);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  *width = cinfo.output_width;
  *height = cinfo.output_height;

  *rgb = (uchar*)malloc(3 * cinfo.output_width * cinfo.output_height);
  if (*rgb == NULL) {
    leprintf("can't allocate memory for rgb-data.\n");
    return -1;
  }

  if (cinfo.output_components == 3) {
    ptr = *rgb;
    while (cinfo.output_scanline < cinfo.output_height) {
      jpeg_read_scanlines(&cinfo, &ptr, 1);
      ptr += 3 * cinfo.output_width;
    }
  } else if (cinfo.output_components == 1) {
    ptr = (uchar*)malloc(cinfo.output_width);
    if (ptr == NULL) {
      leprintf("can't allocate memory for JPEG file.\n");
      return -1;
    }

    ipos = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
      jpeg_read_scanlines(&cinfo, &ptr, 1);
      for (uint i = 0; i < cinfo.output_width; i++) {
	memset(*rgb + ipos, ptr[i], 3);
	ipos += 3;
      }
    }

    free(ptr);
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  return 0;
}

int
fwriteJPG_RGB(uint width, uint height,
	      uchar *rgb, int quality,
	      FILE * stream)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  JSAMPROW scanline[1];

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, stream);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;

  jpeg_set_defaults(&cinfo);

  jpeg_set_quality(&cinfo, quality, TRUE);

  jpeg_start_compress(&cinfo, TRUE);

  while (cinfo.next_scanline < height) {
    scanline[0] = rgb + 3 * width * cinfo.next_scanline;
    jpeg_write_scanlines(&cinfo, scanline, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  return 0;
}
