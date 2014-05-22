#include "chv.h"
#include "encode.h"
#include "decode.h"
#include "stdio_x.h"
#include "pnw_types.h"

#include <ctype.h>
#include <string.h>
#include <arpa/inet.h>

/* #include <sys/types.h> /\* Defines in_addr_t, needed by <netinet/in.h> *\/ */
/* #include <netinet/in.h> */

void
chv_init(chv_t * chv)
{
  chv->size = 0;
  chv->room = 0;
  chv->buf = NULL;
}

void
chv_init_load(chv_t * chv, const char * path)
{
  chv_init(chv);
  chv_load(chv, path);
}

void
chv_init_room(chv_t * chv, size_t room)
{
  chv->size = 0;
  chv->room = room;
  if (room > 0) {
    chv->buf = malloc(chv->room);
  } else {
    chv->buf = NULL;
  }
}

void
chv_init_cstr(chv_t * chv, const char *cstr)
{
  size_t len = strlen(cstr);
  chv->size = len;
  chv->room = len;
  chv->buf = malloc(len);
  memcpy(chv->buf, cstr, len);
}

void
chv_init_cbuf(chv_t * chv, const unsigned char *str, size_t len)
{
  chv->size = len;
  chv->room = len;
  chv->buf = malloc(len);
  memcpy(chv->buf, str, len);
}

void
chv_init_extract_front(chv_t * chv, chv_t * src_chv, size_t len)
{
  chv_init_room(chv, len);
  chv_pop(src_chv, chv->buf, len);
  chv->size = len;
}

void
chv_init_clone(chv_t * a, const chv_t * b)
{
  chv_init_room(a, b->size);
  chv_clone(a, b);
}

/* ---------------------------- Group Separator ---------------------------- */

unsigned char *
chv_get_buf(chv_t * a)
{
  return a->buf;
}

size_t
chv_get_size(chv_t * a)
{
  return a->size;
}

/* ---------------------------- Group Separator ---------------------------- */

void
chv_clone(chv_t * a, const chv_t * b)
{
  if (a->room < b->room) {
    a->buf = realloc(a->buf, b->room);
    a->room = b->room;
  }
  a->size = b->size;
  memcpy(a->buf, b->buf, a->size);
}

/* ---------------------------- Group Separator ---------------------------- */

void
chv_clear(chv_t * chv)
{
  free(chv->buf);
  chv->buf = NULL;
  chv->size = 0;
  chv->room = 0;
}

/* ---------------------------- Group Separator ---------------------------- */

void
chv_resize(chv_t * chv, size_t size)
{
  /* allocate extra */
  if (size > chv->room) {
    chv->buf = realloc(chv->buf, size);
    chv->room = size;
  }

  /* zero */
  if (size > chv->size) {
    memset(chv->buf + chv->size, 0, size - chv->size);
  }

  chv->size = size;
}

void
chv_incsize(chv_t * chv, size_t inc)
{
  chv_resize(chv, chv->size + inc);
}

/* ---------------------------- Group Separator ---------------------------- */

void
chv_app_ch(chv_t * chv, char ch)
{
  chv_incsize(chv, sizeof(ch));
  chv->buf[chv->size - sizeof(ch)] = ch;
}

void
chv_app_str(chv_t * chv, const char *str)
{
  size_t len = strlen(str);
  const size_t old_size = chv->size;
  chv_incsize(chv, len);
  memcpy(chv->buf + old_size, str, len);
}

int
chv_app_cbuf(chv_t * chv, const unsigned char *buf, size_t len)
{
  const size_t old_size = chv->size;
  chv_incsize(chv, len);
  memcpy(chv->buf + old_size, buf, len);
  return len;
}

void
chv_app_chv(chv_t * dest, chv_t * src)
{
  chv_app_cbuf(dest, src->buf, src->size);
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_pop(chv_t * chv, unsigned char *buf, size_t len)
{
  if (len > chv->size) {
    PWARN("Truncated len=%zd to %zd\n", len, chv->size);
    len = chv->size;
  }

  memcpy(buf, chv->buf, len);
  memmove(chv->buf, chv->buf + len, chv->size - len);

  chv->size -= len;

  return len;
}

int
chv_drop(chv_t * chv, size_t len)
{
  if (len > chv->size) {
    PWARN("Truncated len=%zd to %zd\n", len, chv->size);
    len = chv->size;
  }

  memmove(chv->buf, chv->buf + len, chv->size - len);

  chv->size -= len;

  return len;
}

/* ---------------------------- Group Separator ---------------------------- */

void
chv_fprint(FILE * stream, chv_t * chv)
{
  printf("[ ");
  for (size_t i = 0; i < chv->size; i++) {
    printf("%u ", chv->buf[i]);
  }
  printf("]");
}

void
chv_print(chv_t * chv)
{
  chv_fprint(stdout, chv);
}

void
chv_fprint_x(FILE * stream, chv_t * chv)
{
  printf("[ ");
  for (size_t i = 0; i < chv->size; i++) {
    uint ch = chv->buf[i];
    if (isprint(ch)) {
      printf("%c ", ch);
    } else {
      printf("0x%02x ", (uchar) ch);
    }
  }
  printf("]");
}

void
chv_fprint_x_quoted_chars(FILE * stream, chv_t * chv)
{
  printf("[ ");
  for (size_t i = 0; i < chv->size; i++) {
    uint ch = chv->buf[i];
    if (isprint(ch)) {
      printf("'%c' ", ch);
    } else {
      printf("0x%02x ", (uchar) ch);
    }
  }
  printf("]");
}

void
chv_print_x(chv_t * chv)
{
  chv_fprint_x(stdout, chv);
}

void
chv_fprintall(FILE * stream, chv_t * chv)
{
  printf("size:%zd room:%zd buf:%p  ", chv->size, chv->room, chv->buf);
  chv_fprint_x(stream, chv);
}

void
chv_printall(chv_t * chv)
{
  chv_fprintall(stdout, chv);
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_save(chv_t * chv, const char *path)
{
  size_t ret;
  FILE *stream = fopen(path, "wb");
  if (!stream) {
    perror("fopen");
    return -1;
  }

  ret = fwrite(chv->buf, 1, chv->size, stream);
  if (ret != chv->size) { perror("fwrite"); return -1; }

  fclose(stream);

  return 0;
}

int
chv_load(chv_t * chv, const char *path)
{
  size_t ret;
  size_t size;
  FILE *stream = fopen(path, "rb");
  if (!stream) { perror("fopen"); return -1; }

  fseek(stream, 0, SEEK_END);
  size = ftell(stream);
  fseek(stream, 0, SEEK_SET);

  chv_resize(chv, size);

  ret = fread(chv->buf, 1, size, stream);
  if (ret != size) { perror("fread"); return -1; }

  fclose(stream);

  return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_enc_s8(chv_t * chv, const int8_t * x)
{
  chv_app_ch(chv, *x);
  return 1;
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_enc_s16be(chv_t * chv, const int16_t * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_s16be(chv->buf + old_size, x);
  return sz;
}

int
chv_enc_s16le(chv_t * chv, const int16_t * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_s16le(chv->buf + old_size, x);
  return sz;
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_enc_s32be(chv_t * chv, const int32_t * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_s32be(chv->buf + old_size, x);
  return sz;
}

int
chv_enc_s32le(chv_t * chv, const int32_t * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_s32le(chv->buf + old_size, x);
  return sz;
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_enc_s64be(chv_t * chv, const int64_t * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_s64be(chv->buf + old_size, x);
  return sz;
}

int
chv_enc_s64le(chv_t * chv, const int64_t * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_s64le(chv->buf + old_size, x);
  return sz;
}

/* ---------------------------- Group Separator ---------------------------- */

int chv_enc_u8(chv_t * chv, const uint8_t * x) { return chv_enc_s8(chv, (int8_t*) x); }

int chv_enc_u16be(chv_t * chv, const uint16_t * x) { return chv_enc_s16be(chv, (int16_t*) x); }
int chv_enc_u16le(chv_t * chv, const uint16_t * x) { return chv_enc_s16le(chv, (int16_t*) x); }

int chv_enc_u32be(chv_t * chv, const uint32_t * x) { return chv_enc_s32be(chv, (int32_t*) x); }
int chv_enc_u32le(chv_t * chv, const uint32_t * x) { return chv_enc_s32le(chv, (int32_t*) x); }

int chv_enc_u64be(chv_t * chv, const uint64_t * x) { return chv_enc_s64be(chv, (int64_t*) x); }
int chv_enc_u64le(chv_t * chv, const uint64_t * x) { return chv_enc_s64le(chv, (int64_t*) x); }

/* ---------------------------- Group Separator ---------------------------- */

int
chv_enc_f32be(chv_t * chv, const float * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_f32be(chv->buf + old_size, x);
  return sz;
}

int
chv_enc_f32le(chv_t * chv, const float * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_f32le(chv->buf + old_size, x);
  return sz;
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_enc_f64be(chv_t * chv, const double * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_f64be(chv->buf + old_size, x);
  return sz;
}

int
chv_enc_f64le(chv_t * chv, const double * x)
{
  const uint sz = sizeof(*x);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_f64le(chv->buf + old_size, x);
  return sz;
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_enc_sizebe(chv_t * chv, const size_t * x)
{
  const uint sz = sizeof(uint64_t);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_sizebe(chv->buf + old_size, x);
  return sz;
}

int
chv_enc_sizele(chv_t * chv, const size_t * x)
{
  const uint sz = sizeof(uint64_t);
  const size_t old_size = chv->size;
  chv_incsize(chv, sz);
  benc_sizele(chv->buf + old_size, x);
  return sz;
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_dec_s8(chv_t * chv, int8_t *x)
{
  const uint sz = sizeof(*x);
  if (sz <= chv->size) {
    memcpy(x, chv->buf, sz);
    chv_drop(chv, sz);
    return sz;
  } else {
    return -1;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_dec_s16be(chv_t * chv, int16_t *x)
{
  const uint sz = sizeof(*x);
  if (sz <= chv->size) {
    *x = bdec_s16be(chv->buf);
    chv_drop(chv, sz);
    return sz;
  } else {
    return -1;
  }
}

int
chv_dec_s16le(chv_t * chv, int16_t *x)
{
  const uint sz = sizeof(*x);
  if (sz <= chv->size) {
    *x = bdec_s16le(chv->buf);
    chv_drop(chv, sz);
    return sz;
  } else {
    return -1;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_dec_s32be(chv_t * chv, int32_t *x)
{
  const uint sz = sizeof(*x);
  if (sz <= chv->size) {
    *x = bdec_s32be(chv->buf);
    chv_drop(chv, sz);
    return sz;
  } else {
    return -1;
  }
}

int
chv_dec_s32le(chv_t * chv, int32_t *x)
{
  const uint sz = sizeof(*x);
  if (sz <= chv->size) {
    *x = bdec_s32le(chv->buf);
    chv_drop(chv, sz);
    return sz;
  } else {
    return -1;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_dec_s64be(chv_t * chv, int64_t *x)
{
  const uint sz = sizeof(*x);
  if (sz <= chv->size) {
    *x = bdec_s64be(chv->buf);
    chv_drop(chv, sz);
    return sz;
  } else {
    return -1;
  }
}

int
chv_dec_s64le(chv_t * chv, int64_t *x)
{
  const uint sz = sizeof(*x);
  if (sz <= chv->size) {
    *x = bdec_s64le(chv->buf);
    chv_drop(chv, sz);
    return sz;
  } else {
    return -1;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

int chv_dec_u8(chv_t * chv, uint8_t *x) { return chv_dec_s8(chv, (int8_t *) x); }

int chv_dec_u16be(chv_t * chv, uint16_t *x) { return chv_dec_s16be(chv, (int16_t *) x); }
int chv_dec_u16le(chv_t * chv, uint16_t *x) { return chv_dec_s16le(chv, (int16_t *) x); }

int chv_dec_u32be(chv_t * chv, uint32_t *x) { return chv_dec_s32be(chv, (int32_t *) x); }
int chv_dec_u32le(chv_t * chv, uint32_t *x) { return chv_dec_s32le(chv, (int32_t *) x); }

int chv_dec_u64be(chv_t * chv, uint64_t *x) { return chv_dec_s64be(chv, (int64_t *) x); }
int chv_dec_u64le(chv_t * chv, uint64_t *x) { return chv_dec_s64le(chv, (int64_t *) x); }

/* ---------------------------- Group Separator ---------------------------- */

int
chv_dec_f32be(chv_t * chv, float *x)
{
  int ret;
  uint32_t ul;
  ret = chv_dec_u32be(chv, &ul);
  memcpy(x, &ul, sizeof(*x));
  return ret;
}

int
chv_dec_f32le(chv_t * chv, float *x)
{
  int ret;
  uint32_t ul;
  ret = chv_dec_u32le(chv, &ul);
  memcpy(x, &ul, sizeof(*x));
  return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_dec_sizebe(chv_t * chv, size_t *x)
{
  uint64_t x_U64;          /* NOTE: use 64-bit on all architectures */
  int ret = chv_dec_u64be(chv, &x_U64);
  if (sizeof(x_U64) > sizeof(*x) &&
      x_U64 > SIZE_MAX) {
    PWARN("Truncated %llu to %zu\n", x_U64, SIZE_MAX);
  }
  *x = x_U64;
  return ret;
}

int
chv_dec_sizele(chv_t * chv, size_t *x)
{
  uint64_t x_U64;          /* NOTE: use 64-bit on all architectures */
  int ret = chv_dec_u64le(chv, &x_U64);
  if (sizeof(x_U64) > sizeof(*x) &&
      x_U64 > SIZE_MAX) {
    PWARN("Truncated %llu to %zu\n", x_U64, SIZE_MAX);
  }
  *x = x_U64;
  return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_dec_f64be(chv_t * chv, double *x)
{
  const uint sz = sizeof(*x);
  if (sz <= chv->size) {
    *x = bdec_f64be(chv->buf);
    chv_drop(chv, sz);
    return sz;
  } else {
    return -1;
  }
}

int
chv_dec_f64le(chv_t * chv, double *x)
{
  const uint sz = sizeof(*x);
  if (sz <= chv->size) {
    *x = bdec_f64le(chv->buf);
    chv_drop(chv, sz);
    return sz;
  } else {
    return -1;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

int
chv_enc_cstr_lengthU32be(chv_t * chv, const char *str)
{
  int cnt = 0;
  uint32_t len = strlen(str);
  cnt += chv_enc_u32be(chv, &len);
  cnt += chv_app_cbuf(chv, (const unsigned char*)str, len);
  return cnt;
}

int
chv_enc_cstr_lengthU32le(chv_t * chv, const char *str)
{
  int cnt = 0;
  uint32_t len = strlen(str);
  cnt += chv_enc_u32le(chv, &len);
  cnt += chv_app_cbuf(chv, (const unsigned char*)str, len);
  return cnt;
}

int chv_dec_cstr_lengthU32be(chv_t * chv, char **cstr_ret)
{
  return chv_dec_cstr_lengthU32(chv, cstr_ret, 0);
}

int chv_dec_cstr_lengthU32le(chv_t * chv, char **cstr_ret)
{
  return chv_dec_cstr_lengthU32(chv, cstr_ret, 1);
}

int
chv_dec_cstr_lengthU32(chv_t * chv, char **cstr_ret, int little_endian_flag)
{
  int ret;
  uint32_t len;

  if (little_endian_flag) {
    ret = chv_dec_u32le(chv, &len);
  } else {
    ret = chv_dec_u32be(chv, &len);
  }

  *cstr_ret = NULL;		/* default to empty string */

  if (ret == 4) {
    if (len <= chv->size) {
      char *str = malloc(len + 1); /* allocate len + terminator */
      chv_pop(chv, (unsigned char*) str, len);
      str[len] = '\0';          /* set terminator */
      *cstr_ret = str;		/* write returned string */
      return 0;
    } else {
      PWARN("Truncated len=%d to %zd\n", len, chv->size);
      return -1;
    }
  } else {
    PWARN("Decoding of string length returned %d not equal to 4\n",
	     ret);
    return -1;
  }
}

/* ---------------------------- Group Separator ---------------------------- */
