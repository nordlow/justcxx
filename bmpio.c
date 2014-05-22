/*!
 * \file IBMP.c
 *
 * Read BMP-files.
 */

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>

/*!
 * Errors
 */
typedef enum {
  INoError = 0,
  IInvalidImage,
  IInvalidGC,
  IInvalidColor,
  INoTransparentColor,
  IInvalidFont,
  IFunctionNotImplemented,
  IInvalidFormat,
  IFileInvalid,
  IErrorWriting,
  INoFontSet,
  INoSuchFont,
  INoSuchFile,
  IFontError,
  IInvalidEscapeSequence,
  IInvalidArgument,
  IInvalidColorName,
  IGIFError,
  INoGIFSupport,
  IPNGError,
  INoPNGSupport,
  IInvalidPolygon
} IError;

/*!
 * Options
 */
#define IOPTION_NONE		0x0000

/* use the following with ICreateImage() */
#define IOPTION_GREYSCALE	0x0001	/* greyscale image */
#define IOPTION_GRAYSCALE	IOPTION_GREYSCALE

/* use the following with IWriteImageFile() */
#define IOPTION_ASCII		0x0001	/* ascii output for pbm/pgm/ppm */
#define IOPTION_INTERLACED	0x0002	/* interlaced output (GIF) */

/*!
 * Define a magic value to place at the front of each structure.
 * If this value is incorrect, then an invalid structure was
 * passed in.
 */
#define IMAGIC_IMAGE		467
#define IMAGIC_GC		333
#define IMAGIC_COLOR		847
#define IMAGIC_FONT		104

/*!
 * Default color values for black and white
 */
#define IBLACK_PIXEL		0
#define IWHITE_PIXEL		1

typedef void * IImage;		/* image */
typedef void * IFont;		/* font */
typedef void * IGC;		/* graphics context */
typedef uint IColor;	/* color */
typedef uint IOptions;	/* options */

typedef struct {
  uint magic;		/* memory verification */
  uchar red;		/* red value (0-255) */
  uchar green;		/* green value (0-255) */
  uchar blue;		/* blue value (0-255) */
  ulong value;		/* pixel value */
} IColorP;

typedef struct {
  uint magic;		/* memory verification */
  int width;			/* width */
  int height;			/* height */
  char *comments;		/* comments */
  uchar *data;		/* image data */
  ushort interlaced;	/* interlaced output? */
  ushort greyscale;	/* greyscale? */
  IColorP *transparent;		/* transparent color */
} IImageP;

/*!
 * Create a blank (white) image of a specified width and height.
 */
IImage ICreateImage (uint width, /* image width */
		     uint height, /* image height */
		     uint options /* options flags (IOPTION_GREYSCALE, etc.) */
		     );

/*!
 * Frees the memory associated with an image that will not be used again.
 */
IError IFreeImage(IImage image /* pointer to image */
		    );

IImage ICreateImage (unsigned width, unsigned height, unsigned options)
{
  IImageP *image;

  image = (IImageP *) malloc ( sizeof ( IImageP ) );
  memset ( image, '\0', sizeof ( IImageP ) );
  image->width = width;
  image->height = height;
  if ( options & IOPTION_GREYSCALE ) {
    image->data = (uchar *) malloc ( width * height );
    memset ( image->data, 255, width * height );
    image->greyscale = 1;
  }
  else {
    image->data = (uchar *) malloc ( width * height * 3 );
    memset ( image->data, 255, width * height * 3 );
  }

  image->magic = IMAGIC_IMAGE;

  return ( (IImage)image );
}

uint IImageHeight (IImage image)
{
  IImageP *i = (IImageP *) image;

  if ( i ) {
    if ( i->magic != IMAGIC_IMAGE )
      return ( 0 );
  }
  else
    return ( 0 );

  return ( i->height );
}

uint IImageWidth (IImage image)
{
  IImageP *i = (IImageP *) image;

  if ( i ) {
    if ( i->magic != IMAGIC_IMAGE )
      return ( 0 );
  }
  else
    return ( 0 );

  return ( i->width );
}

IError IFreeImage(IImage image)
{
  IImageP *i = (IImageP *) image;

  if ( i ) {
    if ( i->magic != IMAGIC_IMAGE )
      return ( IInvalidImage );
    i->magic = 0;
    free ( i->data );
    if ( i->comments )
      free ( i->comments );
    i->comments = NULL;
    free ( i );
  }

  return INoError;
}

IError ISetComment (IImage image, char *comments)
{
  IImageP *imagep = (IImageP *)image;

  if ( imagep ) {
    if ( imagep->magic != IMAGIC_IMAGE )
      return ( IInvalidImage );
  }
  else
    return ( IInvalidImage );

  if ( imagep->comments != NULL )
    free ( imagep->comments );
  if ( comments == NULL )
    imagep->comments = NULL;
  else {
    imagep->comments = (char *) malloc ( strlen ( comments ) + 1 );
    strcpy ( imagep->comments, comments );
  }
  return INoError;
}

IError IGetComment (IImage image, char **comments)
{
  IImageP *imagep = (IImageP *)image;

  if ( imagep ) {
    if ( imagep->magic != IMAGIC_IMAGE )
      return ( IInvalidImage );
  }
  else
    return ( IInvalidImage );

  *comments = imagep->comments;
  return INoError;
}

static int num_colors = 0;
static IColorP **colors = NULL;

static void init_colors(void)
{
  IColorP *color;

  colors = (IColorP **) malloc ( 2 * sizeof ( IColorP * ) );

  /* add black as color 0 */
  color = (IColorP *) malloc ( sizeof ( IColorP ) );
  color->red = color->green = color->blue = 0;
  color->value = num_colors++;
  colors[0] = color;

  /* add white as color 1 */
  color = (IColorP *) malloc ( sizeof ( IColorP ) );
  color->red = color->green = color->blue = 255;
  color->value = num_colors++;
  colors[1] = color;
}

IColorP *
_IGetColor(int color)
{
  if ( colors == NULL )
    init_colors ();

  if ( color < 0 || color >= num_colors )
    return ( NULL );

  return ( colors[color] );
}

IError ISetTransparent (IImage image, IColor color)
{
  IImageP *imagep = (IImageP *)image;
  IColorP *colorp;

  if ( imagep ) {
    if ( imagep->magic != IMAGIC_IMAGE )
      return ( IInvalidImage );
  }
  else
    return ( IInvalidImage );

  colorp = _IGetColor ( color );
  if ( colorp ) {
    if ( colorp->magic != IMAGIC_COLOR )
      return ( IInvalidColor );
  }
  else
    return ( IInvalidColor );

  imagep->transparent = colorp;

  return INoError;
}



IError IGetTransparent (IImage image, IColor *color)
{
  IImageP *imagep = (IImageP *)image;

  if ( imagep ) {
    if ( imagep->magic != IMAGIC_IMAGE )
      return ( IInvalidImage );
  }
  else
    return ( IInvalidImage );

  if ( imagep->transparent && imagep->transparent->magic == IMAGIC_COLOR ) {
    *color = imagep->transparent->value;
    return INoError;
  } else {
    return ( INoTransparentColor );
  }
}

#define BMP_HEADER ('B' + ('M' << 8))

/* Compression formats */
#define BI_RGB                0
#define BI_RLE8                1
#define BI_RLE4                2
#define BI_BITFIELDS        3

#define SET_PIXEL(buffer, byte) \
	do { \
		*(buffer+0) = (colortable[byte&0xff]&(0xff<<16))>>16; \
		*(buffer+1) = (colortable[byte&0xff]&(0xff<<8))>>8; \
		*(buffer+2) = (colortable[byte&0xff]&(0xff<<0))>>0; \
	} while (0);

static int ReadShort(FILE *fp, int *ret)
{
  int c = fgetc(fp);
  if (c == EOF) return 0;
  *ret = (c&0xff);
  c = fgetc(fp);
  if (c == EOF) return 0;
  *ret |= (c&0xff) << 8;
  return 1;
}

static int
ReadLong(FILE *fp, int *ret)
{
  int c = fgetc(fp);
  if (c == EOF) return 0;
  *ret = (c&0xff);
  c = fgetc(fp);
  if (c == EOF) return 0;
  *ret |= (c&0xff) << 8;
  c = fgetc(fp);
  if (c == EOF) return 0;
  *ret |= (c&0xff) << 16;
  c = fgetc(fp);
  if (c == EOF) return 0;
  *ret |= (c&0xff) << 24;
  return 1;
}

IError
IReadBMP(FILE *fp, IOptions options, IImageP **image_return)
{
  IImageP *image = NULL;
  int scratch, fileSize, offset, w, h, depth, compression,
      imagesize, xpelspermeter, ypelspermeter, colorsused, colorsimportant;
  int redmask = 0, greenmask = 0, bluemask = 0,
      redshift = 0, greenshift = 0, blueshift = 0,
      redbits = 0, greenbits = 0, bluebits = 0;
  int *colortable = NULL;

  /* read the header and make sure this is a BMP file */
  if (!ReadShort(fp, &scratch) || scratch != BMP_HEADER) return IInvalidFormat;

  /* read the file size (not sure this is useful) */
  if (!ReadLong(fp, &fileSize)) return (IInvalidFormat);

  /* read the two reserved fields (must be zero, according to spec) */
  if (!ReadShort(fp, &scratch) || scratch != 0) return (IInvalidFormat);
  if (!ReadShort(fp, &scratch) || scratch != 0) return (IInvalidFormat);

  /* read the offset of the actual graphic bits */
  if (!ReadLong(fp, &offset)) return (IInvalidFormat);

  /*fprintf(stderr, "offset = %d\n", offset);*/

  /* verify that the header is 40 bytes */
  /* XXX: really should save this and just skip bytes we don't understand. */
  /* and we could be using this to detect things in OS/2 format */
  if (!ReadLong(fp, &scratch) || scratch != 40) return (IInvalidFormat);

  /* get image size and depth */
  if (!ReadLong(fp, &w)) return (IInvalidFormat);
  if (!ReadLong(fp, &h)) return (IInvalidFormat);
  if (!ReadShort(fp, &scratch) || scratch != 1) return (IInvalidFormat);
  if (!ReadShort(fp, &depth)) return (IInvalidFormat);

  /*fprintf(stderr, "image size: %dx%dx%d\n", w, h, depth);*/

  if (!ReadLong(fp, &compression)) return (IInvalidFormat);
  if (!ReadLong(fp, &imagesize)) return (IInvalidFormat);
  if (!ReadLong(fp, &xpelspermeter)) return (IInvalidFormat);
  if (!ReadLong(fp, &ypelspermeter)) return (IInvalidFormat);
  if (!ReadLong(fp, &colorsused)) return (IInvalidFormat);
  if (!ReadLong(fp, &colorsimportant)) return (IInvalidFormat);

  /* we have read 40 bytes of header */
  /* we have read 54 bytes of data from the file */

/*
  fprintf(stderr, "compression=%d, imagesize=%d, colorsused=%d\n",
      compression,
      imagesize,
      colorsused);
*/

  /* get the masks, shifts, and bits for bitfields (16 and 32 bits) */
  if (compression == BI_BITFIELDS) {
    if (!ReadLong(fp,&redmask)) return IInvalidImage;
    for (redshift = 0; !(redmask&(1<<redshift)) && redshift<32; redshift++);
    for (redbits = 0; (redmask&(1<<(redshift+redbits))) && redshift<32; redbits++);
    if (!ReadLong(fp,&greenmask)) return IInvalidImage;
    for (greenshift = 0; !(greenmask&(1<<greenshift)) && greenshift<32; greenshift++);
    for (greenbits = 0; (greenmask&(1<<(greenshift+greenbits))) && greenshift<32; greenbits++);
    if (!ReadLong(fp,&bluemask)) return IInvalidImage;
    for (blueshift = 0; !(bluemask&(1<<blueshift)) && blueshift<32; blueshift++);
    for (bluebits = 0; (bluemask&(1<<(blueshift+bluebits))) && blueshift<32; bluebits++);
    /*fprintf(stderr, "masks: %x %x %x\n", redmask, greenmask, bluemask);*/
    /*fprintf(stderr, "shifts: %d %d %d\n", redshift, greenshift, blueshift);*/
    /*fprintf(stderr, "bits: %d %d %d\n", redbits, greenbits, bluebits);*/
  }

  if (depth <= 8) {
    if (!colorsused) colorsused = 2<<depth;
    colortable = malloc(colorsused*sizeof(int));
    for (scratch = 0; scratch < colorsused; scratch++) {
      if (!ReadLong(fp,(colortable+scratch))) return (IInvalidFormat);
    }
  }

  /* seek to the beginning of the image data */
  if (fseek(fp, offset, SEEK_SET) < 0) return (IInvalidFormat);

  /* create a new image object */
  image = (IImageP *) ICreateImage ( w, h, options );

  if (depth == 24 && compression == BI_RGB) {
    int x, y;
    /* slurp in the data */
    for (y = h - 1; y >= 0; y--) {
      for (x = 0; x < w; x++) {
        int r,g,b;
        r = fgetc(fp); g = fgetc(fp); b = fgetc(fp);
        /* this leaks the image object if we hit a premature EOF */
        if (r == EOF || g == EOF || b == EOF) return (IInvalidFormat);
        *(image->data + (y*3*w) + (x*3)) = b;
        *(image->data + (y*3*w) + (x*3) + 1) = g;
        *(image->data + (y*3*w) + (x*3) + 2) = r;
      }
    }
  } else if (depth == 16 && compression == BI_BITFIELDS) {
    int x, y;
    /* slurp in the data */
    for (y = h - 1; y >= 0; y--) {
      for (x = 0; x < w; x++) {
        uchar *pixel = image->data + (y*3*w) + (x*3);
        int color;
        /* this leaks the image object if we hit a premature EOF */
        if (!ReadShort(fp, &color)) return (IInvalidFormat);
        /* scale 5/6 bit values to 8 bits */
        *(pixel + 0) = (((color&redmask)>>redshift)<<8)>>redbits;
        *(pixel + 1) = (((color&greenmask)>>greenshift)<<8)>>greenbits;
        *(pixel + 2) = (((color&bluemask)>>blueshift)<<8)>>bluebits;
      }
    }
  }
  else if (depth == 8 && compression == BI_RGB) {
    int x, y;
    /*fprintf(stderr, "8 bit, no compression\n");*/
    for (y = h - 1; y >= 0; y--) {
      for (x = 0; x < w; x++) {
        uchar *pixel = image->data+(y*w*3)+(x*3);
        int byte = fgetc(fp); if (byte == EOF) return IInvalidFormat;
        SET_PIXEL(pixel, byte);
      }
    }

  } else if (depth <= 8 && compression) {
    int byte, count, x, y;
    uchar *buffer = image->data+w*(h-1)*3;
    for (y = 0; y < h; ) {
      count = fgetc(fp);
      if (count == EOF) return (IInvalidFormat);
      if (count != 0) {
        /*fprintf(stderr, "stretch of %d bytes\n", count);*/
        byte = fgetc(fp); if (byte == EOF) return (IInvalidFormat);
        for (scratch = 0; scratch < count; scratch++) {
          if (compression == 1) {
            SET_PIXEL(buffer, byte);
          }
          else {
            int thisbyte = (scratch & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f);
            SET_PIXEL(buffer, thisbyte);
          }
          buffer += 3;
        }
      }
      else {
        count = fgetc(fp);
        if (count == EOF) return (IInvalidFormat);
        if (count == 0x01) break; /* end of bitmap */
        switch (count) {
        case 0x00: /* end of the line */
          /*fprintf(stderr, "hit end of line %d\n", y);*/
          y++;
          buffer = image->data+w*(h-y-1)*3;
          break;
        case 0x02: /* goto specific position */
          x = fgetc(fp);
          y = fgetc(fp);
          /*fprintf(stderr, "going to %d,%d\n", x, y);*/
          if (x == EOF || y == EOF) return (IInvalidFormat);
          buffer = image->data+w*(h-y-1)*3+(x*3);
          break;
        default: /* a bunch of literal bytes */
          /*fprintf(stderr, "handling %d literal bytes\n", count);*/
          for (scratch = 0; scratch < count; scratch++) {
            byte = fgetc(fp); if (byte == EOF) return (IInvalidFormat);
            if (compression == 1) {
              SET_PIXEL(buffer, byte);
            }
            else {
              int thisbyte = (scratch & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f);
              SET_PIXEL(buffer, thisbyte);
            }
            buffer += 3;
          }
          /* handle padding */
          if (compression == 1) {
            if (count & 0x01) {
              /*fprintf(stderr, "eating padding\n");*/
              (void)fgetc(fp);
            }
          }
          else if ((count & 0x03) == 1 || ((count & 0x03) == 2)) {
            (void)fgetc(fp);
          }
          break;
        }
      }
    }
  }
  else {
    IFreeImage(image);
    return (IInvalidFormat);
  }

  if (colortable) free(colortable);
  *image_return = image;
  return INoError;
}
