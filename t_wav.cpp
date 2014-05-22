#include <stdio.h>

#include "wavfile.h"

int
main(int argc, char *argv[])
{
  WavFile_t wav;

  if (argc != 1 + 1) {
    printf("usage: %s <file.wav>\n\n", argv[0]);
    return 0;
  }

  WAV_init(&wav);
  if (WAV_read(argv[1], &wav) < 0) {
    return -1;
  } else {
    WAV_print(&wav);
  }
  WAV_clear(&wav);

  return 0;
}
