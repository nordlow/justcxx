#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "bz2_utils.h"
#include "utils.h"
#include "clamp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_BZLIB_H
#include <bzlib.h>
#endif

/* ========================================================================= */

#ifdef HAVE_BZLIB_H
static void
bzip2_print(const bz_stream * bzs)
{
  printf("IN: next:%p avail:%d tot:%d,%d",
	 bzs->next_in, bzs->avail_in, bzs->total_in_lo32, bzs->total_in_hi32);
  printf("  OUT: next:%p avail:%d tot:%d,%d\n",
	 bzs->next_out,
	 bzs->avail_out, bzs->total_out_lo32, bzs->total_out_hi32);
}
#endif

static int g_show = 0;

int
bzip2_encode(const char *bufI, size_t lenI, char **bufC, size_t *lenC,
	     int blockSize100k)
{
#ifdef HAVE_BZLIB_H
  blockSize100k = CLAMP(1, blockSize100k, 9);

  /* 0 is silent, and greater numbers give increasingly verbose
   * monitoring/debugging output */
  int verbosity = 0;

  /* 0 is a special case, equivalent to using the default value of 30
   * which gives reasonable behaviour over a wide range of circumstances */
  int workFactor = 0;

  bz_stream bzs;		/* Bzip2 Stream State */

  /* If you don't want to use a custom memory allocator, set
   * bzalloc, bzfree and opaque to NULL, and the library will then use the
   * standard malloc/free routines. */
  bzs.bzalloc = NULL;
  bzs.bzfree = NULL;
  bzs.opaque = NULL;

  /* initialize */

  if (BZ2_bzCompressInit(&bzs, blockSize100k, verbosity, workFactor) != BZ_OK) {
    fprintf(stderr, "%s: could not initalize bzip2!\n", __FUNCTION__);
    return -1;
  }

  /* compress/encode */
  {
    const size_t lenT = 256 * 1024;	/* \TODO: Is this size alright? */
    char *bufT = (char*)malloc(lenT);		/* temporary buffer */
    int ret;

    /* all indata lies in bufI */
    bzs.next_in = (char *) bufI;
    bzs.avail_in = lenI;

    if (g_show) { bzip2_print(&bzs); }

    /* finish up */
    do {
      size_t tmplen_out;

      bzs.next_out = (char *) bufT;
      bzs.avail_out = lenT;

      ret = BZ2_bzCompress(&bzs, BZ_FINISH);

      if (g_show) { bzip2_print(&bzs); }

      tmplen_out = bzs.next_out - bufT;

      *bufC = realloc(*bufC, *lenC + tmplen_out);
      memcpy(*bufC + *lenC, bufT, tmplen_out);
      *lenC += tmplen_out;
    } while (ret != BZ_STREAM_END);

    free(bufT);
  }

  /* clean up */

  BZ2_bzCompressEnd(&bzs);

#endif
  return 0;
}

int
bzip2_decode(const char *bufC, size_t lenC, char **bufD, size_t *lenD)
{
#ifdef HAVE_BZLIB_H
  /* 0 is silent, and greater numbers give increasingly verbose
   * monitoring/debugging output */
  int verbosity = 0;

  int small = 0;

  bz_stream bzs;

  /* If you don't want to use a custom memory allocator, set
   * bzalloc, bzfree and opaque to NULL, and the library will then use the
   * standard malloc/free routines. */
  bzs.bzalloc = NULL;
  bzs.bzfree = NULL;
  bzs.opaque = NULL;

  if (BZ2_bzDecompressInit(&bzs, verbosity, small) != BZ_OK) {
    fprintf(stderr, "%s: could not initalize bzip2!\n", __FUNCTION__);
    return -1;
  }

  /* decompress/decode */
  {
    const size_t lenT = 256 * 1024;	/* \TODO: Is this size alright? */
    char *bufT = (char*)malloc(lenT);		/* temporary buffer */
    int ret;

    /* all indata lies in bufC */
    bzs.next_in = (char *) bufC;
    bzs.avail_in = lenC;

    if (g_show) { bzip2_print(&bzs); }

    /* finish up */
    do {
      size_t tmplen_out;

      bzs.next_out = (char *) bufT;
      bzs.avail_out = lenT;

      ret = BZ2_bzDecompress(&bzs);

      if (g_show) { bzip2_print(&bzs); }

      tmplen_out = bzs.next_out - bufT;

      *bufD = realloc(*bufD, *lenD + tmplen_out);
      memcpy(*bufD + *lenD, bufT, tmplen_out);
      *lenD += tmplen_out;
    } while (ret != BZ_STREAM_END);

    free(bufT);
  }

  /* clean up */

  BZ2_bzDecompressEnd(&bzs);

#endif
  return 0;
}

/*!
 * Tests BZip Compression of a Random Char Array of length \p len
 * randomized using a specific segment extracted from the rand() function.
 *
 * \param[in] bitidx gives the start of the bit segment
 * \param[in] bitlen gives the length of the bit segment
 */
int
test_bzip2_helper(size_t len, int bitidx, int bitlen)
{
  int ret = 0;

  size_t lenI = len;
  char *bufI = (char*)malloc(lenI);	/* original In buffer */

  /* randomize contents of original buffer */
  for (size_t i = 0; i < lenI; i++) {
    bufI[i] = (rand() >> bitidx) % (1 << bitlen);
  }

  for (size_t bsz = 1; bsz <= 9; bsz++) {
    char *bufC = NULL; size_t lenC = 0; /* Compressed buffer and its length */
    char *bufD = NULL; size_t lenD = 0; /* Decompressed buffer and its length */

    printf("lenI: %zd  ", lenI);
    printf("Blocksize in 100 kB: %zd  ", bsz);

    bzip2_encode(bufI, lenI, &bufC, &lenC, bsz);

    printf("compression ratio: %.1f %%  ", (double) 100 * lenC / lenI);

    bzip2_decode(bufC, lenC, &bufD, &lenD);

    if (lenI == lenD && memcmp(bufI, bufD, lenI) == 0) {
      printf("OK  "); ret = +1;
    } else {
      printf("NOT OK  "); ret = +0;
    }

    printf("\n");

    free(bufC);
    free(bufD);
  }

  free(bufI);
  return ret;
}

int
test_bzip2(void)
{
  int ret = 0;
  size_t maxlen = 1024;
  int bitidx = 8;
  int bitlen = 8;

  for (size_t i = 1; i <= maxlen; i *= 2) {
    ret += test_bzip2_helper(i, bitidx, bitlen);
  }

  return ret;
}
