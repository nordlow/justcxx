#include "chv_gmp.h"
#include "stdio_x.h"

int chv_enc_mpz(chv_t * chv, const mpz_t a, int little_endian_flag)
{
  int ret = 0;
  void * rop = NULL;		// no preallocated memory
  size_t count;                 /* number of bytes neeeded to store mpz */
  const int order = 1; // 1:most sign. word first, -1:least sign. word first
  const int size = 4;  /* word size */
  const int endian = 1;   // 1:BIG_ENDIAN, -1:LITTLE_ENDIAN, 0:NATIVE_ENDIAN
  const size_t nails = 0; // number of unused MSBs in each word
  void * data = mpz_export(rop, &count, order, size, endian, nails, a);
  chv_enc_sizebe(chv, &count);
  if (count) {
    const size_t len = count * size;
    chv_app_cbuf(chv, data, len);
    ret = sizeof(uint64_t) + len;
  } else {
    PERR("No data\n");
    ret = -1;
  }
  free(data);
  return ret;
}

int chv_dec_mpz(chv_t * chv, mpz_t a, int little_endian_flag)
{
  int ret = 0;
  size_t count;
  chv_dec_sizebe(chv, &count);      /* decode size in bytes */
  if (count) {
    const int order = 1; // 1:most sign. word first, -1:least sign. word first
    const int size = 4;  /* word size */
    const int endian = 1;  // 1:BIG_ENDIAN, -1:LITTLE_ENDIAN, 0:NATIVE_ENDIAN
    const size_t nails = 0;      // number of unused MSBs in each word
    const size_t len = count * size;
    if (chv_get_size(chv) >= len) {
      mpz_import(a, count, order, size, endian, nails, chv_get_buf(chv));
      chv_drop(chv, len);       /* drop the data */
      ret = sizeof(uint64_t) + len;
    } else {
      PERR("Need %d bytes, but only have %d byte\n", len, chv_get_size(chv));
      ret = -1;
    }
  } else {
    PERR("Empty data\n");
    ret = -1;
    mpz_set_ui(a, 0);           /* zero it */
  }
  return ret;
}

int chv_enc_mpq(chv_t * chv, const mpq_t a, int little_endian_flag)
{
  return (chv_enc_mpz(chv, mpq_numref(a), little_endian_flag) +
          chv_enc_mpz(chv, mpq_denref(a), little_endian_flag));
}

int chv_dec_mpq(chv_t * chv, mpq_t a, int little_endian_flag)
{
  return (chv_dec_mpz(chv, mpq_numref(a), little_endian_flag) +
          chv_dec_mpz(chv, mpq_denref(a), little_endian_flag));
}
