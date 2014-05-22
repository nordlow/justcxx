#include "pnmio.h"

#include <stdlib.h>
#include <string.h>

/* Write */

#if 0
int
fwritePBM(const Bitmtx * data, uint w, uint h, FILE * stream)
{
  fprintf(stream, "P4\n%u %u 255\n", w, h);
  /* \TODO: How should we store binary images in memory? */
  return -1;
}
#endif

int
fwritePGM(const uchar *data,
	  uint w, uint h, FILE * stream)
{
  uint pixmax = 255;
  fprintf(stream, "P5\n%u %u %u\n", w, h, pixmax);
  if (fwrite(data, sizeof(uchar), w * h, stream) != w * h) {
    perror("fwrite");
    return -1;
  } else
    return 0;
}

int
fwritePPM(const uchar *data,
	  uint w, uint h, FILE * stream)
{
  uint pixmax = 255;
  fprintf(stream, "P6\n%u %u %u\n", w, h, pixmax);
  if (fwrite(data, sizeof(uchar), 3 * w * h, stream) != 3 * w * h) {
    perror("fwrite");
    return -1;
  } else
    return 0;
}

/* Read */

int
freadPGM(uint *w, uint *h, uchar **rgb,
	 FILE * stream)
{
  uchar *data;
  char line[80];
  size_t size;
  uint max_pixel;
  size_t i;

  /* Read the first line (until '\n' or '\0') from the stream. */
  fgets(line, 80, stream);

  if (strncmp(line, "P5", 2) != 0) {
    leprintf("wrong format.\n");
    return -1;
  }

  /* Skip comments and then read metadata. */
  while (fgets(line, 80, stream) && (line[0] == '#'));
  sscanf(line, "%d %d", w, h);	/* Dimensions. */
  size = 1 * (*w) * (*h);
  data = malloc(size);

  /* Scan maximum pixel value. */
  fscanf(stream, "%d", &max_pixel);
  fgetc(stream);
  if (max_pixel >= 256) {
    leprintf("can only handle max_pixel < 256 \n.");
    free(data);
    return -1;
  }

  /* Read data. */
  if (fread(data, sizeof(uchar), size, stream) != size) {
    if (feof(stream)) {
      leprintf("end of file\n");
      free(data);
      return -1;
    } else if (ferror(stream)) {
      lperror("fread");
      free(data);
      return -1;
    }
  }

  *rgb = malloc(3 * size);
  for (i = 0; i < size; i++) {
    (*rgb)[i * 3 + 0] = data[i];
    (*rgb)[i * 3 + 1] = data[i];
    (*rgb)[i * 3 + 2] = data[i];
  }

  free(data);

  return 0;
}

int
freadPPM(uint *w, uint *h, uchar **rgb,
	 FILE * stream)
{
  uchar *data;
  char line[80];
  size_t size;
  uint max_pixel;

  /* Read the first line (until '\n' or '\0') from the stream. */
  fgets(line, 80, stream);

  if (strncmp(line, "P6", 2) != 0) {
    leprintf("wrong format.\n");
    return -1;
  }

  /* Skip comments and then read metadata. */
  while (fgets(line, 80, stream) && (line[0] == '#'));
  sscanf(line, "%d %d", w, h);	/* Dimensions. */
  size = 3 * (*w) * (*h);
  data = malloc(size);

  /* Scan maximum pixel value. */
  fscanf(stream, "%d", &max_pixel);
  fgetc(stream);
  if (max_pixel >= 256) {
    leprintf("can only handle max_pixel < 256 \n.");
    free(data);
    return -1;
  }

  /* Read data. */
  if (fread(data, sizeof(uchar), size, stream) != size) {
    if (feof(stream)) {
      leprintf("end of file\n");
      free(data);
      return -1;
    } else if (ferror(stream)) {
      lperror("fread");
      free(data);
      return -1;
    }
  }

  *rgb = data;

  return 0;
}
