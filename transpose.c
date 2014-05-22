#include "transpose.h"
#include "sortn.h"

/* See MMX Note on www.intel.com. */

void
charm_sqr_transpose(char **out, int side)
{
  int x, y;
  for (y = 0; y < side; y++)
    for (x = y + 1; x < side; x++)
      SWAP(out[y][x], out[x][y]);
}

void
shortmatrix_sqr_transpose_16(short **out, int side)
{
  int x, y;
  for (y = 0; y < side; y++)
    for (x = y + 1; x < side; x++)
      SWAP(out[y][x], out[x][y]);
}

void
shortmatrix_sqr_transpose_16(short **out, int side)
{
  int x, y;
  /* Transpose in 2x2xU16 blocks */
  ulong tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
  for (y = 0; y < side; y += 2)
    for (x = y + 2; x < side; x += 2) {
      tmp0 = *((ulong *) &out[y][x]);
      tmp1 = *((ulong *) &out[y + 1][x]);

      tmp2 = (tmp0 & 0xffff) + (tmp1 << 16);
      tmp3 = (tmp0 >> 16) + (tmp1 & 0xffff0000);

      tmp0 = *((ulong *) &out[x][y]);
      tmp1 = *((ulong *) &out[x + 1][y]);

      tmp4 = (tmp0 & 0xffff) + (tmp1 << 16);
      tmp5 = (tmp0 >> 16) + (tmp1 & 0xffff0000);

      *((ulong *) &out[y][x]) = tmp4;
      *((ulong *) &out[y + 1][x]) = tmp5;

      *((ulong *) &out[x][y]) = tmp2;
      *((ulong *) &out[x + 1][y]) = tmp3;
    }
}

void
llongmatrix_sqr_transpose(long long **out, int side)
{
  int x, y;
  for (y = 0; y < side; y++)
    for (x = y + 1; x < side; x++)
      SWAP(out[y][x], out[x][y]);
}

void
llongmatrix_transpose(long long **out, const long long **in, int w_out, int h_out)
{
  int x, y;
  for (y = 0; y < h_out; y++)
    for (x = 0; x < w_out; x++)
      out[y][x] = in[x][y];	/* Linear writes to out. */
}

/* 2048 bytes long lookup table. */
static char chara2048_bitTranspose8x8_table[(1 << 8) * 8];

void
init_transpose(void)
{
  char *table;
  int b, c;

  table = chara2048_bitTranspose8x8_table;

  for (b = 0; b < 256; b++) {
    for (c = 0; c < 8; c++) {
      table[8 * b + c] = (b & (1 << c)) ? 0xff : 0x00;
    }
  }
}

void
chara8_bitTranspose8x8(char *out, const char *in)
{
  char *table;
  int o, i;

  table = chara2048_bitTranspose8x8_table;

  /* Prepare out. */
  for (o = 0; o < 8; o++)
    out[o] = 0;

  /* Lookup byte, mask out bit, and set it. */
  for (o = 0; o < 8; o++)
    for (i = 0; i < 8; i++)
      out[o] |= table[8 * in[i] + o] & (1 << i);
}

#if 0
void
chara256_bitTranspose16x16(char *out, const char *in)
{
  char table[(1 << 16) * 16];	/* 1048576 bytes long lookup table. */
  /* \TODO: Wait for level 1 - caches with megabytes of data. */
}
#endif

#if 0
void
b1m_transpose_slow(b1 ** out, const b1 ** in, int n, int m)
{
  int i, j;
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      b1m_copyB1(out, i, j, in, j, i);
}

/* m: out width, n: out height */
void
b1m_transpose(b1 ** out, const b1 ** in, int n, int m)
{
  int i, j;
  int qm, qn;

  /* Buffers. */
  char char_out[8], char_in[8];

  /* Calculate quotes. */
  qm = m / 8;
  qn = n / 8;

  /* Process 8x8-blocks of bits. Linear writes to out. */
  for (i = 0; i < qm; i++)
    for (j = 0; j < qn; j++) {
      char_in[0] = ((char **) in)[8 * j + 0][i];
      char_in[1] = ((char **) in)[8 * j + 1][i];
      char_in[2] = ((char **) in)[8 * j + 2][i];
      char_in[3] = ((char **) in)[8 * j + 3][i];
      char_in[4] = ((char **) in)[8 * j + 4][i];
      char_in[5] = ((char **) in)[8 * j + 5][i];
      char_in[6] = ((char **) in)[8 * j + 6][i];
      char_in[7] = ((char **) in)[8 * j + 7][i];

      chara8_bitTranspose8x8(char_out, char_in);

      ((char **) out)[8 * i + 0][j] = char_out[0];
      ((char **) out)[8 * i + 1][j] = char_out[1];
      ((char **) out)[8 * i + 2][j] = char_out[2];
      ((char **) out)[8 * i + 3][j] = char_out[3];
      ((char **) out)[8 * i + 4][j] = char_out[4];
      ((char **) out)[8 * i + 5][j] = char_out[5];
      ((char **) out)[8 * i + 6][j] = char_out[6];
      ((char **) out)[8 * i + 7][j] = char_out[7];
    }

  /* The rests on the "right side". */
  for (i = 0; i < 8 * qm; i++)
    for (j = 8 * qn; j < n; j++)
      b1m_copyB1(out, i, j, in, j, i);

  /* The rests on the "lower side". */
  for (i = 8 * qm; i < m; i++)
    for (j = 0; j < 8 * qn; j++)
      b1m_copyB1(out, i, j, in, j, i);

  /* The rests on the "lower right side". */
  for (i = 8 * qm; i < m; i++)
    for (j = 8 * qn; j < n; j++)
      b1m_copyB1(out, i, j, in, j, i);

}
#endif

/* \TODO: Not finished! */
void
charm_transpose(char **out, const char **in, int w_in, int h_in)
{
  int x, y;
#ifdef HAVE_CPU_MMX
  uint64_t i_buf[8], o_buf[8];
  int n_x, n_y;

  n_x = w_in / 8;
  n_y = h_in / 8;

  if (n_x != 0 && n_y != 0) {
    for (x = 0; x < n_x; x++)	/* Linear write order */
      for (y = 0; y < n_y; y++) {
	i_buf[0] = *((uint64_t *) (in[8 * y + 0] + 8 * x));
	i_buf[1] = *((uint64_t *) (in[8 * y + 1] + 8 * x));
	i_buf[2] = *((uint64_t *) (in[8 * y + 2] + 8 * x));
	i_buf[3] = *((uint64_t *) (in[8 * y + 3] + 8 * x));
	i_buf[4] = *((uint64_t *) (in[8 * y + 4] + 8 * x));
	i_buf[5] = *((uint64_t *) (in[8 * y + 5] + 8 * x));
	i_buf[6] = *((uint64_t *) (in[8 * y + 6] + 8 * x));
	i_buf[7] = *((uint64_t *) (in[8 * y + 7] + 8 * x));

	chararray_transpose8x8((char *) o_buf, (char *) i_buf);

	*((uint64_t *) (in[8 * y + 0] + 8 * x)) = o_buf[0];
	*((uint64_t *) (in[8 * y + 1] + 8 * x)) = o_buf[1];
	*((uint64_t *) (in[8 * y + 2] + 8 * x)) = o_buf[2];
	*((uint64_t *) (in[8 * y + 3] + 8 * x)) = o_buf[3];
	*((uint64_t *) (in[8 * y + 4] + 8 * x)) = o_buf[4];
	*((uint64_t *) (in[8 * y + 5] + 8 * x)) = o_buf[5];
	*((uint64_t *) (in[8 * y + 6] + 8 * x)) = o_buf[6];
	*((uint64_t *) (in[8 * y + 7] + 8 * x)) = o_buf[7];
      }
  } else {
    for (y = 0; y < h_in; y++)
      for (x = 0; x < w_in; x++)
	out[x][y] = in[y][x];
  }
#else
  for (y = 0; y < h_in; y++)
    for (x = 0; x < w_in; x++)
      out[x][y] = in[y][x];
#endif
}

void
dmatrix_transpose(double **a, double **b, int bm, int bn)
{
  int i, j;
  for (i = 0; i < bm; i++)
    for (j = 0; j < bn; j++)
      a[j][i] = b[i][j];
}
