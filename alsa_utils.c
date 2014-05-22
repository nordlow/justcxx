#include "alsa_utils.h"				\
						\
  /* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include <string.h>
#include <sched.h>
#include <errno.h>
#include <getopt.h>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <float.h>

void
alsautils_generate_sine(const snd_pcm_channel_area_t * areas,
			snd_pcm_uframes_t offset, int count, double *_phase,
			double freq, uint rate, uint numC)
{
  const double max_phase = 2.0 * M_PI;
  double phase = *_phase;
  double step = max_phase * freq / (double) rate;
  double res;
  signed short *samples[numC];
  int steps[numC];
  uint chn;
  int ires;

  /* verify and prepare the contents of areas */
  for (chn = 0; chn < numC; chn++) {
    if ((areas[chn].first % 8) != 0) {
      printf("areas[%i].first == %i, aborting...\n", chn, areas[chn].first);
      exit(EXIT_FAILURE);
    }
    samples[chn] =
      (signed short *) (((unsigned char *) areas[chn].addr) +
			(areas[chn].first / 8));
    if ((areas[chn].step % 16) != 0) {
      printf("areas[%i].step == %i, aborting...\n", chn, areas[chn].step);
      exit(EXIT_FAILURE);
    }
    steps[chn] = areas[chn].step / 16;
    samples[chn] += offset * steps[chn];
  }

  /* fill the channel areas */
  while (count-- > 0) {
    res = sin(phase) * 32767;
    ires = res;
    for (chn = 0; chn < numC; chn++) {
      *samples[chn] = ires;
      samples[chn] += steps[chn];
    }
    phase += step;
    if (phase >= max_phase) phase -= max_phase; /* wrap around */
  }
  *_phase = phase;
}

int
alsautils_set_hwparams(snd_pcm_t * handle,
		       snd_pcm_hw_params_t * params, snd_pcm_access_t access,
		       snd_pcm_format_t format,
		       uint rate, uint numC,
		       uint * timeB,
		       uint * timeP,
		       snd_pcm_uframes_t * szB,
		       snd_pcm_uframes_t * szP)
{
  uint rrate;
  int err, dir;

  /* choose all parameters */
  err = snd_pcm_hw_params_any(handle, params);
  if (err < 0) {
    printf
      ("Broken configuration for playback: no configurations available: %s\n",
       snd_strerror(err));
    return err;
  }
  /* set the interleaved read/write format */
  err = snd_pcm_hw_params_set_access(handle, params, access);
  if (err < 0) {
    printf("Access type not available for playback: %s\n", snd_strerror(err));
    return err;
  }
  /* set the sample format */
  err = snd_pcm_hw_params_set_format(handle, params, format);
  if (err < 0) {
    printf("Sample format not available for playback: %s\n",
	   snd_strerror(err));
    return err;
  }
  /* set the count of numC */
  err = snd_pcm_hw_params_set_channels(handle, params, numC);
  if (err < 0) {
    printf("Channels count (%i) not available for playbacks: %s\n", numC,
	   snd_strerror(err));
    return err;
  }
  /* set the stream rate */
  rrate = rate;
  err = snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0);
  if (err < 0) {
    printf("Rate %iHz not available for playback: %s\n", rate,
	   snd_strerror(err));
    return err;
  }
  if (rrate != rate) {
    printf("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
    return -EINVAL;
  }
  /* set the buffer time */
  err =
    snd_pcm_hw_params_set_buffer_time_near(handle, params, timeB,
					   &dir);
  if (err < 0) {
    printf("Unable to set buffer time %i for playback: %s\n", *timeB,
	   snd_strerror(err));
    return err;
  }
  err = snd_pcm_hw_params_get_buffer_size(params, szB);
  if (err < 0) {
    printf("Unable to get buffer size for playback: %s\n", snd_strerror(err));
    return err;
  }
  /* set the period time */
  err =
    snd_pcm_hw_params_set_period_time_near(handle, params, timeP,
					   &dir);
  if (err < 0) {
    printf("Unable to set period time %i for playback: %s\n", *timeP,
	   snd_strerror(err));
    return err;
  }
  err = snd_pcm_hw_params_get_period_size(params, szP, &dir);
  if (err < 0) {
    printf("Unable to get period size for playback: %s\n", snd_strerror(err));
    return err;
  }
  /* write the parameters to device */
  err = snd_pcm_hw_params(handle, params);
  if (err < 0) {
    printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
    return err;
  }
  return 0;
}

int
alsautils_set_swparams(snd_pcm_t * handle,
		       snd_pcm_sw_params_t * swparams,
		       snd_pcm_uframes_t szB,
		       snd_pcm_uframes_t szP)
{
  int err;

  /* get the current swparams */
  err = snd_pcm_sw_params_current(handle, swparams);
  if (err < 0) {
    printf("Unable to determine current swparams for playback: %s\n",
	   snd_strerror(err));
    return err;
  }
  /* start the transfer when the buffer is almost full: */
  /* (szB / avail_min) * avail_min */
  err =
    snd_pcm_sw_params_set_start_threshold(handle, swparams,
					  (szB / szP) *
					  szP);
  if (err < 0) {
    printf("Unable to set start threshold mode for playback: %s\n",
	   snd_strerror(err));
    return err;
  }
  /* allow the transfer when at least szP samples can be processed */
  err = snd_pcm_sw_params_set_avail_min(handle, swparams, szP);
  if (err < 0) {
    printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
    return err;
  }
  /* align all transfers to 1 sample */
  err = snd_pcm_sw_params_set_xfer_align(handle, swparams, 1);
  if (err < 0) {
    printf("Unable to set transfer align for playback: %s\n",
	   snd_strerror(err));
    return err;
  }
  /* write the parameters to the playback device */
  err = snd_pcm_sw_params(handle, swparams);
  if (err < 0) {
    printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
    return err;
  }
  return 0;
}

int
alsautils_xrun_recovery(snd_pcm_t * handle, int err)
{
  if (err == -EPIPE) {	       /* under-run */
    err = snd_pcm_prepare(handle);
    if (err < 0)
      printf("Cannot recovery from underrun, prepare failed: %s\n",
	     snd_strerror(err));
    return 0;
  } else if (err == -ESTRPIPE) {
    while ((err = snd_pcm_resume(handle)) == -EAGAIN)
      sleep(1);		       /* wait until the suspend flag is released */
    if (err < 0) {
      err = snd_pcm_prepare(handle);
      if (err < 0)
	printf("Cannot recovery from suspend, prepare failed: %s\n",
	       snd_strerror(err));
    }
    return 0;
  }
  return err;
}

int
alsautils_write_loop(snd_pcm_t * handle,
		     signed short *samples, snd_pcm_channel_area_t * areas,
		     snd_pcm_uframes_t szP,
		     double freq, uint rate, uint numC)
{
  double phase = 0;
  signed short *ptr;
  int err, cptr;

  while (1) {
    alsautils_generate_sine(areas, 0, szP, &phase,
			    freq, rate, numC);
    ptr = samples;
    cptr = szP;
    while (cptr > 0) {
      err = snd_pcm_writei(handle, ptr, cptr);
      if (err == -EAGAIN)
	continue;
      if (err < 0) {
	if (alsautils_xrun_recovery(handle, err) < 0) {
	  printf("Write error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
	break;		       /* skip one period */
      }
      ptr += err * numC;
      cptr -= err;
    }
  }
}

int
alsautils_wait_for_poll(snd_pcm_t * handle, struct pollfd *ufds, uint count)
{
  ushort revents;

  while (1) {
    poll(ufds, count, -1);
    snd_pcm_poll_descriptors_revents(handle, ufds, count, &revents);
    if (revents & POLLERR)
      return -EIO;
    if (revents & POLLOUT)
      return 0;
  }
}

int
alsautils_write_and_poll_loop(snd_pcm_t * handle,
			      signed short *samples,
			      snd_pcm_channel_area_t * areas,
			      snd_pcm_uframes_t szP,
			      double freq, uint rate,
			      uint numC)
{
  struct pollfd *ufds;
  double phase = 0;
  signed short *ptr;
  int err, count, cptr, init;

  count = snd_pcm_poll_descriptors_count(handle);
  if (count <= 0) {
    printf("Invalid poll descriptors count\n");
    return count;
  }

  ufds = (struct pollfd*)malloc(sizeof(struct pollfd) * count);
  if (ufds == NULL) {
    printf("No enough memory\n");
    return -ENOMEM;
  }
  if ((err = snd_pcm_poll_descriptors(handle, ufds, count)) < 0) {
    printf("Unable to obtain poll descriptors for playback: %s\n",
	   snd_strerror(err));
    return err;
  }

  init = 1;
  while (1) {
    if (!init) {
      err = alsautils_wait_for_poll(handle, ufds, count);
      if (err < 0) {
	if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN ||
	    snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED) {
	  err =
	    snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
	  if (alsautils_xrun_recovery(handle, err) < 0) {
	    printf("Write error: %s\n", snd_strerror(err));
	    exit(EXIT_FAILURE);
	  }
	  init = 1;
	} else {
	  printf("Wait for poll failed\n");
	  return err;
	}
      }
    }

    alsautils_generate_sine(areas, 0, szP, &phase,
			    freq, rate, numC);
    ptr = samples;
    cptr = szP;
    while (cptr > 0) {
      err = snd_pcm_writei(handle, ptr, cptr);
      if (err < 0) {
	if (alsautils_xrun_recovery(handle, err) < 0) {
	  printf("Write error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
	init = 1;
	break;		       /* skip one period */
      }
      if (snd_pcm_state(handle) == SND_PCM_STATE_RUNNING)
	init = 0;
      ptr += err * numC;
      cptr -= err;
      if (cptr == 0)
	break;
      /* it is possible, that the initial buffer cannot store */
      /* all data from the last period, so wait awhile */
      err = alsautils_wait_for_poll(handle, ufds, count);
      if (err < 0) {
	if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN ||
	    snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED) {
	  err =
	    snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
	  if (alsautils_xrun_recovery(handle, err) < 0) {
	    printf("Write error: %s\n", snd_strerror(err));
	    exit(EXIT_FAILURE);
	  }
	  init = 1;
	} else {
	  printf("Wait for poll failed\n");
	  return err;
	}
      }
    }
  }
}

void
alsautils_async_callback(snd_async_handler_t * ahandler)
{
  snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
  struct alsautils_async_private_data *data =
    snd_async_handler_get_callback_private(ahandler);
  signed short *samples = data->samples;
  snd_pcm_channel_area_t *areas = data->areas;
  snd_pcm_sframes_t avail;
  int err;
  snd_pcm_uframes_t szP = data->szP;
  double freq = data->freq;
  uint rate = data->rate;
  uint numC = data->numC;

  avail = snd_pcm_avail_update(handle);
  while (avail >= szP) {
    alsautils_generate_sine(areas, 0, szP, &data->phase,
			    freq, rate, numC);
    err = snd_pcm_writei(handle, samples, szP);
    if (err < 0) {
      printf("Initial write error: %s\n", snd_strerror(err));
      exit(EXIT_FAILURE);
    }
    if (err != szP) {
      printf("Initial write error: written %i expected %li\n", err,
	     szP);
      exit(EXIT_FAILURE);
    }
    avail = snd_pcm_avail_update(handle);
  }
}

int
alsautils_async_loop(snd_pcm_t * handle,
		     signed short *samples,
		     snd_pcm_channel_area_t * areas,
		     snd_pcm_uframes_t szP,
		     double freq, uint rate,
		     uint numC)
{
  struct alsautils_async_private_data data;
  snd_async_handler_t *ahandler;
  int err, count;

  data.samples = samples;
  data.areas = areas;
  data.phase = 0;
  data.szP = szP;
  data.freq = freq;
  data.rate = rate;
  data.numC = numC;

  err = snd_async_add_pcm_handler(&ahandler, handle, alsautils_async_callback, &data);
  if (err < 0) {
    printf("Unable to register async handler\n");
    exit(EXIT_FAILURE);
  }
  for (count = 0; count < 2; count++) {
    alsautils_generate_sine(areas, 0, szP, &data.phase,
			    freq, rate, numC);
    err = snd_pcm_writei(handle, samples, szP);
    if (err < 0) {
      printf("Initial write error: %s\n", snd_strerror(err));
      exit(EXIT_FAILURE);
    }
    if (err != szP) {
      printf("Initial write error: written %i expected %li\n", err,
	     szP);
      exit(EXIT_FAILURE);
    }
  }
  err = snd_pcm_start(handle);
  if (err < 0) {
    printf("Start error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }

  /* because all other work is done in the signal handler,
     suspend the process */
  while (1) {
    sleep(1);
  }
}

void
alsautils_async_direct_callback(snd_async_handler_t * ahandler)
{
  snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
  struct alsautils_async_private_data *data =
    snd_async_handler_get_callback_private(ahandler);
  const snd_pcm_channel_area_t *my_areas;
  snd_pcm_uframes_t offset, frames, size;
  snd_pcm_sframes_t avail, commitres;
  snd_pcm_state_t state;
  int first = 0, err;

  snd_pcm_uframes_t szP = data->szP;
  double freq = data->freq;
  uint rate = data->rate;
  uint numC = data->numC;

  while (1) {
    state = snd_pcm_state(handle);
    if (state == SND_PCM_STATE_XRUN) {
      err = alsautils_xrun_recovery(handle, -EPIPE);
      if (err < 0) {
	printf("XRUN recovery failed: %s\n", snd_strerror(err));
	exit(EXIT_FAILURE);
      }
      first = 1;
    } else if (state == SND_PCM_STATE_SUSPENDED) {
      err = alsautils_xrun_recovery(handle, -ESTRPIPE);
      if (err < 0) {
	printf("SUSPEND recovery failed: %s\n", snd_strerror(err));
	exit(EXIT_FAILURE);
      }
    }
    avail = snd_pcm_avail_update(handle);
    if (avail < 0) {
      err = alsautils_xrun_recovery(handle, avail);
      if (err < 0) {
	printf("avail update failed: %s\n", snd_strerror(err));
	exit(EXIT_FAILURE);
      }
      first = 1;
      continue;
    }
    if (avail < szP) {
      if (first) {
	first = 0;
	err = snd_pcm_start(handle);
	if (err < 0) {
	  printf("Start error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
      } else {
	break;
      }
      continue;
    }
    size = szP;
    while (size > 0) {
      frames = size;
      err = snd_pcm_mmap_begin(handle, &my_areas, &offset, &frames);
      if (err < 0) {
	if ((err = alsautils_xrun_recovery(handle, err)) < 0) {
	  printf("MMAP begin avail error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
	first = 1;
      }
      alsautils_generate_sine(my_areas, offset, frames, &data->phase,
			      freq, rate, numC);
      commitres = snd_pcm_mmap_commit(handle, offset, frames);
      if (commitres < 0 || (snd_pcm_uframes_t) commitres != frames) {
	if ((err =
	     alsautils_xrun_recovery(handle,
				     commitres >= 0 ? -EPIPE : commitres)) < 0) {
	  printf("MMAP commit error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
	first = 1;
      }
      size -= frames;
    }
  }
}

int
alsautils_async_direct_loop(snd_pcm_t * handle,
			    signed short *samples,
			    snd_pcm_channel_area_t * areas,
			    snd_pcm_uframes_t szP,
			    double freq, uint rate,
			    uint numC)
{
  struct alsautils_async_private_data data;
  snd_async_handler_t *ahandler;
  const snd_pcm_channel_area_t *my_areas;
  snd_pcm_uframes_t offset, frames, size;
  snd_pcm_sframes_t commitres;
  int err, count;

  data.samples = NULL;	       /* we do not require the global sample area for direct write */
  data.areas = NULL;	       /* we do not require the global areas for direct write */
  data.phase = 0;
  data.szP = szP;
  data.freq = freq;
  data.rate = rate;
  data.numC = numC;

  err =
    snd_async_add_pcm_handler(&ahandler, handle, alsautils_async_direct_callback,
			      &data);
  if (err < 0) {
    printf("Unable to register async handler\n");
    exit(EXIT_FAILURE);
  }
  for (count = 0; count < 2; count++) {
    size = szP;
    while (size > 0) {
      frames = size;
      err = snd_pcm_mmap_begin(handle, &my_areas, &offset, &frames);
      if (err < 0) {
	if ((err = alsautils_xrun_recovery(handle, err)) < 0) {
	  printf("MMAP begin avail error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
      }
      alsautils_generate_sine(my_areas, offset, frames, &data.phase,
			      freq, rate, numC);
      commitres = snd_pcm_mmap_commit(handle, offset, frames);
      if (commitres < 0 || (snd_pcm_uframes_t) commitres != frames) {
	if ((err =
	     alsautils_xrun_recovery(handle,
				     commitres >= 0 ? -EPIPE : commitres)) < 0) {
	  printf("MMAP commit error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
      }
      size -= frames;
    }
  }
  err = snd_pcm_start(handle);
  if (err < 0) {
    printf("Start error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }

  /* because all other work is done in the signal handler,
     suspend the process */
  while (1) {
    sleep(1);
  }
}

int
alsautils_direct_loop(snd_pcm_t * handle,
		      signed short *samples,
		      snd_pcm_channel_area_t * areas,
		      snd_pcm_uframes_t szP,
		      double freq, uint rate,
		      uint numC)
{
  double phase = 0;
  const snd_pcm_channel_area_t *my_areas;
  snd_pcm_uframes_t offset, frames, size;
  snd_pcm_sframes_t avail, commitres;
  snd_pcm_state_t state;
  int err, first = 1;

  while (1) {
    state = snd_pcm_state(handle);
    if (state == SND_PCM_STATE_XRUN) {
      err = alsautils_xrun_recovery(handle, -EPIPE);
      if (err < 0) {
	printf("XRUN recovery failed: %s\n", snd_strerror(err));
	return err;
      }
      first = 1;
    } else if (state == SND_PCM_STATE_SUSPENDED) {
      err = alsautils_xrun_recovery(handle, -ESTRPIPE);
      if (err < 0) {
	printf("SUSPEND recovery failed: %s\n", snd_strerror(err));
	return err;
      }
    }
    avail = snd_pcm_avail_update(handle);
    if (avail < 0) {
      err = alsautils_xrun_recovery(handle, avail);
      if (err < 0) {
	printf("avail update failed: %s\n", snd_strerror(err));
	return err;
      }
      first = 1;
      continue;
    }
    if (avail < szP) {
      if (first) {
	first = 0;
	err = snd_pcm_start(handle);
	if (err < 0) {
	  printf("Start error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
      } else {
	err = snd_pcm_wait(handle, -1);
	if (err < 0) {
	  if ((err = alsautils_xrun_recovery(handle, err)) < 0) {
	    printf("snd_pcm_wait error: %s\n", snd_strerror(err));
	    exit(EXIT_FAILURE);
	  }
	  first = 1;
	}
      }
      continue;
    }
    size = szP;
    while (size > 0) {
      frames = size;
      err = snd_pcm_mmap_begin(handle, &my_areas, &offset, &frames);
      if (err < 0) {
	if ((err = alsautils_xrun_recovery(handle, err)) < 0) {
	  printf("MMAP begin avail error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
	first = 1;
      }
      alsautils_generate_sine(my_areas, offset, frames, &phase,
			      freq, rate, numC);
      commitres = snd_pcm_mmap_commit(handle, offset, frames);
      if (commitres < 0 || (snd_pcm_uframes_t) commitres != frames) {
	if ((err =
	     alsautils_xrun_recovery(handle,
				     commitres >= 0 ? -EPIPE : commitres)) < 0) {
	  printf("MMAP commit error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
	first = 1;
      }
      size -= frames;
    }
  }
}

int
alsautils_direct_write_loop(snd_pcm_t * handle,
			    signed short *samples,
			    snd_pcm_channel_area_t * areas,
			    snd_pcm_uframes_t szP,
			    double freq, uint rate,
			    uint numC)
{
  double phase = 0;
  signed short *ptr;
  int err, cptr;

  while (1) {
    alsautils_generate_sine(areas, 0, szP, &phase,
			    freq, rate, numC);
    ptr = samples;
    cptr = szP;
    while (cptr > 0) {
      err = snd_pcm_mmap_writei(handle, ptr, cptr);
      if (err == -EAGAIN)
	continue;
      if (err < 0) {
	if (alsautils_xrun_recovery(handle, err) < 0) {
	  printf("Write error: %s\n", snd_strerror(err));
	  exit(EXIT_FAILURE);
	}
	break;		       /* skip one period */
      }
      ptr += err * numC;
      cptr -= err;
    }
  }
}

struct alsautils_transfer_method alsautils_transfer_methods[] = {
  {"write", SND_PCM_ACCESS_RW_INTERLEAVED, alsautils_write_loop},
  {"write_and_poll", SND_PCM_ACCESS_RW_INTERLEAVED, alsautils_write_and_poll_loop},
  {"async", SND_PCM_ACCESS_RW_INTERLEAVED, alsautils_async_loop},
  {"async_direct", SND_PCM_ACCESS_MMAP_INTERLEAVED, alsautils_async_direct_loop},
  {"direct_interleaved", SND_PCM_ACCESS_MMAP_INTERLEAVED, alsautils_direct_loop},
  {"direct_noninterleaved", SND_PCM_ACCESS_MMAP_NONINTERLEAVED, alsautils_direct_loop},
  {"direct_write", SND_PCM_ACCESS_MMAP_INTERLEAVED, alsautils_direct_write_loop},
  {NULL, SND_PCM_ACCESS_RW_INTERLEAVED, NULL}
};

void
alsautils_help(void)
{
  int k;
  printf("Usage: pcm [OPTION]... [FILE]...\n"
	 "-h,--help	help\n"
	 "-D,--device	playback device\n"
	 "-r,--rate	stream rate in Hz\n"
	 "-c,--numC	count of numC in stream\n"
	 "-f,--frequency	sine wave frequency in Hz\n"
	 "-b,--buffer	ring buffer size in us\n"
	 "-p,--period	period size in us\n"
	 "-m,--method	transfer method\n"
	 "-v,--verbose   show the PCM setup parameters\n" "\n");
  printf("Recognized sample formats are:");
  for (k = 0; k < SND_PCM_FORMAT_LAST; ++k) {
    const char *s = snd_pcm_format_name(k);
    if (s)
      printf(" %s", s);
  }
  printf("\n");
  printf("Recognized transfer methods are:");
  for (k = 0; alsautils_transfer_methods[k].name; k++)
    printf(" %s", alsautils_transfer_methods[k].name);
  printf("\n");
}

int
alsautils_read_options(int argc, char *argv[],
		       const char ** device, uint * rate, uint * numC,
		       double * freq, uint * timeB, uint * timeP,
		       int * method, int * verbose)
{
  int morehelp = 0;

  struct option long_option[] = {
    {"help", 0, NULL, 'h'},
    {"device", 1, NULL, 'D'},
    {"rate", 1, NULL, 'r'},
    {"numC", 1, NULL, 'c'},
    {"frequency", 1, NULL, 'f'},
    {"buffer", 1, NULL, 'b'},
    {"period", 1, NULL, 'p'},
    {"method", 1, NULL, 'm'},
    {"verbose", 1, NULL, 'v'},
    {NULL, 0, NULL, 0},
  };

  while (1) {
    int c;
    if ((c =
	 getopt_long(argc, argv, "hD:r:c:f:b:p:m:v", long_option, NULL)) < 0)
      break;
    switch (c) {
    case 'h':
      morehelp++;
      break;
    case 'D':
      *device = strdup(optarg);
      break;
    case 'r':
      *rate = atoi(optarg);
      *rate = *rate < 4000 ? 4000 : *rate;
      *rate = *rate > 196000 ? 196000 : *rate;
      break;
    case 'c':
      *numC = atoi(optarg);
      *numC = *numC < 1 ? 1 : *numC;
      *numC = *numC > 1024 ? 1024 : *numC;
      break;
    case 'f':
      *freq = atoi(optarg);
      *freq = *freq < 10 ? 10 : *freq;
      *freq = *freq > 20000 ? 20000 : *freq;
      break;
    case 'b':
      *timeB = atoi(optarg);
      *timeB = *timeB < 1000 ? 1000 : *timeB;
      *timeB = *timeB > 1000000 ? 1000000 : *timeB;
      break;
    case 'p':
      *timeP = atoi(optarg);
      *timeP = *timeP < 1000 ? 1000 : *timeP;
      *timeP = *timeP > 1000000 ? 1000000 : *timeP;
      break;
    case 'm':
      for (*method = 0; alsautils_transfer_methods[*method].name; (*method)++)
	if (!strcasecmp(alsautils_transfer_methods[*method].name, optarg))
	  break;
      if (alsautils_transfer_methods[*method].name == NULL)
	*method = 0;
      break;
    case 'v':
      *verbose = 1;
      break;
    }
  }

  if (morehelp) {
    alsautils_help();
    return 0;
  } else {
    return 1;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

int
test_alsautils_params(void)
{
  int rc;
  snd_pcm_t *hnd;
  snd_pcm_hw_params_t *params;
  uint val, val2;
  int dir;
  snd_pcm_uframes_t frames;

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&hnd, "default", SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(hnd, params);

  /* Set the desired hardware parameters. */
  snd_pcm_hw_params_set_access(hnd, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(hnd, params, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_channels(hnd, params, 2); /* stereo */
  val = 22500;    /* 44100 bits/second sampling rate (CD quality) */
  snd_pcm_hw_params_set_rate_near(hnd, params, &val, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(hnd, params);
  if (rc < 0) {
    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
    exit(1);
  }

  /* Display information about the PCM interface */
  printf("PCM hnd name = '%s'\n", snd_pcm_name(hnd));
  printf("PCM state = %s\n", snd_pcm_state_name(snd_pcm_state(hnd)));
  snd_pcm_hw_params_get_access(params, (snd_pcm_access_t *) & val);
  printf("access type = %s\n", snd_pcm_access_name((snd_pcm_access_t) val));
  snd_pcm_hw_params_get_format(params, &val);
  printf("format = '%s' (%s)\n",
	 snd_pcm_format_name((snd_pcm_format_t) val),
	 snd_pcm_format_description((snd_pcm_format_t) val));
  snd_pcm_hw_params_get_subformat(params, (snd_pcm_subformat_t *) & val);
  printf("subformat = '%s' (%s)\n",
	 snd_pcm_subformat_name((snd_pcm_subformat_t) val),
	 snd_pcm_subformat_description((snd_pcm_subformat_t) val));
  snd_pcm_hw_params_get_channels(params, &val);
  printf("numC = %d\n", val);
  snd_pcm_hw_params_get_rate(params, &val, &dir);
  printf("rate = %d bps\n", val);
  snd_pcm_hw_params_get_period_time(params, &val, &dir);
  printf("period time = %d us\n", val);
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  printf("period size = %d frames\n", (int) frames);
  snd_pcm_hw_params_get_buffer_time(params, &val, &dir);
  printf("buffer time = %d us\n", val);
  snd_pcm_hw_params_get_buffer_size(params, (snd_pcm_uframes_t *) & val);
  printf("buffer size = %d frames\n", val);
  snd_pcm_hw_params_get_periods(params, &val, &dir);
  printf("periods per buffer = %d frames\n", val);
  snd_pcm_hw_params_get_rate_numden(params, &val, &val2);
  printf("exact rate = %d/%d bps\n", val, val2);
  val = snd_pcm_hw_params_get_sbits(params);
  printf("significant bits = %d\n", val);
  snd_pcm_hw_params_get_tick_time(params, &val, &dir);
  printf("tick time = %d us\n", val);
  val = snd_pcm_hw_params_is_batch(params);
  printf("is batch = %d\n", val);
  val = snd_pcm_hw_params_is_block_transfer(params);
  printf("is block transfer = %d\n", val);
  val = snd_pcm_hw_params_is_double(params);
  printf("is double = %d\n", val);
  val = snd_pcm_hw_params_is_half_duplex(params);
  printf("is half duplex = %d\n", val);
  val = snd_pcm_hw_params_is_joint_duplex(params);
  printf("is joint duplex = %d\n", val);
  val = snd_pcm_hw_params_can_overrange(params);
  printf("can overrange = %d\n", val);
  val = snd_pcm_hw_params_can_mmap_sample_resolution(params);
  printf("can mmap = %d\n", val);
  val = snd_pcm_hw_params_can_pause(params);
  printf("can pause = %d\n", val);
  val = snd_pcm_hw_params_can_resume(params);
  printf("can resume = %d\n", val);
  val = snd_pcm_hw_params_can_sync_start(params);
  printf("can sync start = %d\n", val);

  snd_pcm_close(hnd);

  return 0;
}

int
test_alsautils_write(bool nonblock_flag)
{
  long loops;
  int rc;
  int size;
  snd_pcm_t *hnd;		/* handle */
  snd_pcm_hw_params_t *params;	/* parameters */
  uint val;
  int dir;
  snd_pcm_uframes_t frames;	/* frames of audiosamples */
  char *buf;

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&hnd, "default", SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
    exit(1);
  }

  if (nonblock_flag) {
    if ((rc = snd_pcm_nonblock(hnd, 1)) < 0) {
      fprintf(stderr, "unable to set nonblocking mode: %s\n",
	      snd_strerror(rc));
    }
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(hnd, params);

  /* Set the desired hardware parameters. */
  snd_pcm_hw_params_set_access(hnd, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(hnd, params, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_channels(hnd, params, 2); /* stereo */
  val = 22500;    /* 44100 bits/second sampling rate (CD quality) */
  snd_pcm_hw_params_set_rate_near(hnd, params, &val, &dir);
  frames = 32;			/* Set period size to 32 frames. */
  snd_pcm_hw_params_set_period_size_near(hnd, params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(hnd, params);
  if (rc < 0) {
    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
    return -1;
  }

  /* Use a buf large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  size = frames * 4;	       /* 2 bytes/sample, 2 numC */
  buf = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params, &val, &dir);

  /* 5 seconds in microseconds divided by period time */
  loops = 5000000 / val;
  while (loops > 0) {
    loops--;

    /* read size bytes from stdin to buf */
    rc = read(fileno(stdin), buf, size);

    if (rc == 0) {
      fprintf(stderr, "end of file on input\n");
      break;
    } else if (rc != size) {
      fprintf(stderr, "short read: read %d bytes\n", rc);
    }

    rc = snd_pcm_writei(hnd, buf, frames);
    if (rc == -EPIPE) {		/* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(hnd);
    } else if (rc == -EBUSY) {
      fprintf(stderr, "buffer data...\n");
    } else if (rc < 0) {
      fprintf(stderr, "error from writei: %s\n", snd_strerror(rc));
    } else if (rc != (int) frames) {
      fprintf(stderr, "short write, write %d frames\n", rc);
    }
  }

  snd_pcm_drain(hnd);
  snd_pcm_close(hnd);
  free(buf);

  return 0;
}

int
test_alsautils(int argc, char *argv[])
{
  static const char *device = "default";	/* playback device */
  snd_pcm_format_t format = SND_PCM_FORMAT_S16;	/* sample format */
  uint rate = 8000;				/* stream rate */
  uint numC = 2;		/* count of numC */
  uint timeB = 500000;	/* ring buffer length in us */
  uint timeP = 100000;	/* period time in us */
  double freq = 440;		/* sinusoidal wave frequency in Hz */
  int verbose = 0;		/* verbose flag */

  snd_pcm_uframes_t szB;	/* buffer size */
  snd_pcm_uframes_t szP;	/* period size */
  snd_output_t *output = NULL;

  snd_pcm_t *handle;
  int err;
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  int method = 0;
  signed short *samples;
  uint chn;
  snd_pcm_channel_area_t *areas;

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_sw_params_alloca(&swparams);

  if (alsautils_read_options(argc, argv, &device,
			     &rate, &numC, &freq,
			     &timeB, &timeP, &method,
			     &verbose) == 0) { return 0; }

  err = snd_output_stdio_attach(&output, stdout, 0);
  if (err < 0) {
    printf("Output failed: %s\n", snd_strerror(err));
    return 0;
  }

  printf("Playback device is %s\n", device);
  printf("Stream parameters are %iHz, %s, %i numC\n", rate,
	 snd_pcm_format_name(format), numC);
  printf("Sine wave rate is %.4fHz\n", freq);
  printf("Using transfer method: %s\n", alsautils_transfer_methods[method].name);

  if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    printf("Playback open error: %s\n", snd_strerror(err));
    return 0;
  }

  if ((err = alsautils_set_hwparams(handle, hwparams,
				    alsautils_transfer_methods[method].access,
				    format,
				    rate, numC,
				    &timeB,
				    &timeP,
				    &szB,
				    &szP)) < 0) {
    printf("Setting of hwparams failed: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }
  if ((err = alsautils_set_swparams(handle, swparams,
				    szB,
				    szP)) < 0) {
    printf("Setting of swparams failed: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }

  if (verbose > 0)
    snd_pcm_dump(handle, output);

  samples =
    malloc((szP * numC * snd_pcm_format_width(format)) / 8);
  if (samples == NULL) {
    printf("No enough memory\n");
    exit(EXIT_FAILURE);
  }

  areas = calloc(numC, sizeof(snd_pcm_channel_area_t));
  if (areas == NULL) {
    printf("No enough memory\n");
    exit(EXIT_FAILURE);
  }
  for (chn = 0; chn < numC; chn++) {
    areas[chn].addr = samples;
    areas[chn].first = chn * 16;
    areas[chn].step = numC * 16;
  }

  err = alsautils_transfer_methods[method].
    transfer_loop(handle, samples, areas, szP, freq, rate, numC);
  if (err < 0)
    printf("Transfer failed: %s\n", snd_strerror(err));

  free(areas);
  free(samples);
  snd_pcm_close(handle);
  return 0;
}
