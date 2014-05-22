#include "atrans.h"
#include "extremes.h"
#include "math_x.h"
#include "stdio_x.h"

#include <math.h>

void
ATTabf_init(ATTabf * attab, ATRANS_t att, size_t sampN)
{
  size_t i;

  attab->att = att;
  attab->sampN = sampN;
  attab->samps = (float*)malloc(sampN * sizeof(float));

  /* set samples */
  for (i = 0; i < sampN; i++) {
    float x = (float)(i + 1) / sampN;	/* + 1 => we reach destination (1.0) */
    float y, pi = M_PI, pi2 = M_2PI, hpi = M_PI_2;
    switch (attab->att) {
    case ATRANS_LINEAR:
      y = x;
      break;
    case ATRANS_COSSTEP:
      y = (1 - cosf(x * pi)) / 2;
      break;
    case ATRANS_COSBUMP:
      y = (1 - cosf(x * pi2)) / 2;
      break;
    case ATRANS_SINSHAKE:
      y = (1 - cosf(x * pi2)) / 2 * sinf(x * 8 * pi);
      break;
    case ATRANS_RIPPEL: {
      /***
       * Use the following code to plot this transition in Octave.
       *
       * n = 1024;
       * x = [0:n-1]/n;
       * r = 0.01;
       * y = sqrt(sin(x * hpi)) - sin((1+r)./(1+r-x) * pi2) .* (1-x).^2;
       * plot(x, y)
       */
      float r = 0.01;
      y = sqrtf(sinf(x * hpi)) - sin((1+r)/(1+r-x) * pi2) * (1-x) * (1-x);
      break;
    }
    default:
      leprintf("unknown atrans type %d. defaulting to linear.\n", att);
      y = x;
      break;	// default to linear
    }
    attab->samps[i] = y;
  }
}

float
ATTabf_lookup(ATTabf * attab, size_t idx)
{
#ifdef DEBUG_CHECK_RANGE
  if (idx >= attab->sampN) { PWARN("idx >= sampN\n"); }
#endif
  if (attab->sampN > 0) {
    idx = MIN2(idx, attab->sampN-1);
    return attab->samps[idx];
  } else {
    return 0;			/* default to zero */
  }
}

void
ATTabf_clear(ATTabf * attab)
{
  attab->sampN = 0;
  free(attab->samps);
  attab->samps = NULL;
}
