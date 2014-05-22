#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

void
test_jpeg(const char *filename)
{
  FILE *infile;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    exit(1);
  }

  jpeg_stdio_src(&cinfo, infile);

  jpeg_read_header(&cinfo, TRUE);

  int w = cinfo.image_width;
  int h = cinfo.image_height;

  printf("dims: %d x %d  color_space: %d  num_components: %d\n",
	 w, h, cinfo.jpeg_color_space, cinfo.num_components);
/*      JCS_RGB; */

  jpeg_start_decompress(&cinfo);

  int y;
  for (y = 0; y < h; y++) {
/*         jpeg_read_scanlines(&cinfo); */
  }

  jpeg_finish_decompress(&cinfo);
}

int
main(void)
{
  test_jpeg("");
  return 0;
}
