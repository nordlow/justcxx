/*!
 * \file t_alsa_pcm_min.c
 * \brief This extra small demo sends a random samples to your speakers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <getopt.h>
#include <alsa/asoundlib.h>
#include <sys/time.h>
#include <math.h>

/*!
 * Open ALSA Audio Device named device.
 */
int audio_open_ALSA(snd_pcm_t ** handle, const char * device,
		    int * chan, int * srate, int * frag, int * precision,
		    bool sign_flag, bool little_endian_flag)
{
  int err = 0;

  snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
  /* open */
  if ((err = snd_pcm_open(handle, device, stream, 0)) < 0) {
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }

  snd_pcm_format_t format = SND_PCM_FORMAT_S16; /* default */
  switch (*precision) {
  case 8:
    if (sign_flag) {
      format = SND_PCM_FORMAT_S8;
    } else {
      format = SND_PCM_FORMAT_U8;
    }
    break;
  case 16:
    if (sign_flag) {
      format = little_endian_flag ? SND_PCM_FORMAT_S16_LE  : SND_PCM_FORMAT_S16_BE;
    } else {
      format = little_endian_flag ? SND_PCM_FORMAT_U16_LE  : SND_PCM_FORMAT_U16_BE;
    }
    break;
  default:
    printf("cannot set precision %d. defaulting to 16.\n", *precision);
    *precision = 16;
    if (sign_flag) {
      format = little_endian_flag ? SND_PCM_FORMAT_S16_LE  : SND_PCM_FORMAT_S16_BE;
    } else {
      format = little_endian_flag ? SND_PCM_FORMAT_U16_LE  : SND_PCM_FORMAT_U16_BE;
    }
  }

  /* set parameters */
  snd_pcm_access_t access = SND_PCM_ACCESS_RW_INTERLEAVED;
  unsigned int channels = *chan; /* number of channels */
  unsigned int rate = *srate;	/* samples rate */
  int soft_resample = 1;
  unsigned int latency = 500000; /* latency in microseconds */
  if ((err = snd_pcm_set_params(*handle, format, access,
				channels, rate,
				soft_resample, latency)) < 0) {	/* 0.5sec */
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }

  return err;
}

void audio_close_ALSA(snd_pcm_t * handle)
{
  snd_pcm_close(handle);
}

int
main(void)
{
  int err;
  snd_pcm_t *handle = NULL;

  unsigned char buffer[16 * 1024];	/* some random data */

  /* generate sample buffer */
  for (unsigned int i = 0; i < sizeof(buffer); i++) {
    buffer[i] = random() & 0xff;
  }

  /* open */
  const char *device = "default";	/* playback device */
  int schan = 1;
  int srate = 48000;
  int sprecision = 16;
  bool sign_flag = 1;
  bool little_endian_flag = 1;
  int frag = 0x0004000b;

  audio_open_ALSA(&handle, device, &schan, &srate, &frag,
		  &sprecision, sign_flag, little_endian_flag);

  for (unsigned int i = 0; i < 16; i++) {
    snd_pcm_sframes_t frames;
    frames = snd_pcm_writei(handle, buffer, sizeof(buffer));
    if (frames < 0)
      frames = snd_pcm_recover(handle, frames, 0);
    if (frames < 0) {
      printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
      break;
    }
    if (frames > 0 && frames < (long) sizeof(buffer))
      printf("Short write (expected %li, wrote %li)\n", (long) sizeof(buffer),
	     frames);
  }

  audio_close_ALSA(handle);

  return 0;
}
