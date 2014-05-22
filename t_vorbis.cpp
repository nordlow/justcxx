#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

/*!
 * Bottom (Transport) Layer.
 *
 * Defined in <ogg/ogg.h>
 */
void
test_ogg(void)
{
  ogg_stream_state os;
  int serialno = 0;
  int ret;

  /* init */
  ret = ogg_stream_init(&os, serialno);
  printf("init() returned %d\n", ret);

  /* clear */
  ret = ogg_stream_clear(&os);
  printf("clear() returned %d\n", ret);
}

/*!
 * Middle (Stream) Layer
 *
 * Used for streaming audio for example over networks.
 *
 * Defined in <vorbis/codec.h> <vorbis/vorbisenc.h>
 */
void
test_vorbis(void)
{
  vorbis_info vi;
  int ret = 0;

  /* init */
  vorbis_info_init(&vi);
  long channels = 2;		/* number of channels */
  long rate = 0;	       /* sampling rate of the source audio */
  long max_bitrate = 128;
  long nominal_bitrate = 128;
  long min_bitrate = 128;

  ret = vorbis_encode_init(&vi,
			   channels,
			   rate,
			   max_bitrate,
			   nominal_bitrate,
			   min_bitrate);
  printf("encode_init() returned %d\n", ret);

/*   vorbis_dsp_state v; */
/*   vorbis_block vb; */
/*   vorbis_block_init(&v, &vb); */

  /* clear */
  vorbis_info_clear(&vi);
}

/*!
 * Top (FILE) Layer
 *
 * Defined in <vorbis/vorbisenc.h>
 */
void
test_vorbisfile(void)
{
  FILE * f = NULL;
  OggVorbis_File vf;
  size_t nmemb = 1024;
  char * ptr = NULL;

  f = fopen("t_vorbis.vorbis", "wb");

  ptr = calloc(nmemb, 1);
  fwrite(ptr, 1, nmemb, f);
  free(ptr);

  /* takes complete possession of the FILE-resource and is closed in
   * ov_clear() */
  char * initial = NULL;
  long ibytes = 0;
  ov_open(f, &vf, initial, ibytes);

  ov_clear(&vf);
}

int
main(int argc, char *argv[])
{
  test_vorbis();
  test_vorbisfile();
  return 0;
}
