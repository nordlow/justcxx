/*!
 * \file alz3d.h
 * \brief Audiolizer that generates Linear Combinations of Time-Linearly
 * Fading Harmonic (Sine) Waves in a Simulated 3D Environment.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \keywords 3d-audio, hearable, spectrum, simulation, mapping, radio
 *
 * Currently used to \em reconstruct \em radio-traffic and \em map it
 * to the \em hearable spectrum \em 3D-space.
 *
 * Time-complexity is:
 * - O(srate * signum * schan) for simple stereo and
 * - O(srate * signum * schan * filtlen), for 3D-audio (when \c use_a3dflag is non-zero).
 */

#pragma once

#include "utils.h"
#include "timing.h"
#include "a3d.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Audiolizer Waveform Type
 */
typedef enum {
  ALZ3D_WAVE_SINE,		/**< Sine (Harmonic) */
} ALZ3D_WAVE_t;

/*!
 * Audiolizer Fade Type
 */
typedef enum {
  ALZ3D_FADE_NONE,		/**< No Fading */
  ALZ3D_FADE_IN,		/**< Fade-In */
  ALZ3D_FADE_OUT,		/**< Fade-Out */
  ALZ3D_FADE_INOUT,		/**< Fade-In then Fade-Out */
  ALZ3D_FADE_OUTIN,		/**< Fade-Out then Fade-In */
} ALZ3D_FADE_t;

/*!
 * Audiolizer Fade Window Type
 */
typedef enum {
  ALZ3D_FADEWIN_LINEAR,		/**< Linear */
  ALZ3D_FADEWIN_QUADRATIC,	/**< Quadratic */
} ALZ3D_FADEWIN_t;

/*!
 * Audiolizer Signal Model.
 */
typedef struct {
  float wfq;			/**< Wave Frequency [Hz]. */
  size_t scnt;			/**< Sample counter. */
  pTime toff;			/**< Time Offset for creation of signal. */
  float tdur;			/**< Signal Time Duration [s]. */
  float inv_tdur;		/**< Inverse of Signal duration [s]. */

  float amp;			/**< Amplitude. */

  /***
   * Azimuth (direction) in [Degrees].
   *
   * -   0: in front of listener
   * -  90: to the right of listener
   * - 180: behind listener
   * - 270: to the left of listener
   */
  float azim;
  float azim_cos;		/**< Cosine of the Aziumth */
  float azim_ampL;	    /**< Stereo amplitude for left channel. */
  float azim_ampR;	   /**< Stereo amplitude for right channel. */
  float azim_dtd; /** Direction Time Difference calculated from direction (azim). */

  ALZ3D_WAVE_t wave:2;		/**< Wave Type */
  ALZ3D_FADE_t fade:3;		/**< Fading Type */

  /***
   * Sample history.
   *
   * Needed by 3D-audio. Stored in ascending time-order (latest first) for
   * trivial and fast convolution with HRIR-filters using the function
   * a3d_filter().
   */
  float * hist;

  uint obsflag:1;		/**< Obselete flag. */
} Alz3dModel;

/*!
 * Audiolizer Main State.
 */
typedef struct {
  Alz3dModel * models; /**< The currently active Models ("in the air") */

  uint signum;                  /**< Number of active signals. */

  uint schan:2;                 /**< Number of sampling channels. */
  uint srate;                   /**< Sample rate [samples/second]. */
  uint sprecision:6;            /**< Sample precision [bits]. */
  bool sign_flag:1;             /**< Sample format is signed. */
  bool little_endian_flag:1;    /**< Sample Little endian flag. */

  /***
   * Maximum Amplitude.
   *
   * Depends on \c sprecision and \c sign_flag;
   */
  float maxamp;

  char * swbuf;			/**< Sample window buffer. */
  size_t swlen;			/**< Sample window buffer length. */

  size_t sgcnt;			/**< Sample global counter. */

  FILE * sfile;			/**< Samples file. */

  pTime tref;			/**< Time reference for start of sampling. */

  A3d a3d;			/**< 3D-audio structure. */
  uint a3dflag:1;		/**< Set if we should use 3D-audio. */
} Alz3d;

/*!
 * Initialize Audiolizer.
 */
void alz3d_init(Alz3d * alz,
		uint schan, uint srate, uint sprecision,
		bool sign_flag,
		bool little_endian_flag,
		uint use_a3dflag,
		const char * sfile_path);

/*!
 * Clear Audiolizer.
 */
int alz3d_clear(Alz3d * alz);

/*!
 * Register Signal at Audiolizer.
 *
 * \param[in] wfq Wave Frequency in [Hz].
 * \param[in] dur_sec Duration of Signal in [seconds].
 * \param[in] azim Azimuth Direction to Source in [Degrees].
 * \param[in] amp Normalized Amplitude between 0 and 1.
 * \param[in] fade Type of Signal Model Fading.
 */
int alz3d_addNow(Alz3d * alz,
		 float wfq, float dur_sec, float azim, float amp,
		 ALZ3D_WAVE_t wave, ALZ3D_FADE_t fade);

/*!
 * Sample \p swlen samples from current time and ahead (\p swlen / \p srate) in time.
 */
void alz3d_fill(Alz3d * alz, size_t swlen);

/*!
 * Sample Window Write.
 *
 * Write at most \p snum samples to \p fd.
 */
int alz3d_write(Alz3d * alz, int fd, size_t snum);

void alz3d_print(Alz3d * alz);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
