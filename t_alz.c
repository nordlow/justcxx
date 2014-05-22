#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../utils.h"
#include "../meman.h"
#include "../a3d.h"
#include "../alz3d.h"

void test_a3d(void)
{
  A3d a3d; a3d_init(&a3d, "home/per/KEMAR/compact", ".dat", 44100);

  size_t inslen = a3d.filtlen;
  float * ins = farray_calloc(inslen); /**< Input sample buffer. */

  float inamp = 32767;
  float infreq = 1.0;

  float elev = 0, azim = 90;

  for (size_t i = 0; i < inslen; i++) {
    /* make previous samples one sample-time "older" */
    memmove(ins, ins + 1, (inslen-1) * sizeof(float));

    /* append recent sample */
    float in = inamp * sinf(infreq * M_2PI * i / inslen);
    ins[inslen-1] = in;

    float outL, outR;		/* out sample */
    a3d_filter(&a3d, elev, azim, ins, inslen, &outL, &outR);
    printf("in:[%f] => out:[%f,%f]\n", in, outL, outR);
  }

  free(ins);

  a3d_clear(&a3d);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void test_alz(void)
{
  Alz3d alz;
  int swnum = 128;
  int schan = 2;
  int srate = 44100;
  int sprecision = 16;
  bool sign_flag = 1;
  bool little_endian_flag = FALSE;
  int use_a3dflag = TRUE;

  alz3d_init(&alz, schan, srate, sprecision, sign_flag, little_endian_flag, use_a3dflag, NULL);

  for (size_t i = 0; i < 100; i++) {
    alz3d_addNow(&alz,
                 220 + i,
                 10,
                 90,
                 0.1,
                 ALZ3D_WAVE_SINE, ALZ3D_FADE_OUT);
  }

  int fd = open((char*)"/dev/null", O_WRONLY, 0);
  for (size_t i = 0; i < 128; i++) {
    alz3d_write(&alz, fd, swnum);
  }
  close(fd);

  alz3d_clear(&alz);
}

#include "../linfilt.h"

int main(void)
{
  test_alz();
}
