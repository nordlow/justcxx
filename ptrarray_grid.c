#include "ptrarray_grid.h"

vec2f **
ptrarrayvec2f_newGrid(float step, float dim, size_t m, size_t n, int random)
{
  size_t i, j;
  vec2f ** a;

  a = (vec2f**)malloc((m * n) * sizeof(vec2f*));

  /* create a rectangular grid of boxes */
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      if (random) {
        a[i * n + j] = vec2f_new(step * i + dim * float_rangerand(0.0, 0.25),
                                 step * j + dim * float_rangerand(0.0, 0.25));
      } else {
        a[i * n + j] = vec2f_new(step * i, step * j);
      }
    }
  }

  return a;
}

box2f **
ptrarraybox2f_newGrid(float step, float dim, size_t m, size_t n, int random)
{
  size_t i, j;
  box2f ** a;

  a = (box2f**)malloc((m * n) * sizeof(box2f*));

  /* create a rectangular grid of boxes */
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      if (random) {
        a[i * n + j] = box2f_new(step * i + dim * float_rangerand(0.0, 0.25),
                                 step * j + dim * float_rangerand(0.0, 0.25),
                                 step * i + dim * float_rangerand(0.5, 0.75),
                                 step * j + dim * float_rangerand(0.5, 0.75));
      } else {
        a[i * n + j] = box2f_new(step * i,
                                 step * j, step * i + dim, step * j + dim);
      }
    }
  }

  return a;
}

vis_t **
ptrarrayvis_newGrid(float step, float dim, size_t m, size_t n, int random)
{
  size_t i, j;
  vis_t ** a;

  a = (vis_t**)malloc((m * n) * sizeof(vis_t*));

  /* create a rectangular grid of boxes */
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      box2f bnd;

      if (random) {
        box2f_set(&bnd,
                  step * i + dim * float_rangerand(0.0, 0.25),
                  step * j + dim * float_rangerand(0.0, 0.25),
                  step * i + dim * float_rangerand(0.5, 0.75),
                  step * j + dim * float_rangerand(0.5, 0.75));

        a[i * n + j] = vis_new(SFORM_BOX2f, &bnd, COLOR_PAPAYA_WHIP, 0);
      } else {
        box2f_set(&bnd,
                  step * i,
                  step * j,
                  step * i + dim,
                  step * j + dim);

        a[i * n + j] = vis_new(SFORM_BOX2f, &bnd, COLOR_PAPAYA_WHIP, 0);
      }
    }
  }

  return a;
}

void
ptrarrayvis_clrGrid(vis_t ** grid, size_t m, size_t n)
{
  size_t i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      vis_delete(grid[i * n + j]);
    }
  }
  free(grid);
}
