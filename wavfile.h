#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  union
  {
    uchar s8;
    uchar s16;
  } bipol;			/* bipolar storage */
  float *data;
  uint samp_num;	/* number of samples (per channel) */
  uint samp_rate;	/* sampling rate in Hz */
  uchar chan_num:4;	/* number of channels (1 or 2) */
  uchar prec:4;		/* sampling precision in bytes (1 or 2) */
} WavFile_t;

void WAV_init(WavFile_t * wav);
void WAV_clear(WavFile_t * wav);

int WAV_read(const char *path, WavFile_t * wav);

void WAV_print(WavFile_t const *wav);

#ifdef __cplusplus
}
#endif
