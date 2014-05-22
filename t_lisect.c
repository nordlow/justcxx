#include "../intersect.h"

void
test_vec2d_intersect(void)
{
  vec2d a, b, c, d, i, p;
  double dst;

  a.x = 0;
  a.y = 0;

  b.x = 3;
  b.y = -3;

  c.x = 1;
  c.y = 1;

  /* normalize */
  dst = sqrt(c.x * c.x + c.y * c.y);
  c.x /= dst;
  c.y /= dst;

  d.x = 0;
  d.y = 1;

  i = vec2d_line_intersect(a, b, c, d);

  printf("intersection param: [%f, %f]\n", i.x, i.y);

  p = vec2d_line_intersection_point(a, c, i.x);
  printf("intersection at: [%f, %f]\n", p.x, p.y);

  p = vec2d_line_intersection_point(b, d, i.y);
  printf("intersection at: [%f, %f]\n", p.x, p.y);
}

int
main(int argc, char *argv[])
{
  test_vec2d_intersect();
  return 0;
}
