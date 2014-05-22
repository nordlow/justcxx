/*!
 * \file ffmpeg_avcodec_tbb.hpp
 * \brief Intel TBB Variant of FFMPEG avcodec.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-07-01 12:54
 *
 * \usage Use \c avcodec_tbb_open() and \c avcodec_tbb_close() instead of \c avcodec_open() and \c avcodec_close().
 */

#pragma once

// Author Robert Nagy

// #include "avcodec_tbb.h"

#include <tbb/task.h>
#include <tbb/atomic.h>

extern "C"
{
#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

inline
int avcodec_tbb_task_execute(AVCodecContext* s,
                             std::function<int(void* arg, int arg_size, int jobnr, int threadnr)>&& func,
                             void* arg, int* ret, int count, int size)
{
    tbb::atomic<int> counter;
    counter = 0;
    // Execute s->thread_count number of tasks in parallel.
    tbb::parallel_for(0, s->thread_count, 1, [&](int threadnr)
                      {
                          while(true)
                          {
                              int jobnr = counter++;
                              if(jobnr >= count)
                                  break;

                              int r = func(arg, size, jobnr, threadnr);
                              if (ret)
                                  ret[jobnr] = r;
                          }
                      });
    return 0;
}

inline
int avcodec_tbb_task_execute(AVCodecContext* s,
                             int (*func)(AVCodecContext *c2, void *arg2),
                             void* arg, int* ret, int count, int size)
{
    return avcodec_tbb_task_execute(s, [&](void* arg, int arg_size, int jobnr, int threadnr) -> int
                                    {
                                        return func(s, reinterpret_cast<uint8_t*>(arg) + jobnr*size);
                                    }, arg, ret, count, size);
}

inline
int avcodec_tbb_task_execute2(AVCodecContext* s,
                              int (*func)(AVCodecContext* c2, void* arg2, int, int),
                              void* arg, int* ret, int count)
{
    return avcodec_tbb_task_execute(s, [&](void* arg, int arg_size, int jobnr, int threadnr) -> int
                                    {
                                        return func(s, arg, jobnr, threadnr);
                                    }, arg, ret, count, 0);
}

inline
void avcodec_tbb_task_init(AVCodecContext* s)
{
    static const size_t MAX_THREADS = 16; // See mpegvideo.h
    static int dummy_opaque;
    s->active_thread_type = FF_THREAD_SLICE;
    s->thread_opaque      = &dummy_opaque;
    s->execute            = avcodec_tbb_task_execute;
    s->execute2           = avcodec_tbb_task_execute2;
    s->thread_count       = MAX_THREADS; // We are using a task-scheduler, so use as many "threads/tasks" as possible.
}

inline
void avcodec_tbb_thread_free(AVCodecContext* s)
{
    s->thread_opaque = nullptr;
}

inline
int avcodec_tbb_open(AVCodecContext* avctx, AVCodec* codec)
{
    avctx->thread_count = 1;
    if((codec->capabilities & CODEC_CAP_SLICE_THREADS) && (avctx->thread_type & FF_THREAD_SLICE))
        avcodec_tbb_task_init(avctx);
// ff_thread_init will not be executed since thread_opaque != nullptr || thread_count == 1.
    return avcodec_open(avctx, codec);
}

inline
int avcodec_tbb_close(AVCodecContext* avctx)
{
    avcodec_tbb_thread_free(avctx);
// ff_thread_free will not be executed since thread_opaque == nullptr.
    return avcodec_close(avctx);
}
