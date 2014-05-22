#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "utils.h"
#include "meman.h"
#include "a3d.h"
#include "linfilt.h"
#include "dot.h"
#include "decode.h"
#include "stdio_x.h"
#include "extremes.h"

/*! Single Precision Real to Complex Fast Fourier Transform (FFT).
 *
 * \param[in] job -1 for forward FFT and 1 for backward FFT.
 *
 * \param[in] n on entry, N specifies the number of elements in the 1D FFT.
 * Unchanged on exit.
 *
 * @param[in,out] array contains float array of dimension at least 2*( (N+2)/2 ).
 * On entry it contains the elements for the FFT.
 * On exit, SEQUENCE is overwritten by the 1D transform.
 *
 * \param[in] inc Increment between two consecutive elements of the sequence.
 * Unchanged on exit.
 */
int sfft1du(int job, size_t n, float *array, int inc)
{
  return 0;
}

complexf *
cfltv_calloc(size_t n)
{
  return calloc(n * sizeof(complexf), 1);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Length of the HRTF filters in number stereo-pairs (rounded up to FFT size). */
#define A3D_FILTLEN (128L)

/*! FFT buffers contain this many complexf points. */
#define A3D_BUFLEN (A3D_FILTLEN * 2)

/*! Miscellaneous routines for \em Spatializer Demo taken from various library
 * modules.
 */
void
s_split(short *stereo,		/* ptr to stereo samples */
        int ns,			/* number stereo pairs */
        short *left,		/* left result, pre-allocated */
        short *right)
{			       /* right result, pre-allocated */
  short *end;
  end = stereo + ns * 2;
  while (stereo < end) {
    *left++ = *stereo++;
    *right++ = *stereo++;
  }
}

static inline complexf
cf_mul(complexf a, complexf b)
{
  complexf c;
  c.re = a.re * b.re - a.im * b.im;
  c.im = a.im * b.re + a.re * b.im;
  return c;
}

/*! Optimized multiply of two conjugate symmetric arrays (c = a * b). */
void
c_opt_mul(const complexf *a, const complexf *b, complexf *c, int n)
{
  int i, half = n >> 1;
  for (i = 0; i <= half; i++)
    c[i] = cf_mul(*a++, *b++);
  for (i = 1; i < half; i++) {
    c[half + i].re = c[half - i].re;
    c[half + i].im = -c[half - i].im;
  }
}

/*! Load all whole pairs of 16-bit samples from file named fname.
 *
 * \param[out] p_n is set to number of 16-bit samples that were read from file.
 * p_n/2 is the number of stereo-pairs that were read.
 *
 * \return pointer to buffer of shorts where samples were were allocated and
 * set, or 0 if data could not be loaded.
 */
short *
load_spairs_S16be(char *fname, int *p_n)
{
  FILE *fp;
  size_t n;
  char *buf;                    /* file buffer */
  short * sbuf;			/* sample buffer */
  size_t i, sz;

  *p_n = 1;

  if ((fp = fopen(fname, "rb")) == NULL) {
/*     leprintf("can't fopen %s\n", fname); */
    return 0;
  }

  sz = get_fsize(fp);		/* sample byte size */
  *p_n = n = sz / sizeof(short); /* number of stereo-pairs */
  buf = (char*)malloc(n * sizeof(short));

  if (fread(buf, sizeof(short), n, fp) != n) {
/*     leprintf("error reading from file\n"); */
    free(buf);
    return 0;
    fclose(fp);
  } else {
    fclose(fp);
  }

  /* konvertera byte-order och kopiera till short-array */
  sbuf = shortarray_calloc(n);
  for (i = 0; i < n; i++) {
    sbuf[i] = bdec_s16be(buf + i * 2); /* hantera byte-order */
  }

  return sbuf;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Code to load/transform/lookup HRTFs and maintain control
 * values for 3Daudio spatializer.
 */

/*!
 * Default root path, can be overridden by setting environment
 * variable ROOT_ENV.
 */
#define ROOT_ENV "HRTFROOT"

/*!
 * File format (without extension) for compact, diffuse and diffuse32k data.
 */
#define PATH_FMT "%s/elev%d/H%de%03da%s"

/* #define a3d->fileext ".res" */

#define MIN_ELEV (-40)
#define MAX_ELEV (90)
#define ELEV_INC (10)
#define N_ELEV (((MAX_ELEV - MIN_ELEV) / ELEV_INC) + 1)

/*!
 * This array gives the total number of azimuths measured per elevation,
 * and hence the AZIMUTH INCREMENT. Note that only azimuths up to and
 * including 180 degrees actually exist on the file system (because the data
 * is symmetrical.
 */
static const int g_azim_nums[N_ELEV] = {
  56, 60, 72, 72, 72, 72, 72, 60, 56, 45, 36, 24, 12, 1
};

#define MIN_AZIM (-180)
#define MAX_AZIM (180)

#define MIN_ATTEN (0)		/**< Minimum Attenuation [dB] */
#define MAX_ATTEN (20)		/**< Maximum Attenuation [dB] */

/*!
 * Control stuff.
 */
#define CTL_ELEV (4)		/**< Elevation. */
#define CTL_AZIM (2)		/**< Azimuth */
#define CTL_ATTEN (3)		/**< Attenuation */

#define MAX_CTL_VAL	(127)

#define CTL_VAL(ctl_num)	((float) a3d->ctls[ctl_num] / MAX_CTL_VAL)

/*! Get the number of azimuths filters stored at elevation index el_idx. */
int
get_azimnum(int el_idx)
{
  return (g_azim_nums[el_idx] / 2) + 1;
}

/*! Get (closest) elevation index for given elevation \p elev.
 * Elevation is clipped to legal range.
 */
int
get_el_idx(float elev)
{
  int el_idx;
  el_idx = round((elev - MIN_ELEV) / ELEV_INC);
  if (el_idx < 0)
    el_idx = 0;
  else if (el_idx >= N_ELEV)
    el_idx = N_ELEV - 1;
  return el_idx;
}

/*! For a given elevation \p elev and azimuth \p azim in degrees,
 * lookup the indices for the proper HRIR/HRTF into \p p_el and \p p_ax.
 *
 * \param p_flip *p_flip will be set to TRUE if left and right
 *               channels need to be flipped.
 */
void
a3d_lookup_indices(A3d * a3d,
                   float elev, float azim,
                   int * p_el, int * p_az, bool * p_flip)
{
  int naz, azimnum;
  int el_idx;
  int az_idx;

  el_idx = get_el_idx(elev);
  naz = g_azim_nums[el_idx];
  azimnum = get_azimnum(el_idx);

  /***
   * Coerce azimuth into legal range and calculate
   * flip if any.
   */
  azim = fmod(azim, 360.0);
  if (azim < 0)
    azim += 360;
  if (azim > 180) {
    azim = 360 - azim;
    *p_flip = TRUE;
  } else
    *p_flip = FALSE;

  /***
   * Now 0 <= azim <= 180. Calculate index and clip to
   * legal range just to be sure.
   */
  az_idx = round(azim / (360.0 / naz));
  if (az_idx < 0)
    az_idx = 0;
  else if (az_idx >= azimnum)
    az_idx = azimnum - 1;

  *p_el = el_idx;
  *p_az = az_idx;
}

/*! Change the internal indices of a3d to match [\p elev, \p azim] both given in
 * degrees.
 */
void
a3d_change_indices(A3d * a3d, float elev, float azim)
{
  /* Clip angles and convert to indices. */
  a3d_lookup_indices(a3d,
                     elev, azim,
                     &a3d->cur_el_idx,
                     &a3d->cur_az_idx,
                     &a3d->cur_flip_flag);

  if (a3d->cur_el_idx != a3d->last_el_idx ||
      a3d->cur_az_idx != a3d->last_az_idx ||
      a3d->cur_flip_flag != a3d->last_flip_flag) {
    a3d->changeflag = TRUE;
  }

  a3d->last_el_idx = a3d->cur_el_idx;
  a3d->last_az_idx = a3d->cur_az_idx;
  a3d->last_flip_flag = a3d->cur_flip_flag;
}

/*! Convert Elevation Index \p el_idx to Angles and return it. */
int
index_to_elev(int el_idx)
{
  return MIN_ELEV + el_idx * ELEV_INC;
}

/*! Convert Elevation Index \p el_idx and Azimuth Index \p ax_idx to Angles and
 *  return it. */
int
index_to_azim(int el_idx, int az_idx)
{
  return round(az_idx * 360.0 / g_azim_nums[el_idx]);
}

/*! Lookup HRTF-path at elevation index \p el_idx and azimuth index \p az_idx. */
char *
a3d_lookupHRTFpath(A3d * a3d, int el_idx, int az_idx)
{
  static char buf[128];
  int elev;
  int azim;

  elev = index_to_elev(el_idx);
  azim = index_to_azim(el_idx, az_idx);

  sprintf(buf, PATH_FMT, a3d->rootpath,
          elev, elev, azim, a3d->fileext);
  return buf;
}

/*! Compute Spectrum of short array of length \p n as needed by
 * convolution alg.
 *
 * Filter response is copied into \c A3D_FILTLEN size buffer,
 * PRE-padded with \c A3D_FILTLEN zeros, and transformed. The
 * resulting size of returned spectrum depends on which fft package
 * we're using.
 */
complexf *
transformHR(const short * sx, size_t n, float filtnorm)
{
  size_t i;
  float *x;

  /* convert to float */
  x = (float *) cfltv_calloc(A3D_FILTLEN + 1);
  for (i = 0; i < n; i++) {
    x[A3D_FILTLEN + i] = (float)sx[i] / filtnorm;
  }

  sfft1du(-1, A3D_BUFLEN, x, 1); /* compute spectrum */

  return (complexf *)x;
}

/*! Called with control numbers and values. */
void
A3D_do_ctl(A3d * a3d, int ctl_num, int ctl_val)
{
  if (ctl_num >= 0 && ctl_num < A3D_N_CTLS)
    a3d->ctls[ctl_num] = ctl_val;
}

/*! Get the closest HRIR to the specified elevation \p elev and azimuth
 * \p azim in degrees.
 *
 * Return via left and right channel pointers.
 */
void
a3d_lookupHRIR(A3d * a3d,
               float elev, float azim,
               short ** p_left, short ** p_right)
{
  HRIR_t *hd;

  a3d_change_indices(a3d, elev, azim);

  /* Get data and flip channels if necessary. */
  hd = &a3d->HRIRs[a3d->cur_el_idx][a3d->cur_az_idx];
  if (a3d->cur_flip_flag) {
    *p_left = hd->right;
    *p_right = hd->left;
  } else {
    *p_left = hd->left;
    *p_right = hd->right;
  }
}

/*!
 * Get the closest HRTF to the specified elevation \p elev and azimuth
 * \p azim in degrees.
 * Return via left and right channel pointers.
 */
void
a3d_lookupHRTF(A3d * a3d,
               float elev, float azim,
               complexf ** p_left, complexf ** p_right)
{
  HRTF_t *hd;

  a3d_change_indices(a3d, elev, azim);

  /* Get data and flip channels if necessary. */
  hd = &a3d->HRTFs[a3d->cur_el_idx][a3d->cur_az_idx];
  if (a3d->cur_flip_flag) {
    *p_left = hd->right;
    *p_right = hd->left;
  } else {
    *p_left = hd->left;
    *p_right = hd->right;
  }
}

/*! Call to get current left and right HRTFs based on control values. */
void
a3d_curHRTF(A3d * a3d,
            complexf ** p_left, complexf ** p_right, float *p_gain)
{
  float elev, azim;

  a3d->changeflag = FALSE;
  elev = MIN_ELEV + (MAX_ELEV - MIN_ELEV) * CTL_VAL(CTL_ELEV);
  azim = MIN_AZIM + (MAX_AZIM - MIN_AZIM) * CTL_VAL(CTL_AZIM);
  a3d_lookupHRTF(a3d, elev, azim, p_left, p_right);

  a3d->cur_atten = MIN_ATTEN + (MAX_ATTEN - MIN_ATTEN) * CTL_VAL(CTL_ATTEN);
  *p_gain = pow(10.0, -a3d->cur_atten / 20);
  if (a3d->cur_atten != a3d->last_atten)
    a3d->changeflag = TRUE;
  a3d->last_atten = a3d->cur_atten;

  if (a3d->changeflag) {
    printf("elev %d azim %d atten %.0lf\n",
           index_to_elev(a3d->cur_el_idx),
           a3d->cur_flip_flag ? -index_to_azim(a3d->cur_el_idx, a3d->cur_az_idx) :
           index_to_azim(a3d->cur_el_idx, a3d->cur_az_idx), a3d->cur_atten);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Convolution code for 3D-Audio Spatializer. */

/*! \name These are allocated buffers. */

/*! Function should be a macro) to do spectral product, inverse
 * transform, and interleave output samples.
 */
void
a3d_inv_transform(A3d * a3d,
                  const complexf * g_filt_L, const complexf * g_filt_R,
                  float gain, float * out)
{
  float *p0, *p1, *p2;
  size_t n;

  /* Spectral multiply and inverse transform. */
  c_opt_mul(a3d->fwd_buf, g_filt_L, a3d->inv_buf_L, A3D_BUFLEN);
  sfft1du(1, A3D_BUFLEN, (float *) a3d->inv_buf_L, 1);
  farray_scale((float*) a3d->inv_buf_L, gain / A3D_BUFLEN, A3D_FILTLEN);

  c_opt_mul(a3d->fwd_buf, g_filt_R, a3d->inv_buf_R, A3D_BUFLEN);
  sfft1du(1, A3D_BUFLEN, (float *) a3d->inv_buf_R, 1);
  farray_scale((float*) a3d->inv_buf_R, gain / A3D_BUFLEN, A3D_FILTLEN);

  /* Interleave left and right channels into output buffer. */
  p0 = (float *) a3d->inv_buf_L;
  p1 = (float *) a3d->inv_buf_R;
  p2 = out;
  n = A3D_FILTLEN;
  while (n-- > 0) {
    *p2++ = *p0++;
    *p2++ = *p1++;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*! \em Read \em HRIR and \em transform it the \em HRTF.
 *
 * Specified by the \em indices.
 *
 * Store into pre-allocated \p hrir and \p hrtf.  We always store into
 * 128-pt result, even if HRTF data is smaller.
 *
 * \return filter length, or -1 upon error.
 */
int
a3d_loadHR(A3d * a3d, int el_idx, int az_idx, HRIR_t * hrir, HRTF_t * hrtf)
{
  char *name;
  short *sbuf;
  int i, n, nr;

  name = a3d_lookupHRTFpath(a3d, el_idx, az_idx);

  /* allocate sample buffer and load sample-pairs from file into it */
  sbuf = load_spairs_S16be(name, &nr);
  if (! sbuf) {
    return -1;
  }

  n = nr / 2;			/* number of stereo samples */
  if (n > A3D_FILTLEN) {
    fprintf(stderr, "unexpected length of HRTF, %d sample frames\n", n);
    return -1;
  }

  /* allocate left and right HRIR buffers */
  hrir->left = shortarray_calloc(n);
  hrir->right = shortarray_calloc(n);

  /* split interleaved data from file into left and right HRIR buffer */
  s_split(sbuf, n, hrir->left, hrir->right);

  /* free sample buffer */
  free(sbuf);

  if (TRUE) {
    short min_L, max_L;
    short min_R, max_R;

    min_L = SHRT_MAX; max_L = SHRT_MIN;
    min_R = SHRT_MAX; max_R = SHRT_MIN;

    /* update filter extremes */
    for (i = 0; i < n; i++) {
      min_L = MIN2(min_L, hrir->left[i]);
      max_L = MAX2(max_L, hrir->left[i]);
      min_R = MIN2(min_R, hrir->right[i]);
      max_R = MAX2(max_R, hrir->right[i]);
    }

    a3d->IRsmin_L = MIN2(a3d->IRsmin_L, min_L);
    a3d->IRsmax_L = MAX2(a3d->IRsmax_L, max_L);
    a3d->IRsmin_R = MIN2(a3d->IRsmin_R, min_R);
    a3d->IRsmax_R = MAX2(a3d->IRsmax_R, max_R);
  }

  /* transform HRIRs into HRTFs */
  hrtf->left = transformHR(hrir->left, n, a3d->filtnorm);
  hrtf->right = transformHR(hrir->right, n, a3d->filtnorm);

  return n;
}

/*! Read HRIRs and store them, make copies of them and transform them to HRTFs
 * and store them aswell.
 *
 * \return >= 0 if all HRIR/HRTFs could be loaded, -1 otherwise.
 */
int
a3d_loadHRs(A3d * a3d)
{
  int el_idx;
  int az_idx;
  int azimnum;
  int show = TRUE;

  if (show) {
    lprintf("Loading HRTFs from %s\n", a3d->rootpath);
  }

  a3d->HRIRs = (HRIR_t **) calloc(N_ELEV * sizeof(HRIR_t *), 1);
  a3d->HRTFs = (HRTF_t **) calloc(N_ELEV * sizeof(HRTF_t *), 1);

  for (el_idx = 0; el_idx < N_ELEV; el_idx++) {
    if (show) {
      lprintf("elevation:%3d azimuths:", index_to_elev(el_idx));
    }

    azimnum = get_azimnum(el_idx);

    a3d->HRIRs[el_idx] = (HRIR_t *) calloc(azimnum * sizeof(HRIR_t), 1);
    a3d->HRTFs[el_idx] = (HRTF_t *) calloc(azimnum * sizeof(HRTF_t), 1);

    for (az_idx = 0; az_idx < azimnum; az_idx++) {
      int rval;

      if (show) {
        printf(" %3d", index_to_azim(el_idx, az_idx));
      }

      rval = a3d_loadHR(a3d, el_idx, az_idx,
                        &a3d->HRIRs[el_idx][az_idx],
                        &a3d->HRTFs[el_idx][az_idx]);
      if (rval >= 0) {		/* if load was successful */
        if (a3d->filtlen > 0) {
          if (a3d->filtlen != (size_t)rval) {
            leprintf("filter lengths are not of same size\n");
          }
        } else {
          a3d->filtlen = rval;
        }
      } else {
        for (int i = 0; i < el_idx + 1; i++) {
          free(a3d->HRIRs[el_idx]);
          free(a3d->HRTFs[el_idx]);
        }
        free(a3d->HRIRs);
        free(a3d->HRTFs);
        return -1;
      }
    }
    if (show) {
      printf("\n");
    }
  }

  lprintf("HRIR global range L: [%d %d]\n", a3d->IRsmin_L, a3d->IRsmax_L);
  lprintf("HRIR global range R: [%d %d]\n", a3d->IRsmin_R, a3d->IRsmax_R);

  return 0;
}

/*! Initialization function. */
int
a3d_init(A3d * a3d, const char * rootpath, const char * fileext, int srate)
{
  size_t i;

  const char * envroot = getenv(ROOT_ENV);
  if (envroot) {
    a3d->rootpath = strdup(envroot);
  } else {
    if (rootpath) {
      a3d->rootpath = strdup(rootpath);
    } else {
      a3d->rootpath = NULL;
    }
  }

  if (fileext) {
    a3d->fileext = strdup(fileext);
  } else {
    a3d->fileext = strdup("");
  }

  a3d->HRIRs = NULL;
  a3d->HRTFs = NULL;

  a3d->filtlen = 0;		/* indicate not initialized */
  a3d->filtnorm = 32768;
  a3d->invfiltnorm = 1.0f / a3d->filtnorm;

  if (a3d_loadHRs(a3d) < 0) {
    /* free strings */
    if (a3d->rootpath) { free(a3d->rootpath); }
    if (a3d->fileext) { free(a3d->fileext); }
    return -1;
  }

  /* initialize filter extremes */
  a3d->IRsmin_L = SHRT_MAX;
  a3d->IRsmax_L = SHRT_MIN;
  a3d->IRsmin_R = SHRT_MAX;
  a3d->IRsmax_R = SHRT_MIN;

  a3d->srate = srate;		/* set sample rate */

  /* ===================0 */

  /* allocate buffers */
  a3d->in0_LR = farray_calloc(A3D_FILTLEN * 2);
  a3d->in1_LR = farray_calloc(A3D_FILTLEN * 2);
  a3d->p_newin_LR = a3d->in0_LR;
  a3d->p_oldin_LR = a3d->in1_LR;
  a3d->out0_LR = farray_calloc(A3D_FILTLEN * 2);
  a3d->out1_LR = farray_calloc(A3D_FILTLEN * 2);
  a3d->fwd_buf = cfltv_calloc(A3D_BUFLEN);
  a3d->inv_buf_L = cfltv_calloc(A3D_BUFLEN);
  a3d->inv_buf_R = cfltv_calloc(A3D_BUFLEN);

  /* lookup current HRTF */
  a3d_curHRTF(a3d, &a3d->filt_L, &a3d->filt_R, &a3d->gain);

  /* Linear ramp table and FFT initialization. */
  a3d->ramp_up = farray_calloc(A3D_FILTLEN);
  a3d->ramp_down = farray_calloc(A3D_FILTLEN);
  for (i = 0; i < A3D_FILTLEN; i++) {
    a3d->ramp_up[i] = i / (float) A3D_FILTLEN;
    a3d->ramp_down[i] = 1.0 - a3d->ramp_up[i];
  }

  /* set initial elevation */
  A3D_do_ctl(a3d, CTL_ELEV, 40);

  return 0;
}

void
a3d_freeHRIRs(A3d * a3d)
{
  int el_idx, az_idx;
  int azimnum;
  if (a3d->HRIRs) {
    for (el_idx = 0; el_idx < N_ELEV; el_idx++) {
      azimnum = get_azimnum(el_idx);
      for (az_idx = 0; az_idx < azimnum; az_idx++) {
        free(a3d->HRIRs[el_idx][az_idx].left);
        free(a3d->HRIRs[el_idx][az_idx].right);
      }
      free(a3d->HRIRs[el_idx]);
    }
    free(a3d->HRIRs);
  }
}

void
a3d_freeHRTFs(A3d * a3d)
{
  int el_idx, az_idx;
  int azimnum;
  if (a3d->HRTFs) {
    for (el_idx = 0; el_idx < N_ELEV; el_idx++) {
      azimnum = get_azimnum(el_idx);
      for (az_idx = 0; az_idx < azimnum; az_idx++) {
        free(a3d->HRTFs[el_idx][az_idx].left);
        free(a3d->HRTFs[el_idx][az_idx].right);
      }
      free(a3d->HRTFs[el_idx]);
    }
    free(a3d->HRTFs);
  }
}

void
a3d_clear(A3d * a3d)
{
  free(a3d->rootpath);
  free(a3d->fileext);

  a3d_freeHRIRs(a3d);
  a3d_freeHRTFs(a3d);

  /* free buffers */
  free(a3d->in0_LR);
  free(a3d->in1_LR);
  free(a3d->out0_LR);
  free(a3d->out1_LR);
  free(a3d->fwd_buf);
  free(a3d->inv_buf_L);
  free(a3d->inv_buf_R);
  free(a3d->ramp_up);
  free(a3d->ramp_down);
}

void
a3d_filter(A3d * a3d,
           float elev, float azim,
           float * ins, size_t inslen,
           float * outL, float * outR)
{
  size_t n;

  if (a3d->filtlen > 0) {
    /* lookup filters */
    short * fL, * fR;		/* left and right filter */
    a3d_lookupHRIR(a3d, elev, azim, &fL, &fR);

    /* apply HRIR-filter on in signal (history) */
    *outL = 0;
    *outR = 0;
    n = MIN2(inslen, a3d->filtlen);
    int use_dot_opt = FALSE;

    if (use_dot_opt &&
        n == A3D_FILTLEN) {
      float aligned_ins[A3D_FILTLEN] __attribute__((aligned(16)));
      float aligned_filt[A3D_FILTLEN] __attribute__((aligned(16)));

      memcpy(aligned_ins, ins, A3D_FILTLEN * sizeof(float));

      for (int i = 0; i < A3D_FILTLEN; i++) { aligned_filt[i] = (float)fL[i]; }
      *outL += farray_dot_8stretched_anyaligned(aligned_ins, aligned_filt,
                                                A3D_FILTLEN);

      for (int i = 0; i < A3D_FILTLEN; i++) { aligned_filt[i] = (float)fR[i]; }
      *outR += farray_dot_8stretched_anyaligned(aligned_ins, aligned_filt,
                                                A3D_FILTLEN);
    } else {
      *outL += farray_dot_shv_8stretched(ins, fL, n);
      *outR += farray_dot_shv_8stretched(ins, fR, n);
    }

    *outL *= a3d->invfiltnorm;
    *outR *= a3d->invfiltnorm;
  } else {
    PERR("Filter length is zero.\n");
  }
}

/*! This is currently not used. */
float *
a3d_do_buf(A3d * a3d, float **p_ibuf, int flag)
{
  float *p0, *p1, *p2;
  float *r0, *r1;
  int n;

  if (flag) {
    a3d->oldfilt_L = a3d->filt_L;
    a3d->oldfilt_R = a3d->filt_R;
    a3d->old_gain = a3d->gain;
    a3d_curHRTF(a3d, &a3d->filt_L, &a3d->filt_R, &a3d->gain);
  }

  /***
   * Read left channel input samples into a3d->fwd_buf. Left input
   * samples must be de-interleaved from stereo input. First
   * read old input, then new input.
   */
  p0 = (float *) a3d->fwd_buf;
  p1 = a3d->p_oldin_LR;
  n = A3D_FILTLEN;
  while (n-- > 0) {
    *p0++ = *p1;
    p1 += 2;
  }
  p1 = a3d->p_newin_LR;
  n = A3D_FILTLEN;
  while (n-- > 0) {
    *p0++ = *p1;
    p1 += 2;
  }

  /* Do forward FFT (real packed data). */
  sfft1du(-1, A3D_BUFLEN, (float *) a3d->fwd_buf, 1);

  /* Spectral multiply, inverse transform, and interleave output samples. */
  a3d_inv_transform(a3d, a3d->filt_L, a3d->filt_R, a3d->gain, a3d->out0_LR);

  /*! If crossfading, calculate convolution with old filter and crossfade two
   *  results.   */
  if (flag) {
    a3d_inv_transform(a3d, a3d->oldfilt_L, a3d->oldfilt_R, a3d->old_gain, a3d->out1_LR);

    /** Crossfade using pointers. */
    r0 = a3d->ramp_up;
    r1 = a3d->ramp_down;
    p0 = a3d->out0_LR;
    p1 = a3d->out1_LR;
    p2 = a3d->out0_LR;
    n = A3D_FILTLEN;
    while (n-- > 0) {
      *p2++ = *p0++ * *r0 + *p1++ * *r1;
      *p2++ = *p0++ * *r0++ + *p1++ * *r1++;
    }
  }

  /* Swap input buffer pointers. */
  p0 = a3d->p_newin_LR;
  a3d->p_newin_LR = a3d->p_oldin_LR;
  a3d->p_oldin_LR = p0;

  /* Change caller's pointer to input buffer. */
  *p_ibuf = a3d->p_newin_LR;

  /* Return pointer to output buffer. */
  return a3d->out0_LR;
}
