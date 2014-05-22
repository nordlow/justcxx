#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

#include "wavfile.h"
#include "endian_utils.h"

/*!
  COMP630 WAV File Format Description

  WAV - format:
  http://www.technology.niagarac.on.ca/courses/comp630/WavFileFormat.html

  WAV files are probably the simplest of the common formats for storing audio
  samples. Unlike MPEG and other compressed formats, WAVs store samples
  "in the raw" where no pre-processing is required other that formatting of
  the data.

  The following information was derived from several sources including some
  on the internet which no longer exist.
  Being somewhat of a proprietary Microsoft format there are some elements
  here which were empirically determined
  and so some details may remain somewhat sketchy. From what I've heard,
  the best source for information is the File
  Formats Handbook by Gunter Born (1995, ITP Boston)

  The WAV file itself consists of three "chunks" of information: The RIFF
  chunk which identifies the file as a WAV file,
  The FORMAT chunk which identifies parameters such as sample rate and the
  DATA chunk which contains the actual data (samples).

  Each Chunk breaks down as follows:

  RIFF Chunk (12 bytes in length total)
  Byte Number
  0 - 3    "RIFF" (ASCII Characters)
  4 - 7    Total Length Of Package To Follow (Binary, little endian)
  8 - 11   "WAVE" (ASCII Characters)

  FORMAT Chunk (24 bytes in length total)
  Byte Number
  0 - 3    "fmt_" (ASCII Characters)
  4 - 7    Length Of FORMAT Chunk (Binary, always 0x10)
  8 - 9    Always 0x01
  10 - 11  Channel Numbers (Always 0x01=Mono, 0x02=Stereo)
  12 - 15  Sample Rate (Binary, in Hz)
  16 - 19  Bytes Per Second
  20 - 21  Bytes Per Sample: 1=8 bit Mono,
  2=8 bit Stereo or 16 bit Mono, 4=16 bit Stereo
  22 - 23  Bits Per Sample

  DATA Chunk
  Byte Number
  0 - 3    "data" (ASCII Characters)
  4 - 7    Length Of Data To Follow
  8 - end  Data (Samples)

  The easiest approach to this file format might be to look at an actual WAV
  file to see how data is stored. In this case, we examine DING.WAV which is
  standard with all Windows packages. DING.WAV is an 8-bit, mono, 22.050 KHz
  WAV file of 11,598 bytes in length. Lets begin by looking at the header of
  the file (using DEBUG).

  246E:0100  52 49 46 46 46 2D 00 00-57 41 56 45 66 6D 74 20   RIFFF-..WAVEfmt
  246E:0110  10 00 00 00 01 00 01 00-22 56 00 00 22 56 00 00   ........"V.."V..
  246E:0120  01 00 08 00 64 61 74 61-22 2D 00 00 80 80 80 80   ....data"-......
  246E:0130  80 80 80 80 80 80 80 80-80 80 80 80 80 80 80 80   ................
  246E:0140  80 80 80 80 80 80 80 80-80 80 80 80 80 80 80 80   ................

  As expected, the file begins with the ASCII characters "RIFF" identifying it
  as a WAV file. The next four bytes tell us the length is 0x2D46 bytes
  (11590 bytes in decimal) which is the length of the entire file minus the 8
  bytes for the "RIFF" and length (11598 - 11590 = 8 bytes).

  The ASCII characters for "WAVE" and "fmt " follow. Next (line 2 above) we
  find the value 0x00000010 in the first 4 bytes (length of format chunk:
  always constant at 0x10). The next four bytes are 0x0001 (Always) and 0x0001
  (A mono WAV, one channel used).

  Since this is a 8-bit WAV, the sample rate and the bytes/second are the same
  at 0x00005622 or 22,050 in decimal. For a 16-bit stereo WAV the bytes/sec
  would be 4 times the sample rate. The next 2 bytes show the number of bytes
  per sample to be 0x0001 (8-bit mono) and the number of bits per sample to be
  0x0008.

  Finally, the ASCII characters for "data" appear followed by 0x00002D22
  (11,554 decimal) which is the number of bytes of data to follow (actual
  samples). The data is a value from 0x00 to 0xFF. In the example above 0x80
  would represent "0" or silence on the output since the DAC used to playback
  samples is a bipolar device (i.e. a value of 0x00 would output a negative
  voltage and a value of 0xFF would output a positive voltage at the output of
  the DAC on the sound card).

  Note that there are extension to the basic WAV format which may be supported
  in newer systems -- for example if you look at DING.WAV in Windows '95
  you'll see some extra bytes added after the format chunk before the "data"
  area -- but the basic format remains the same.

  As a final example consider the header for the following WAV file recorded
  at 44,100 samples per second in 16-bit stereo.

  246E:0100  52 49 46 46 2C 48 00 00-57 41 56 45 66 6D 74 20   RIFF,H..WAVEfmt
  246E:0110  10 00 00 00 01 00 02 00-44 AC 00 00 10 B1 02 00   ........D.......
  246E:0120  04 00 10 00 64 61 74 61-00 48 00 00 00 00 00 00   ....data.H......
  246E:0130  00 00 00 00 00 00 00 00-00 00 00 00 00 00 00 00   ................

  Again we find all the expected structures. Note that the sample rate is
  0xAC44 (44,100 as an uint in decimal) and the bytes/second is 4
  times that figure since this is a 16-bit WAV (* 2) and is stereo
  (again * 2). The Channel Numbers field is also found to be 0x02 here and the
  bits per sample is 0x10 (16 decimal).
*/

void
WAV_init(WavFile_t * wav)
{
  wav->data = 0;
}

void
WAV_clear(WavFile_t * wav)
{
  free(wav->data);
  wav->data = 0;
}

void
WAV_print(const WavFile_t * wav)
{
  printf("PCM, %d kHz, %d-bit", wav->samp_rate / 1000, wav->prec * 8);
  switch (wav->chan_num) {
  case 1:
    printf(", mono");
    break;
  case 2:
    printf(", stereo");
    break;
  default:
    printf(", %d-channel", wav->chan_num);
    break;
  }
  printf(", %d samples\n", wav->samp_num);

  uint i, j;
  for (i = 0; i < wav->samp_num; i++) {
    printf("[ ");
    for (j = 0; j < wav->chan_num; j++) {
      printf("%f ", wav->data[i * wav->chan_num + j]);
    }
    printf("]\n");
  }
}

static void fread_err(const char *chunk_name, const char * path)
{
  leprintf("Could not read at %s for file %s\n", chunk_name, path);
}

static void decode_err(const char *chunk_name, const char * path)
{
  leprintf("Incorrect file format at %s for file %s\n", chunk_name, path);
}

static void prec_err(const uint prec, const char * path)
{
  leprintf("Cannot decode samples with precision %d for file %s\n", prec, path);
}

int
WAV_read(const char *path, WavFile_t * wav)
{
  FILE *file;

  file = fopen(path, "rb");
  if (file == NULL) {
    leprintf("Error: Could open file %s!\n", path);
    return -1;
  }

  int ret;

  /* riff */

  const int riff_len = 12;
  char riff_buf[riff_len];
  ret = fread(riff_buf, sizeof(char), riff_len, file);
  if (ret != riff_len) {
    fread_err("RIFF chunk", path);
  }
  if (strncmp(riff_buf + 0, "RIFF", 4) != 0 ||
      strncmp(riff_buf + 8, "WAVE", 4) != 0) {
    decode_err("RIFF chunk", path);
    return -1;
  }
  int file_len = 8 + INT32_TOGGLE_LE(*((uint32_t *) (riff_buf + 4)));
  lprintf("File length: %d\n", file_len);

  /* format */

  const int fmt_len = 24;
  char fmt_buf[fmt_len];
  ret = fread(fmt_buf, sizeof(char), fmt_len, file);
  if (ret != fmt_len) {
    fread_err("FORMAT chunk", path);
  }
  if (strncmp(fmt_buf + 0, "fmt", 3) != 0) {
    decode_err("FORMAT chunk", path);
    return -1;
  }
  wav->chan_num = INT16_TOGGLE_LE(*((uint16_t *) (fmt_buf + 10)));
  wav->samp_rate = INT32_TOGGLE_LE(*((uint32_t *) (fmt_buf + 12)));
  uint bytes_per_sample = INT16_TOGGLE_LE(*((uint16_t *) (fmt_buf + 20)));
  wav->prec = bytes_per_sample / wav->chan_num;
/*     ushort samp_bits = INT16_TOGGLE_LE (*((uint16_t *)(fmt_buf + 22))); */

  /* crap - needed because of Microsoft's standard procedure of changing
   * their standards constantly. */

  const int crap_len = 2;
  char crap_buf[crap_len];
  ret = fread(crap_buf, sizeof(char), crap_len, file);
  int crap_num = INT16_TOGGLE_LE(*((uint16_t *) (crap_buf + 0)));
  if (crap_num == 0) {
    fseek(file, 12, SEEK_CUR);
  } else {
    fseek(file, -4, SEEK_CUR);
  }

  /* data */

  const int dat_len = 8;
  char dat_buf[dat_len];
  ret = fread(dat_buf, sizeof(char), dat_len, file);
  if (ret != dat_len) {
    fread_err("DATA chunk", path);
  }
  if (strncmp(dat_buf + 0, "data", 4)) {
    decode_err("DATA chunk", path);
    return -1;
  }

  const int sz = INT32_TOGGLE_LE(*((uint32_t *) (dat_buf + 4)));
  wav->samp_num = sz / wav->chan_num / wav->prec;

  printf("sz: %d, snum: %d\n", sz, wav->samp_num);

  /* samples */

  uchar *samp_buf = malloc(sz);
  ret = fread(samp_buf, sizeof(uchar), sz, file);
  if (ret != sz) {
    fread_err("SAMPLES chunk", path);
  }

  wav->data = malloc(wav->samp_num * wav->chan_num * sizeof(float));

  double sum = 0;
  uint i;
  switch (wav->prec) {
  case 1: {
    uint8_t *samples = (uint8_t *) samp_buf;
    float zoff = 128;		/* zero offset */
    for (i = 0; i < wav->samp_num * wav->chan_num; i++) {
      sum += wav->data[i] = (float) samples[i] - zoff;
    }
    break;
  }
  case 2: {
    uint16_t *samples = (uint16_t *) samp_buf;
    float zoff = 512;		/* zero offset: FIXME: Check mean */
    for (i = 0; i < wav->samp_num * wav->chan_num; i++) {
      sum += wav->data[i] = (float) INT16_TOGGLE_LE(samples[i]) - zoff;
    }
    break;
  }
  default:
    prec_err(wav->prec, path);
    return -1;
    break;
  }
  /*     printf ("DC-level: %g\n", sum / (wav->samp_num * wav->chan_num)); */

  free(samp_buf);

  fclose(file);

  return 0;
}
