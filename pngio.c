#include "pngio.h"
#include "utils.h"
#include "bitvec.h"

#include <png.h>
#include <stdio.h>
#include <stdlib.h>

int
freadPNG_RGB(uint *w, uint *h, uchar **rgb,
	 FILE * stream)
{
  png_structp pngp;
  png_infop infop;
  png_uint_32 width, height;
  int bit_depth, color_type, interlace_type, compression_type, filter_type;
  int rowsize, channels;
  png_uint_32 x, y;
  uchar *rowdat;

/*    fread(header, 1, number, stream);
    is_png = !png_sig_cmp(header, 0, number);
    if (!is_png)
    {
        return 0;
    }*/

  pngp = png_create_read_struct
    (PNG_LIBPNG_VER_STRING, (png_voidp) NULL /*user_error_ptr */ ,
     NULL /*user_error_fn */ , NULL /*user_warning_fn */ );
  if (!pngp)
    return -1;

  infop = png_create_info_struct(pngp);
  if (!infop) {
    png_destroy_read_struct(&pngp, (png_infopp) NULL, (png_infopp) NULL);
    return -1;
  }

  /* png_set_invert_alpha(pngp); */
  png_init_io(pngp, stream);

  png_read_info(pngp, infop);

  png_get_IHDR(pngp, infop, &width, &height,
	       &bit_depth, &color_type, &interlace_type,
	       &compression_type, &filter_type);

  if (interlace_type != PNG_INTERLACE_NONE) {
    leprintf("cannot handle interlaced images\n");
    free(infop);
    free(pngp);
    return -1;
  }

  channels = png_get_channels(pngp, infop);
  rowsize = png_get_rowbytes(pngp, infop);
  rowdat = malloc(rowsize);	/* allocate temporary row */
  *rgb = malloc(3 * width * height); /* allocate rgb image */

  png_colorp palette;
  int num_palette = 0;

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_get_PLTE(pngp, infop, &palette, &num_palette);
    if (num_palette != (1 << bit_depth)) {
      leprintf("num_palette %d does not match bit_depth %d\n",
	       num_palette, bit_depth);
    }
  }

  /* convert image data to rgb */
  for (y = 0; y < height; y++) {
    Bitvec btv;			/* handles non-byte-aligned decoding */

    png_read_rows(pngp, &rowdat, NULL, 1);
    bitvec_initbytes(&btv, rowsize, (char*)rowdat); /* copy to bitvector */

    for (x = 0; x < width; x++) {
      png_uint_32 i = x + y * width;
      const uchar * rowp = rowdat;
      switch (color_type) {
      case PNG_COLOR_TYPE_GRAY:	/* 1 channel */
	switch (bit_depth) {
	case 1:
	  break;
	case 2:
	  break;
	case 4:
	  break;
	case 8:
	  (*rgb)[3 * i + 0] = rowp[x];
	  (*rgb)[3 * i + 1] = rowp[x];
	  (*rgb)[3 * i + 2] = rowp[x];
	  break;
	case 16:
	  break;
	default:
	  leprintf("cannot handle bit_depth %d!\n", bit_depth);
	  break;
	}
	break;
      case PNG_COLOR_TYPE_GRAY_ALPHA: /* 2 channels */
	switch (bit_depth) {
	case 8:
	  (*rgb)[3 * i + 0] = rowp[2 * x + 0];
	  (*rgb)[3 * i + 1] = rowp[2 * x + 0];
	  (*rgb)[3 * i + 2] = rowp[2 * x + 0];
	  break;
	case 16:
	  break;
	default:
	  leprintf("cannot handle bit_depth %d!\n", bit_depth);
	  break;
	}
	break;
      case PNG_COLOR_TYPE_PALETTE: /* 1 channel */
	switch (bit_depth) {
	case 1:
	  break;
	case 2:
	  break;
	case 4:
	  break;
	case 8:
	  (*rgb)[3 * i + 0] = palette[rowp[x]].red;
	  (*rgb)[3 * i + 1] = palette[rowp[x]].green;
	  (*rgb)[3 * i + 2] = palette[rowp[x]].blue;
	  break;
	default:
	  leprintf("cannot handle bit_depth %d!\n", bit_depth);
	  break;
	};
	break;
      case PNG_COLOR_TYPE_RGB:	/* 3 channels */
	switch (bit_depth) {
	case 8:
	  (*rgb)[3 * i + 0] = rowp[3 * x + 0]; /* read */
	  (*rgb)[3 * i + 1] = rowp[3 * x + 1]; /* green */
	  (*rgb)[3 * i + 2] = rowp[3 * x + 2]; /* blue */
	  break;
	case 16:
	  break;
	default:
	  leprintf("cannot handle bit_depth %d!\n", bit_depth);
	  break;
	}
	break;
      case PNG_COLOR_TYPE_RGB_ALPHA: /* 4 channels */
	switch (bit_depth) {
	case 8:
	  (*rgb)[3 * i + 0] = rowp[4 * x + 0]; /* read */
	  (*rgb)[3 * i + 1] = rowp[4 * x + 1]; /* green */
	  (*rgb)[3 * i + 2] = rowp[4 * x + 2]; /* blue */
	  /* skip alpha part */
	  break;
	case 16:
	  break;
	default:
	  leprintf("cannot handle bit_depth %d!\n", bit_depth);
	  break;
	}
	break;
      default:
	break;
      }
    }
    bitvec_clear(&btv);
  }

  free(rowdat);
  free(infop);
  free(pngp);

  *w = width;
  *h = height;
  /* *depth = channels * bit_depth; */

  return 0;
}
