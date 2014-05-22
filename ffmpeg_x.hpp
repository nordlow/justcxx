/*! \file ffmpeg_x.hpp
 * \brief FFMPEG Convenince Functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-09-28 15:31
 */

#pragma once

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

/* prevent compilation error for \c UINT64_C */
#ifndef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS (1)
#endif

/* NOTE: ffmpeg headers must include these because ffmpeg developers are lazy. */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef HAVE_LIBAVFORMAT_AVFORMAT_H
#  include <libavformat/avformat.h> /* ffmpeg */
#endif
#ifdef HAVE_LIBAVCODEC_AVCODEC_H
#  include <libavcodec/avcodec.h> /* ffmpeg */
#endif
// #ifdef HAVE_LIBSWSCALE_SWSCALE_H
// #  include <libswscale/swscale.h>
// #endif
// #ifdef HAVE_LIBPOSTPROC_POSTPROCESS_H
// #  include <libpostproc/postprocess.h>
// #endif
#ifdef __cplusplus
}
#endif

void avformat_show(const AVFormatContext *ctx);

int av_ffmpeg_playback_test(int argc, char **argv);
