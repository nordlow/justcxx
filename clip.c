#include "vec2f.h"
#include "clip.h"
#include "utils.h"

static int
f_liang_barsky_clip_test(float p, float q, float *u1, float *u2)
{
  float r;
  int ret = TRUE;

  if (p < 0.0) {
    r = q / p;
    if (r > *u2) {
      ret = FALSE;
    } else {
      if (r > *u1) {
	*u1 = r;
      }
    }
  } else if (p > 0.0) {
    r = q / p;
    if (r < *u1) {
      ret = FALSE;
    } else if (r < *u2) {
      *u2 = r;
    }
  } else {			/* p = 0, so line is parallel to this clipping edge */

    if (q < 0.0) {
      /* line is outside clipping edge */
      ret = FALSE;
    }
  }

  return ret;
}

int
fvec2_liang_barsky_clip_2d(vec2f min, vec2f max,
			   vec2f p1, vec2f p2,
			   vec2f * rp1, vec2f * rp2)
{
  float u1 = 0.0, u2 = 1.0, dx = p2.x - p1.x, dy;
  if (f_liang_barsky_clip_test(-dx, p1.x - min.x, &u1, &u2)) {
    if (f_liang_barsky_clip_test(dx, max.x - p1.x, &u1, &u2)) {
      dy = p2.y - p1.y;
      if (f_liang_barsky_clip_test(-dy, p1.y - min.y, &u1, &u2)) {
	if (f_liang_barsky_clip_test(dy, max.y - p1.y, &u1, &u2)) {
	  if (u2 < 1.0) {
	    p2.x = p1.x + u2 * dx;
	    p2.y = p1.y + u2 * dy;
	  }
	  if (u1 > 0.0) {
	    p1.x += u1 * dx;
	    p1.y += u1 * dy;
	  }
	  *rp1 = p1;
	  *rp2 = p2;
	  return TRUE;
	}
      }
    }
  }
  return FALSE;
}
