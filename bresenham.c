#include "bresenham.h"
#include "utils.h"

void
bresenham(int x1, int y1, int x2, int y2, int (*plot) (int, int))
{
  int dx = x2 - x1;
  int ax = ABS(dx) << 1;
  int sx = SGN(dx);
  int dy = y2 - y1;
  int ay = ABS(dy) << 1;
  int sy = SGN(dy);
  int x = x1;
  int y = y1;

  if (ax > ay) {		/* x is dominant. */
    int d = ay - (ax >> 1);
    for (;;) {
      (*plot) (x, y);
      if (x == x2)
	return;
      if (d >= 0) {
	y += sy;
	d -= ax;
      }
      x += sx;
      d += ay;
    }
  } else {			/* y is dominant. */

    int d = ax - (ay >> 1);
    for (;;) {
      (*plot) (x, y);
      if (y == y2)
	return;
      if (d >= 0) {
	x += sx;
	d -= ay;
      }
      y += sy;
      d += ax;
    }
  }
}
