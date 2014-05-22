#include "audioutils.h"

void
fix_byteorder(char * buf, size_t size)
{
#ifdef __linux__
  int afmt = AFMT_S16_LE;
  switch (afmt) {
  case AFMT_S16_LE:
  case AFMT_S16_BE:
    break;
  }
#endif
}

#ifdef __linux__

/*!
 * Using (Linux) OSS API.
 *
 * \param[in,out] frag is a buffering parameter setting both fragment
 * count and size
 *
 * Format is in the form 0xMMMMSSSS, where
 * - 2^MMMM gives fragment count.
 * - 2^SSSS gives fragment size.
 */
int
audioctx_open_OSS(AudioCtx * audio_ctx,
                  const char * devname,
                  int * chan, int * srate, int * frag, int * precision,
                  int sign_flag, int little_endian_flag)
{
  int fd;
  int fmt = AFMT_S16_LE;
  int mask, chan_tmp, rate_tmp, fmt_tmp, frag_tmp;

  if ((fd = open((char*)devname, O_RDWR, 0)) == -1) {
    lperror("could not open audio device");
    return -1;
  }

  /* get available formats */
  if (ioctl(fd, SNDCTL_DSP_GETFMTS, &mask) == -1) {
    lperror("ioctl(SNDCTL_DSP_GETFMTS)");
    close(fd);
    return -1;
  }
  /* and check if our wanted format is listed */
  if (!(mask & fmt)) {
    fprintf(stderr, "%s: format is not supported\n", __FUNCTION__);
    close(fd);
    return -1;
  }

  switch (*precision) {
  case 8:
    if (sign_flag) {
      fmt = AFMT_S8;
    } else {
      fmt = AFMT_U8;
    }
    break;
  case 16:
    if (sign_flag) {
      fmt = little_endian_flag ? AFMT_S16_LE  : AFMT_S16_BE;
    } else {
      fmt = little_endian_flag ? AFMT_U16_LE  : AFMT_U16_BE;
    }
    break;
  default:
    leprintf("cannot set precision %d. defaulting to 16.\n", *precision);
    if (sign_flag) {
      fmt = little_endian_flag ? AFMT_S16_LE  : AFMT_S16_BE;
    } else {
      fmt = little_endian_flag ? AFMT_U16_LE  : AFMT_U16_BE;
    }
    *precision = 16;
  }

  chan_tmp = *chan;
  rate_tmp = *srate;
  fmt_tmp = fmt;
  frag_tmp = *frag;

  /* set format */
  if (ioctl(fd, SNDCTL_DSP_SETFMT, &fmt_tmp) == -1) {
    lperror("ioctl(SNDCTL_DSP_SETFMT)");
    close(fd);
    return -1;
  }
  if (fmt_tmp != fmt) {
    lprintf("audio format not supported by audio-device %s. changing format from %d to %d\n",
            devname, fmt, fmt_tmp);
  }

  if (ioctl(fd, SNDCTL_DSP_CHANNELS, &chan_tmp) == -1) {
    lperror("ioctl(SNDCTL_DSP_CHANNELS)");
    close(fd);
    return -1;
  }
  if (chan_tmp != *chan) {
    lprintf("changed chan: from %d => %d\n", *chan, chan_tmp);
    *chan = chan_tmp;
  }

  if (ioctl(fd, SNDCTL_DSP_SPEED, &rate_tmp) == -1) {
    lperror("ioctl(SNDCTL_DSP_SPEED)");
    close(fd);
    return -1;
  }
  if (rate_tmp != *srate) {
    lprintf("changed srate: from %d => %d\n", *srate, rate_tmp);
    *srate = rate_tmp;
  }

  if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &frag_tmp) == -1) {
    lperror("ioctl(SNDCTL_DSP_SETFRAGMENT)");
    close(fd);
    return -1;
  }
  if (frag_tmp != *frag) {
    lprintf("changed frag: from 0x%08x => 0x%08x\n", *frag, frag_tmp);
    *frag = frag_tmp;
  }

  /* print fragment statistics */
  lprintf("using fragment 0x%08x => count:%f and size:%f\n",
          frag_tmp,
          pow(2, frag_tmp >> 16),
          pow(2, frag_tmp & 0xffff));

  return fd;
}

int
audioctx_open(AudioCtx * audio_ctx,
              const char * devname,
              int * chan, int * srate, int * frag, int * precision,
              bool sign_flag, bool little_endian_flag)
{
  if (1) {
    return audioctx_open_OSS(audio_ctx,
                             devname,
                             chan, srate, frag, precision,
                             sign_flag, little_endian_flag);
  } else {
  }
}

#elif defined(__solaris__)

/*!
 * Using Solaris Audio API.
 */
int
audioctx_open(AudioCtx * audio_ctx,
              const char * devname,
              int * chan, int * srate, int * frag, int * precision,
              bool sign_flag, bool little_endian_flag)
{
  int fd, fdc;
  struct audio_info ai;

  if ((fd = open(devname, O_RDWR, 0)) == -1) {
    lperror("could not open audio device");
    return -1;
  }

  if ((fdc = open("/dev/audioctl", O_RDWR, 0)) == -1) {
    lperror("could not open audio control device");
    leprintf("skipping configuration of audio control device\n");
  } else {
    long rval;			/* return argument */
    int initflag = TRUE;

    if (initflag) {
      AUDIO_INITINFO(&ai);		/* read current audio data format */
      rval = 0;
    } else {
      if ((rval = ioctl(fdc, AUDIO_GETINFO, &ai)) < 0) {
        lperror("ioctl AUDIO_GETINFO");
      }
    }

    if (rval >= 0) {
      ai.record.port = AUDIO_MICROPHONE;

      ai.play.sample_rate = *srate;

      switch (*chan) {
      case AUDIO_CHANNELS_MONO:
      case AUDIO_CHANNELS_STEREO:
        ai.play.channels = *chan;
        break;
      default:
        leprintf("cannot set number of channels %d\n", *chan);
        break;
      }

      switch (*precision) {
      case AUDIO_PRECISION_8:
      case AUDIO_PRECISION_16:
        ai.play.precision = *precision;
        break;
      default:
        leprintf("cannot set precision %d\n", *precision);
        break;
      }

      if (sign_flag) {
        ai.play.encoding = AUDIO_ENCODING_LINEAR;
      } else {
        if (*precision != 8) {
          leprintf("cannot set unsigned encoding for precision %d. defaulting to LINEAR (signed)\n",
                   *precision);
          ai.play.encoding = AUDIO_ENCODING_LINEAR;
        } else {
          ai.play.encoding = AUDIO_ENCODING_LINEAR8;
        }
      }

      ai.play.port = AUDIO_SPEAKER | AUDIO_LINE_OUT | AUDIO_HEADPHONE;

      if ((rval = ioctl(fdc, AUDIO_SETINFO, &ai)) < 0) {
        lperror("ioctl AUDIO_SETINFO");
      }
    }

    close(fdc);
  }

  return fd;
}

#else

/*!
 * Dummy Audio API.
 */
int
audioctx_open(AudioCtx * audio_ctx,
              const char * devname,
              int * chan, int * srate, int * frag, int * precision,
              int sign_flag, int little_endian_flag)
{
  int fd = -1;
  return fd;
}

#endif

/*!
 * Close Audio Context.
 */
int
audioctx_close(AudioCtx * audio_ctx)
{
  return 0;
}
