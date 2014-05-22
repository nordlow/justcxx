/*!
 * \file audio.c
 * \brief Audio Handling
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include "OSSio.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

int
OSS_open(const char *dev_path, int chan, int rate, int fmt, int frag)
{
  int fd;

  if ((fd = open(dev_path, O_RDONLY, 0)) == -1) {
    return -1;
  }

  /* get available formats */
  int mask;
  if (ioctl(fd, SNDCTL_DSP_GETFMTS, &mask) == -1) {
    perror("OSS_open: ioctl(SNDCTL_DSP_GETFMTS)");
    close(fd);
    return -1;
  }
  /* and check if our wanted format is listed */
  if (!(mask & fmt)) {
    fprintf(stderr, "%s: format is not supported\n", __FUNCTION__);
    close(fd);
    return -1;
  }

  int chan_tmp = chan;
  int rate_tmp = rate;
  int fmt_tmp = fmt;
  int frag_tmp = frag;

  /* set format */
  if (ioctl(fd, SNDCTL_DSP_SETFMT, &fmt_tmp) == -1) {
    perror("OSS_open: ioctl(SNDCTL_DSP_SETFMT)");
    close(fd);
    return -1;
  }
  if (fmt_tmp != fmt) {
    printf("changed fmt: from %d => %d\n", fmt, fmt_tmp);
  }

  if (ioctl(fd, SNDCTL_DSP_CHANNELS, &chan_tmp) == -1) {
    perror("OSS_open: ioctl(SNDCTL_DSP_CHANNELS)");
    close(fd);
    return -1;
  }
  if (chan_tmp != chan) {
    printf("changed chan: from %d => %d\n", chan, chan_tmp);
  }

  if (ioctl(fd, SNDCTL_DSP_SPEED, &rate_tmp) == -1) {
    perror("OSS_open: ioctl(SNDCTL_DSP_SPEED)");
    close(fd);
    return -1;
  }
  if (rate_tmp != rate) {
    printf("changed rate: from %d => %d\n", rate, rate_tmp);
  }

  if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &frag_tmp) == -1) {
    perror("OSS_open: ioctl(SNDCTL_DSP_SETFRAGMENT)");
    close(fd);
    return -1;
  }
  if (frag_tmp != frag) {
    printf("changed frag: from %d => %d\n", frag, frag_tmp);
  }

  return fd;
}
