/*! \file a3d.h
 * \brief Infrastructure for 3D-Audio Generation.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! \em Complex Number in float precision. */
typedef struct {
  float re;                     /**< \em Real Part. */
  float im;                     /**< \em Imaginary Part. */
} complexf;

/*! Head Related Impulse Response (\em HRIR), Time-Domain Filter. */
typedef struct {
  int16_t * left;                 /**< \em Left Ear Filter. */
  int16_t * right;                /**< \em Right Ear Filter. */
} HRIR_t;

/*! Head Related Transfer Function (HRTF), Frequency (Fourier) Domain Filter.
 *
 * An HRTF is stored as left and right spectra for handy convolution.  The
 * compact HRTFs are 128 time samples, which gives us 128 \c complexf spectral
 * points (which are conjugate symmetric). Depending on the type of FFT used, we
 * will either have 128 (fft.h) or 64 \c complexf points.
 */
typedef struct {
  complexf * left;
  complexf * right;
} HRTF_t;

#define A3D_N_CTLS (64)		/**< Number of controls. */

/*! Context for 3D-audio. */
typedef struct {
  char * rootpath;
  char * fileext;

  /** A two-dimensional array of HRIRs, stored by elevation then by azimuth. */
  HRIR_t ** HRIRs;

  /** A two-dimensional array of HRTFs, stored by elevation then by azimuth. */
  HRTF_t ** HRTFs;

  size_t filtlen;	     /**< Filter length in number of stereo-samples. */
  float filtnorm;			/**< Filter norm. */
  float invfiltnorm;			/**< Inverse of filter norm. */

  short IRsmin_L, IRsmax_L; /**< Minimum & maximum sample of left HRIRs. */
  short IRsmin_R, IRsmax_R; /**< Minimum & maximum sample of right HRIRs. */

  int srate;			/**< Sample rate. */

  /***
   * \name Current and last values for position.
   *
   * Need these to print position values only when things change. Pretty gross.
   */
  /* @{ */
  int cur_el_idx;
  int cur_az_idx;
  bool cur_flip_flag;
  int last_el_idx;
  int last_az_idx;
  bool last_flip_flag;
  float gain;
  float old_gain;
  float cur_atten;
  float last_atten;
  int changeflag;
  /* @} */

  float * in0_LR;	    /**< Stereo interleaved input samples. */
  float * in1_LR;	    /**< Ditto, float buffer of above. */
  float * out0_LR;	    /**< Stereo interleaved output samples. */
  float * out1_LR;	    /**< Ditto, used only for crossfades. */

  complexf * fwd_buf;	    /**< Buffer for forward FFT. */
  complexf * inv_buf_L;	    /**< Buffer for inverse FFT, left. */
  complexf * inv_buf_R;	    /**< Buffer for inverse FFT, right. */

  float * ramp_up;		/**< Crossfade ramp \em up (0...1). */
  float * ramp_down;		/**< Crossfade ramp \em down (1...0). */

  /***
   * \name These point "other" input signal buffers.
   */
  /* @{ */
  float * p_newin_LR; /**< Pointer to new input buffer (in0_LR or in1_LR). */
  float * p_oldin_LR; /**< Pointer to old input buffer (in0_LR or in1_LR). */
  /* @} */

  complexf * filt_L;	/**< Left channel filter spectrum. */
  complexf * filt_R;	/**< Right channel filter spectrum. */
  complexf * oldfilt_L; /**< Previous left filter, when crossfading. */
  complexf * oldfilt_R; /**< Previous right filter, when crossfading. */

  /** Array of controls. */
  int ctls[A3D_N_CTLS];
} A3d;

/* ---------------------------- Group Separator ---------------------------- */

/*! Initialize 3D-Audio Context.
 *
 * \param[in] rootpath \em Path to where KEMAR \em data is placed.
 * \param[in] fileext File-Extension of a KEMAR data file.
 * signal source samples should be placed.
 * \param[in] srate Sample Rate.
 *
 * Use any of the following combinations of rootpath, fileext and sample rate.
 * - KEMAR/compact .dat 44100
 * - KEMAR/diffuse .dat 44100
 * - KEMAR/diffuse32 .res (resampled) 32000
 *
 * \return >= 0 if 3d-autio was successfully allocated and intialized, < 0
 * upon error (nothing was allocated so no \c a3d_clear() should be performed.
 */
int a3d_init(A3d * a3d, const char * rootpath, const char * fileext, int srate);

/*! Clear 3D-Audio Context. */
void a3d_clear(A3d * a3d);

/*! Filter the input samples \p ins of length \p inslen and write result to
 * [\p outL, \p outR].
 *
 * Input samples are stored in ascending time-order (latest first).
 */
void a3d_filter(A3d * a3d,
                float elev, float azim,
                float * ins, size_t inslen,
                float * outL, float * outR);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
