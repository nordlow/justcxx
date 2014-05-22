#include "perm.h"

void
farray_perm(float *a, const float *b, const size_t *p, size_t n)
{
  if (a == b || b == 0) {
    float t[n];
    for (size_t i = 0; i < n; i++) { t[i] = a[p[i]]; }
    memcpy(a, t, n * sizeof(float));
  } else {
    for (size_t i = 0; i < n; i++) { a[i] = b[p[i]]; }
  }
}

void
darray_perm(double *a, const double *b, const size_t *p, size_t n)
{
  if (a == b || b == 0) {
    double t[n];
    for (size_t i = 0; i < n; i++) { t[i] = a[p[i]]; }
    memcpy(a, t, n * sizeof(double));
  } else {
    for (size_t i = 0; i < n; i++) { a[i] = b[p[i]]; }
  }
}
