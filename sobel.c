#include "sobel.h"

#include <math.h>

void
fmatrix_sobel3x3(float **out, const float **in, int w_out, int h_out)
{
  int x, y;
  for (y = 0; y < h_out; y++)
    for (x = 0; x < w_out; x++) {
      float sobel_x, sobel_y;

      sobel_x = -in[y + 0][x + 0] - 2 * in[y + 1][x + 0] - in[y + 2][x + 0]
	+ in[y + 0][x + 2] + 2 * in[y + 1][x + 2] + in[y + 2][x + 2];

      sobel_y = -in[y + 0][x + 0] - 2 * in[y + 0][x + 1] - in[y + 0][x + 2]
	+ in[y + 2][x + 0] + 2 * in[y + 2][x + 1] + in[y + 2][x + 2];

      out[y][x] = std::hypot(sobel_x, sobel_y) / 8.0;
    }
}

void
fmatrix_blur3x1(float **out, const float **in, int w_out, int h_out)
{
  int x, y;
  float sum0, sum1;

  for (y = 0; y < h_out; y++) {
    sum0 = in[y][0] + in[y][1];
    for (x = 0; x < w_out; x++) {
      sum1 = in[y][x + 1] + in[y][x + 2];
      out[y][x] = sum0 + sum1;
      sum0 = sum1;
    }
  }
}

void
fmatrix_blur3x3(float **out, const float **in, int w_out, int h_out)
{
  int x, y;
  float sum_l, sum_r, sum_u, sum_d;	/* left, right, up, down */
  float sum_lr;				/* left_right */
  float sum_rows[2][w_out];

  for (y = 0; y < 2; y++) {
    sum_l = in[y][0] + in[y][1];
    for (x = 0; x < w_out; x++) {
      sum_r = in[y][x + 1] + in[y][x + 2];
      sum_rows[y][x] = sum_l + sum_r;
      sum_l = sum_r;
    }
  }

  farray_add1(sum_rows[0], sum_rows[1], w_out);

  for (y = 0; y < h_out; y++) {
    sum_l = in[y + 2][0] + in[y + 2][1];
    for (x = 0; x < w_out; x++) {
      sum_r = in[y + 2][x + 1] + in[y + 2][x + 2];
      sum_lr = sum_l + sum_r;

      sum_d = sum_rows[1][x] + sum_lr;
      sum_u = sum_rows[0][x];

      out[y][x] = (sum_u + sum_d) / 16.0;

      sum_rows[0][x] = sum_d;
      sum_rows[1][x] = sum_lr;

      sum_l = sum_r;
    }
  }

}

/* Unoptimized and more understandable variant of fmatrix_blur3x3. */
void
fmatrix_blur3x3_s(float **out, const float **in, int w_out, int h_out)
{
  int x, y;
  for (y = 0; y < h_out; y++)
    for (x = 0; x < w_out; x++)
      out[y][x] =
	(1 * in[y + 0][x + 0] + 2 * in[y + 0][x + 1] + 1 * in[y + 0][x + 2] +
	 2 * in[y + 1][x + 0] + 4 * in[y + 1][x + 1] + 2 * in[y + 1][x + 2] +
	 1 * in[y + 2][x + 0] + 2 * in[y + 2][x + 1] + 1 * in[y + 2][x + 2]) /
	16.0;
}
