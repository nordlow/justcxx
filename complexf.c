#include "complexf.h"

void
complexfarray_fprint(FILE * stream, const complexf * in, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    complexf_out_str(stream, &in[i]);
    if (i < n - 1) {
      putc(' ', stream);
    } else {
      putc('\n', stream);
    }
  }
}
