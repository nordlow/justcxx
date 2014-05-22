/*!
 * \file OSSio.h
 * \brief OSS Audio Handling
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <sys/soundcard.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Open audio device.
 *
 * \param[in] dev_path is the full path to the audio device.
 * \param[in] chan is the number of channels.
 * \param[in] rate is the sample rate.
 * \param[in] fmt is the format.
 * \param[in] frag is the fragmentation.
 */
int OSS_open(const char *dev_path, int chan, int rate, int fmt, int frag);

#ifdef __cplusplus
}
#endif
