/*!
 * \file alsa_utils.h
 * \brief Various Utilities for handling ALSA
 *         (Advanced Linux Sound Architecture).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <stdbool.h>
#include <alsa/asoundlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Asynchronous Notification Data.
 */
struct alsautils_async_private_data
{
  signed short *samples;
  snd_pcm_channel_area_t *areas;
  double phase;
  snd_pcm_uframes_t szP;	/**< Period Size. */
  double freq;
  uint rate;
  uint numC;		/**< Number of Channels. */
};

/*!
 * ALSA Transfer Method.
 *
 * \param[in] numC Number of Channels.
 */
struct alsautils_transfer_method
{
  const char *name;
  snd_pcm_access_t access;
  int (*transfer_loop) (snd_pcm_t * handle,
			signed short *samples,
			snd_pcm_channel_area_t * areas,
			snd_pcm_uframes_t szP,
			double freq, uint rate, uint numC);
};

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Generate Sine Wave.
 *
 * \param[in] numC Number of Channels.
 */
void
alsautils_generate_sine(const snd_pcm_channel_area_t * areas,
			snd_pcm_uframes_t offset, int count, double *_phase,
			double freq, uint rate, uint numC);

/*!
 * ALSA Set Transfer Method.
 *
 * \param[in] numC Number of Channels.
 * \param timeB Buffer Time.
 * \param timeP Period Time.
 * \param szB Buffer Size.
 * \param szP Period Size.
 */
int
alsautils_set_hwparams(snd_pcm_t * handle,
		       snd_pcm_hw_params_t * params, snd_pcm_access_t access,
		       snd_pcm_format_t format,
		       uint rate, uint numC,
		       uint * timeB,
		       uint * timeP,
		       snd_pcm_uframes_t * szB,
		       snd_pcm_uframes_t * szP);

/*!
 * ALSA Get Transfer Method.
 *
 * \param[in] szB Buffer Size.
 * \param[in] szP Period Size.
 */
int
alsautils_set_swparams(snd_pcm_t * handle,
		       snd_pcm_sw_params_t * swparams,
		       snd_pcm_uframes_t szB,
		       snd_pcm_uframes_t szP);

/*!
 * Underrun and Suspend Recovery
 */
int
alsautils_xrun_recovery(snd_pcm_t * handle, int err);

/*!
 * Transfer method - write only
 *
 * \param[in] numC Number of Channels.
 * \param[in] szP Period Size.
 */
int
alsautils_write_loop(snd_pcm_t * handle,
		     signed short *samples, snd_pcm_channel_area_t * areas,
		     snd_pcm_uframes_t szP,
		     double freq, uint rate, uint numC);

/*!
 * Transfer method - write and wait for room in buffer using poll
 */
int
alsautils_wait_for_poll(snd_pcm_t * handle, struct pollfd *ufds, uint count);

/*!
 * \param[in] szP Period Size.
 * \param[in] numC Number of Channels.
 */
int
alsautils_write_and_poll_loop(snd_pcm_t * handle,
			      signed short *samples,
			      snd_pcm_channel_area_t * areas,
			      snd_pcm_uframes_t szP,
			      double freq, uint rate,
			      uint numC);

void
alsautils_async_callback(snd_async_handler_t * ahandler);

/*!
 * \param[in] szP Period Size.
 * \param[in] numC Number of Channels.
 */
int
alsautils_async_loop(snd_pcm_t * handle,
		     signed short *samples,
		     snd_pcm_channel_area_t * areas,
		     snd_pcm_uframes_t szP,
		     double freq, uint rate, uint numC);

/*!
 * Transfer method - asynchronous notification + direct write
 */
void
alsautils_async_direct_callback(snd_async_handler_t * ahandler);

/*!
 * \param[in] szP Period Size.
 * \param[in] numC Number of Channels.
 */
int
alsautils_async_direct_loop(snd_pcm_t * handle,
			    signed short *samples,
			    snd_pcm_channel_area_t * areas,
			    snd_pcm_uframes_t szP,
			    double freq, uint rate,
			    uint numC);

/*!
 * Transfer method - direct write only
 * \param[in] szP Period Size.
 * \param[in] numC Number of Channels.
 */
int
alsautils_direct_loop(snd_pcm_t * handle,
		      signed short *samples,
		      snd_pcm_channel_area_t * areas,
		      snd_pcm_uframes_t szP,
		      double freq, uint rate, uint numC);

/*!
 * Transfer method - direct write only using mmap_write functions
 * \param[in] szP Period Size.
 * \param[in] numC Number of Channels.
 */
int
alsautils_direct_write_loop(snd_pcm_t * handle,
			    signed short *samples,
			    snd_pcm_channel_area_t * areas,
			    snd_pcm_uframes_t szP,
			    double freq, uint rate,
			    uint numC);

extern struct alsautils_transfer_method alsautils_transfer_methods[];

void
alsautils_help(void);

/*!
 * Transfer method - direct write only using mmap_write functions
 * \param numC Number of Channels.
 * \param timeB Buffer Time.
 * \param timeP Period Time.
 */
int
alsautils_read_options(int argc, char *argv[],
		       const char ** device, uint * rate, uint * numC,
		       double * freq,
		       uint * timeB, uint * timeP,
		       int * method, int * verbose);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * This example opens the default PCM device, sets some parameters, and then
 * displays the value of most of the hardware parameters. It does not perform
 * any sound playback or recording.
 */
int test_alsautils_params(void);

/*!
 * Reads from standard input and writes to the default PCM device for 5
 * seconds of data.
 */
int test_alsautils_write(bool nonblock_flag);

int test_alsautils(int argc, char *argv[]);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
