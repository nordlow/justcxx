#include "rlcode.h"

#include <stdlib.h>
#include "readline/readline.h"

int
rl_encode(uchar *out, uchar *in, int nin)
{
  int o = 0, i = 0;
  const int maxlen = 128;	/* maximum allowed length of each block */
  while (i < nin) {
    int j = i + 1;
    while (j < nin && in[i] == in[j] && j - i < maxlen)
      j++;
    if (o >= nin)
      return -1;		/* no compression possible, so return */
    out[o++] = (uchar) j - i - 1;
    out[o++] = in[i];
    i = j;
  }
  return o;
}

int
rl_decode(uchar *out, int nout, uchar *in, int nin)
{
  int o = 0, i = 0;
  while (i < nin) {
    int j = in[i] + 1;
    while (j--)
      out[o++] = in[i + 1];
    i += 2;
  }
  return o;
}

/* ---------------------------- Group Separator ---------------------------- */

int
rl_encode1(uchar *out, uchar *in, int nin)
{
  int o = 0, i = 0;
  const int maxlen = 128;	/* maximum allowed length of each block */
  while (i < nin) {
    int j = i + 1;
    if (j < nin && in[i] == in[j]) {	/* run length code */
      j++;
      while (j < nin && in[i] == in[j] && j - i < maxlen)
	j++;
      if (o + 2 > nin)
	return -1;		/* no compression possible, so return */
      ((signed char *) out)[o++] = (signed char) +(j - i - 1);
      out[o++] = in[i];
      i = j;
    } else {			/* block code */
      while (j + 1 < nin && in[j] != in[j + 1] && (j - 1) - i < maxlen)
	j++;
      if (o + 1 + (j - i) > nin)
	return -1;		/* no compression possible, so return */
      ((signed char *) out)[o++] = (signed char) -(j - i);
      while (i < j)		/* copy block */
	out[o++] = in[i++];
    }
  }
  return o;
}

int
rl_decode1(uchar *out, int nout, uchar *in, int nin)
{
  int o = 0, i = 0;
  while (i < nin) {
    int j = ((signed char *) in)[i];
    if (j >= 0) {		/* decode run */
      while (j-- >= 0)
	out[o++] = in[i + 1];	/* expand run */
      i += 2;
    } else {			/* decode block */
      i++;			/* skip count */
      while (j++ < 0)
	out[o++] = in[i++];	/* copy block */
    }
  }
  return o;
}

/* ---------------------------- Group Separator ---------------------------- */

void
rl_test(void)
{
  uchar *in, *out;
  size_t inlen;
  size_t outlen;

  while (1) {
    char *line = readline(">> ");

    in = (uchar*)fload(line, NULL, &inlen);
    printf("inlen: %zd\n", inlen);

    out = malloc(inlen);
    outlen = rl_encode1(out, in, inlen);
    printf("outlen: %zd\n", outlen);

    free(line);
    free(in);
    free(out);
  }
}
