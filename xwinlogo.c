#include "xwinlogo.h"

static float
compute_inverse_slope(line2f * l)
{
  return ((l->q.x - l->p.x) / (l->q.y - l->p.y));
}

static float
compute_x_intercept(line2f * l, float inverse_slope)
{
  return (l->p.x) - inverse_slope * l->p.y;
}

static void
xlogo_intersect(line2f * l1, line2f * l2, vec2f * intersection)
{
  float check;
  /*
   * x = m1y + b1
   * x = m2y + b2
   * m1y + b1 = m2y + b2
   * y * (m1 - m2) = b2 - b1
   * y = (b2 - b1) / (m1 - m2)
   */
  float m1 = compute_inverse_slope(l1);
  float b1 = compute_x_intercept(l1, m1);
  float m2 = compute_inverse_slope(l2);
  float b2 = compute_x_intercept(l2, m2);

  intersection->y = (b2 - b1) / (m1 - m2);
  intersection->x = m1 * intersection->y + b1;

  check = m2 * intersection->y + b2;

  if (check >= intersection->x) {
    check -= intersection->x;
  } else {
    check = intersection->x - check;
  }

  if (check > (1 / (float) (1 << 16))) {
    fprintf(stderr, "intersect: intersection is off by %f\n", check);
  }
}

void
gen_xlogo(float x, float y, float width, float height,
	  vec2f poly_left[6], vec2f poly_right[6])
{
  float side;
  float thin, thick, gap, d31;
  line2f thick_left, thick_right, thin_left, thin_right, gap_left, gap_right;

  /* for now, do a centered even-sized square, at least for now */
  side = width;
  if (height < width) {
    side = height;
  }

  x += (width - side) / 2;
  y += (height - side) / 2;

  thin = (side / 11.0);	       /* width of the thin line */
  thick = (side / 4.0);	       /* width of the thick line */
  gap = thin / 4.0;	       /* the gap between the lines */
  d31 = thin + thin + gap;

  thick_left.p.x = x;
  thick_left.p.y = y;
  thick_left.q.x = x + side - thick;
  thick_left.q.y = y + side;

  thick_right.p.x = x + thick;
  thick_right.p.y = y;
  thick_right.q.x = x + side;
  thick_right.q.y = y + side;

  thin_left.p.x = x + side - d31;
  thin_left.p.y = y;
  thin_left.q.x = x + 0;
  thin_left.q.y = y + side;

  thin_right.p.x = x + side;
  thin_right.p.y = y;
  thin_right.q.x = x + d31;
  thin_right.q.y = y + side;

  gap_left.p.x = x + side - (thin + gap);
  gap_left.p.y = y;
  gap_left.q.x = x + thin;
  gap_left.q.y = y + side;

  gap_right.p.x = x + side - thin;
  gap_right.p.y = y;
  gap_right.q.x = x + thin + gap;
  gap_right.q.y = y + side;

  // left polygon
  poly_left[0] = thick_left.p;
  poly_left[1] = thick_right.p;
  xlogo_intersect(&thick_right, &gap_left, &poly_left[2]);
  poly_left[3] = gap_left.q;
  poly_left[4] = thin_left.q;
  xlogo_intersect(&thick_left, &thin_left, &poly_left[5]);

  // right polygon
  poly_right[0] = thin_right.p;
  poly_right[1] = gap_right.p;
  xlogo_intersect(&thick_left, &gap_right, &poly_right[2]);
  poly_right[3] = thick_left.q;
  poly_right[4] = thick_right.q;
  xlogo_intersect(&thick_right, &thin_right, &poly_right[5]);
}
