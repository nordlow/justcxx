/*!
 * \file audioutils.h
 * \brief Wrappers for handling audio on Linux and Solaris.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __linux__
#  include <sys/ioctl.h>
#  include <sys/soundcard.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <math.h>
#  ifdef HAVE_ALSA_ASOUNDLIB_H
#    include <alsa/asoundlib.h>
#  endif
#endif
#ifdef __solaris__
#  include <sys/audioio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*! Audio Context
 */
typedef struct {
#ifdef __linux__
#  ifdef HAVE_ALSA_ASOUNDLIB_H
  snd_pcm_t *handle_ALSA;	/**< Handle used by Linux ALSA. */
#  endif
  int fd_OSS; /**< Audio Device File Descriptor used in Linux OSS. */
#endif
#  ifdef __solaris__
  int fd_AUDIO;	/**< Audio Device File Descriptor used in Solaris Audio. */
#endif
} AudioCtx;

void fix_byteorder(char * buf, size_t size);

/*!
 * Open Audio Context.
 *
 * \param[in] devname name of audio device to open.
 *
 * \param[in,out] chan pointer to number of channels.
 * \param[in,out] srate pointer to desired sample rate and may be
 *                modified upon return to the closest value supported
 *                by the audio device.
 * \param[in,out] frag fragment size
 *
 * \param[in,out] precision pointer to sample precision in number of bits.
 *
 * \param[in] sign_flag sampling sign.
 * \param[in] little_endian_flag indicate that byte-order is little endian.
 *
 * \return file descriptor for opened audio device upon success, otherwise -1.
 */
int
audioctx_open(AudioCtx * audio_ctx,
              const char * devname,
              int * chan, int * srate, int * frag,
              int * precision, bool sign_flag, bool little_endian_flag);

/*!
 * Close Audio Context.
 */
int
audioctx_close(AudioCtx * audio_ctx);

#ifdef __cplusplus
}
#endif
