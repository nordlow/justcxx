#include "alz3d.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "encode.h"
#include "decode.h"

#include "timing.h"
#include "meman.h"
#include "clamp.h"

void
alz3d_init(Alz3d * alz,
	   uint schan,
	   uint srate, uint sprecision,
	   bool sign_flag,
	   bool little_endian_flag,
	   uint use_a3dflag,
	   const char * sfile_path)
{
  alz->models = NULL;

  alz->signum = 0;

  uint schanmin = 1, schanmax = 2;
  if (schan < schanmin || schan > schanmax) {
    alz->schan = int_clamp(schanmin, schan, schanmax);
    leprintf("%d number of channels not supported. changed to %d\n",
	     schan, alz->schan);
  } else {
    alz->schan = schan;
  }

  alz->srate += srate;
  alz->sprecision = sprecision;
  alz->little_endian_flag = little_endian_flag;

  switch (alz->sprecision) {
  case 8: alz->maxamp = INT8_MAX; break;
  case 16: alz->maxamp = INT16_MAX; break;
  case 32: alz->maxamp = INT32_MAX; break;
  default: alz->maxamp = INT8_MAX; break;
  }

  alz->swbuf = NULL;
  alz->swlen = 0;
  alz->sgcnt = 0;

  /* use sfile only if we have specified a path */
  if (sfile_path) {
    alz->sfile = fopen(sfile_path, "wb");
  } else {
    alz->sfile = NULL;
  }

  ptime_read_CLOCK_REALTIME(&alz->tref);

  if (use_a3dflag) {
    alz->a3dflag = (a3d_init(&alz->a3d,
			     "/home/per/KEMAR/compact", ".dat", srate) >= 0);
    if (! alz->a3dflag) {
      leprintf("Could not load KEMAR-data. 3D-audio will not be used\n");
    }
  } else {
    alz->a3dflag = 0;
  }
}

int
alz3d_clear(Alz3d * alz)
{
  if (alz->models) { free(alz->models); alz->models = NULL; }

  alz->signum = 0;

  if (alz->swbuf) { free(alz->swbuf); alz->swbuf = NULL; }
  alz->swlen = 0;

  alz->sgcnt = 0;

  if (alz->sfile) {
    fclose(alz->sfile);
    alz->sfile = NULL;
  }

  if (alz->a3dflag) {
    a3d_clear(&alz->a3d);
  }

  return 0;
}

int
alz3d_addNow(Alz3d * alz,
	     float wfq, float dur_sec, float azim, float amp,
	     ALZ3D_WAVE_t wave, ALZ3D_FADE_t fade)
{
  uint i = alz->signum;

  alz->models = realloc(alz->models, (i + 1) * sizeof(Alz3dModel));

  alz->models[i].wfq = wfq;
  alz->models[i].scnt = 0;
  ptime_read_CLOCK_REALTIME(&alz->models[i].toff);
  alz->models[i].tdur = dur_sec;
  alz->models[i].inv_tdur = 1 / alz->models[i].tdur;

  alz->models[i].amp = amp * alz->maxamp;

  alz->models[i].azim = azim;
  alz->models[i].azim_cos = cosf(float_azim2rad(alz->models[i].azim));
  alz->models[i].azim_ampL = alz->models[i].amp * (1 - alz->models[i].azim_cos) / 2.0f;
  alz->models[i].azim_ampR = alz->models[i].amp * (1 + alz->models[i].azim_cos) / 2.0f;
  const float sos = 340.29;	/* Speed of sound at sea level [m/s] */
  const float esr = 0.075f;/* Ear Separation Radius. Half the ear-distance [m] */
  float etr = esr / sos;	/* Ear Separation Time. */
  alz->models[i].azim_dtd = etr * cosf(float_azim2rad(alz->models[i].azim));

  alz->models[i].wave = wave;
  alz->models[i].fade = fade;

  /* if 3D-audio allocate sample history */
  if (alz->a3dflag) {
    alz->models[i].hist = farray_calloc(alz->a3d.filtlen);
  } else {
    alz->models[i].hist = NULL;
  }

  alz->models[i].obsflag = 0;	/* not obselete (yet) */

  alz->signum++;

  /* lprintf("Added wfq:%f fade_usec:%d amp:%f\n", wfq, fade_usec, amp); */
  return 1;
}

/*!
 * Calculate the size (in bytes) of a sample times number of channels.
 */
size_t
alz3d_getSSize(Alz3d * alz)
{
  size_t ssz = 0;		/* sample size in bytes */
  switch (alz->sprecision) {
  case 8: ssz = 1; break;
  case 16: ssz = 2; break;
  case 32: ssz = 4; break;
  default: ssz = 1; break;
  }
  ssz *= alz->schan;		/* times number of channels */
  return ssz;
}

/*!
 * Set sample [\p sl, \p sr] at index \p wi in \p alz's sample buffer.
 */
void
alz3d_setSample(Alz3d * alz, size_t wi, float sl, float sr)
{
  float sm = (sl + sr) / 2.0f;	/* sample in middle */
  switch (alz->schan) {
  case 1:			/* one channel (MONO) */
    switch (alz->sprecision) {
    case 8:
      if (alz->sign_flag) {
	uint8_t smx = sm;
	benc_u8((char*)&((int8_t*)alz->swbuf)[wi], (uint8_t*)&smx);
      } else {
	uint8_t smx = sm;
	benc_u8((char*)&((uint8_t*)alz->swbuf)[wi], (uint8_t*)&smx);
      }
      break;
    case 16:
      if (alz->sign_flag) {
	if (alz->little_endian_flag) {
	  int16_t smx = sm;
	  benc_s16le((char*)&((int16_t*)alz->swbuf)[wi], &smx);
	} else {
	  int16_t smx = sm;
	  benc_s16be((char*)&((int16_t*)alz->swbuf)[wi], &smx);
	}
      } else {
	if (alz->little_endian_flag) {
	  uint16_t smx = sm;
	  benc_u16le((char*)&((uint16_t*)alz->swbuf)[wi], &smx);
	} else {
	  uint16_t smx = sm;
	  benc_u16be((char*)&((uint16_t*)alz->swbuf)[wi], &smx);
	}
      }
      break;
    case 32:
      if (alz->sign_flag) {
	if (alz->little_endian_flag) {
	  int32_t smx = sm;
	  benc_s32le((char*)&((int32_t*)alz->swbuf)[wi], &smx);
	} else {
	  int32_t smx = sm;
	  benc_s32be((char*)&((int32_t*)alz->swbuf)[wi], &smx);
	}
      } else {
	if (alz->little_endian_flag) {
	  uint32_t smx = sm;
	  benc_u32le((char*)&((uint32_t*)alz->swbuf)[wi], &smx);
	} else {
	  uint32_t smx = sm;
	  benc_u32be((char*)&((uint32_t*)alz->swbuf)[wi], &smx);
	}
      }
      break;
    default:
      leprintf("cannot handle precision %d\n", alz->sprecision);
      break;
    }
    break;
  case 2:			/* two channels (STEREO) */
    switch (alz->sprecision) {
    case 8:
      if (alz->sign_flag) {
	const int8_t slx = sl, srx = sr;
	benc_s8((char*)&((int8_t*)alz->swbuf)[2*wi+0], &slx);
	benc_s8((char*)&((int8_t*)alz->swbuf)[2*wi+1], &srx);
      } else {
	const uint8_t slx = sl, srx = sr;
	benc_u8((char*)&((uint8_t*)alz->swbuf)[2*wi+0], &slx);
	benc_u8((char*)&((uint8_t*)alz->swbuf)[2*wi+1], &srx);
      }
      break;
    case 16:
      if (alz->sign_flag) {
	const int16_t slx = sl, srx = sr;
	if (alz->little_endian_flag) {
	  benc_s16le((char*)&((int16_t*)alz->swbuf)[2*wi+0], &slx);
	  benc_s16le((char*)&((int16_t*)alz->swbuf)[2*wi+1], &srx);
	} else {
	  benc_s16be((char*)&((int16_t*)alz->swbuf)[2*wi+0], &slx);
	  benc_s16be((char*)&((int16_t*)alz->swbuf)[2*wi+1], &srx);
	}
      } else {
	const uint16_t slx = sl, srx = sr;
	if (alz->little_endian_flag) {
	  benc_u16le((char*)&((uint16_t*)alz->swbuf)[2*wi+0], &slx);
	  benc_u16le((char*)&((uint16_t*)alz->swbuf)[2*wi+1], &srx);
	} else {
	  benc_u16be((char*)&((uint16_t*)alz->swbuf)[2*wi+0], &slx);
	  benc_u16be((char*)&((uint16_t*)alz->swbuf)[2*wi+1], &srx);
	}
      }
      break;
    case 32:
      if (alz->sign_flag) {
	const int32_t slx = sl, srx = sr;
	if (alz->little_endian_flag) {
	  benc_s32le((char*)&((int32_t*)alz->swbuf)[2*wi+0], &slx);
	  benc_s32le((char*)&((int32_t*)alz->swbuf)[2*wi+1], &srx);
	} else {
	  benc_s32be((char*)&((int32_t*)alz->swbuf)[2*wi+0], &slx);
	  benc_s32be((char*)&((int32_t*)alz->swbuf)[2*wi+1], &srx);
	}
      } else {
	const uint32_t slx = sl, srx = sr;
	if (alz->little_endian_flag) {
	  benc_u32le((char*)&((uint32_t*)alz->swbuf)[2*wi+0], &slx);
	  benc_u32le((char*)&((uint32_t*)alz->swbuf)[2*wi+1], &srx);
	} else {
	  benc_u32be((char*)&((uint32_t*)alz->swbuf)[2*wi+0], &slx);
	  benc_u32be((char*)&((uint32_t*)alz->swbuf)[2*wi+1], &srx);
	}
      }
      break;
    default:
      leprintf("cannot handle precision %d\n", alz->sprecision);
      break;
    }
    break;
  default:
    leprintf("cannot handle %d channels\n", alz->schan);
    break;
  }
}

float
alz3d_getFade(Alz3d * alz, float tcurr, size_t si)
{
  float ff;
  const float tdur = alz->models[si].tdur; /* duration */
  if (tcurr < tdur) {	/* if signal is in the "air" */
    const float inv_tdur = alz->models[si].inv_tdur; /* inverse duration */
    ALZ3D_FADE_t fwin = ALZ3D_FADEWIN_QUADRATIC;
    switch (alz->models[si].fade) {
    case ALZ3D_FADE_NONE: ff = 1; break;
    case ALZ3D_FADE_IN: ff = tcurr * inv_tdur; break; break;
    case ALZ3D_FADE_OUT: ff = (tdur - tcurr) * inv_tdur; break;
    case ALZ3D_FADE_INOUT:
      if (tcurr < 0.5 * tdur) {
	ff = 2 * tcurr * inv_tdur; /* fade in part */
      } else {
	ff = 2 * (tdur - tcurr) * inv_tdur; /* fade out part */
      }
      break;
    case ALZ3D_FADE_OUTIN:
      if (tcurr < 0.5 * tdur) {
	ff = (tdur - 2 * tcurr) * inv_tdur; /* fade out part */
      } else {
	ff = (2 * tcurr - tdur) * inv_tdur; /* fade in part */
      }
      break;
    default: ff = 1; break;
    }
    switch (fwin) {
    case ALZ3D_FADEWIN_LINEAR: break;
    case ALZ3D_FADEWIN_QUADRATIC: ff = ff * ff; break;
    default: break;
    }
  } else {
    ff = 0;
  }
  return ff;
}

float
alz3d_getWave(Alz3d * alz, float t, size_t si)
{
  float samp;
  const float wfq = alz->models[si].wfq; /* wave frequency [Hz] */
  switch (alz->models[si].wave) {
  case ALZ3D_WAVE_SINE: samp = sinf(M_2PI * wfq * t); break;
  default: samp = 0; break;
  }
  return samp;
}

void
alz3d_fill(Alz3d * alz, size_t snum)
{
  size_t si;		/* signal index */
  size_t wi;		/* window index */
  size_t pi;		/* packing index */

  const size_t ssz = alz3d_getSSize(alz);

  if (snum <= alz->swlen) { return; } /* buffer already filled */

  alz->swbuf = dynalloc(alz->swbuf, ssz * snum);

  const float tstep = 1.0f / alz->srate; /* sampling timestep [seconds] */

  const size_t filtlen = alz->a3d.filtlen;
  const float filtdur = tstep * filtlen; /* duration of filter [seconds] */

  for (wi = alz->swlen; wi < snum; wi++) { /* for each sample window index */
    const size_t ri = wi - alz->swlen; /* relative window index */
    /* float wtoff = tstep * ri; */	/* window time offset [seconds] */

    float sumL = 0, sumR = 0; /* left and right channel audio sample sum */
    for (si = 0; si < alz->signum; si++) { /* for each signal */
      /* current window-relative time [seconds] */
      const float tcurr = tstep * (alz->models[si].scnt + ri);
      const float tdur = alz->models[si].tdur; /* duration */
      const float tdur_listener = tdur + filtdur;
      const float ff = alz3d_getFade(alz, tcurr, si); /* fade factor */
      const float amp = alz->models[si].amp; /* audio amplitud */

      /* sample signal */
      float yL, yR;
      if (alz->a3dflag) {	/* 3d-audio generation */
	if (tcurr < tdur_listener) {
	  float * const hist = alz->models[si].hist; /* sample history */

	  /* advance sample histories by making previous samples one
	   * sample-time "older" */
	  memmove(hist + 1, hist, (filtlen - 1) * sizeof(float));

	  float y;
	  if (tcurr < tdur) {
	    y = amp * ff * alz3d_getWave(alz, tcurr, si); /* sample signal  */
	  } else {
	    y = 0.0f;		/* signal har left "air" at source */
	  }
	  hist[0] = y; /* append recent sample or zero it */

	  /* 3d-audio filtering using signal source elevation and azimuth */
	  const float elev = 0, azim = alz->models[si].azim;
	  a3d_filter(&alz->a3d, elev, azim,
		     hist, filtlen,
		     &yL, &yR);

	  /* add to mixed signal */
	  sumL += yL;
	  sumR += yR;
	} else {
	  alz->models[si].obsflag = 1;
	}
      } else {			/* simple stereo generation */
	if (tcurr < tdur) {
	  /* lookup/calculate stereo amplitudes and phases */
	  float ampL = alz->models[si].azim_ampL;
	  float ampR = alz->models[si].azim_ampR;
	  float phsL = + alz->models[si].azim_dtd; /* left phase */
	  float phsR = - alz->models[si].azim_dtd; /* right phase */

	  /* generate signal stereo samples */
	  yL = ampL * ff * alz3d_getWave(alz, tcurr + phsL, si);
	  yR = ampR * ff * alz3d_getWave(alz, tcurr + phsR, si);

	  /* add to mixed signal */
	  sumL += yL;
	  sumR += yR;
	} else {
	  alz->models[si].obsflag = 1;
	}
      }
    }

    /* make sample fit to audio device sampling precision */
    sumL = float_clamp(- alz->maxamp, sumL, + alz->maxamp);
    sumR = float_clamp(- alz->maxamp, sumR, + alz->maxamp);

    alz3d_setSample(alz, wi, sumL, sumR);
  }

  /* printf("\n"); */

  /* remove signals that have left "air */
  for (pi = si = 0; si < alz->signum; si++) { /* for each signal */
    alz->models[si].scnt += (snum - alz->swlen);
    if (alz->models[si].obsflag) { /* if signal is obselete */
      if (alz->models[si].hist) { /* if signal has history allocated */
	free(alz->models[si].hist); /* free it */
	alz->models[si].hist = 0;
      }
/*       lprintf("model %d wfq:%f[Hz] deactived\n", si, alz->models[si].wfq); */
    } else {
      if (pi < si) {
	/* pack signals still in the "air" */
	alz->models[pi] = alz->models[si];
      }
      pi++;
    }
  }

  alz->models = dynalloc(alz->models, pi * sizeof(Alz3dModel));

  alz->signum = pi;
  /* lprintf("signum NOW %d\n", alz->signum); */

  alz->swlen = snum;
}

int
alz3d_write(Alz3d * alz, int fd, size_t snum)
{
  size_t ssz = alz3d_getSSize(alz);

  alz3d_fill(alz, snum);		/* calculate snum samples ahead */

  ssize_t rval = write(fd, alz->swbuf, ssz * snum);

  if (alz->sfile) {
    /* fwrite(alz->swbuf, 1, rval, alz->sfile); */
  }

  if (rval < 0) {
    lperror("write() return < 0");
    return -1;
  } else if (rval > 0) {
    size_t wsz = rval;		/* number of bytes written */
    size_t sadv = wsz / ssz;	/* number of samples written (and advanced) */

    if (sadv * ssz != wsz) {
      leprintf("write() could not complete a whole sample! MUST BE FIXED\n");
    }

    int show_flag = 0;
    if (show_flag) {
      if (sadv != snum) {
	lprintf("WROTE only %zd of snum:%zd\n", sadv, snum);
      } else {
	lprintf("WROTE ALL of swlen:%zd\n", snum);
      }
    }

    /* advanced sample buffer */
    memmove(alz->swbuf + ssz * sadv, alz->swbuf, ssz * (alz->swlen - sadv));
    alz->swlen -= sadv;
    alz->swbuf = realloc(alz->swbuf, ssz * alz->swlen);
    alz->sgcnt += sadv;

    return sadv;
  } else {
    lperror("write() returned 0");
    return 0;
  }
}

void
alz3d_print(Alz3d * alz)
{
  printf("signum=%d: ", alz->signum);
  for (uint i = 0; i < alz->signum; i++) {
    printf("%f ", alz->models[i].wfq);
  }
  printf("\n");
}
