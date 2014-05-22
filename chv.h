/*!
 * \file chv.h
 * \brief Encode and decode of C-types using \c chv_t (acronym "CHar Vector").
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * All communication over networks and files must somehow define ways of
 * serializing (<b>encoding</b>) and deserializing (<b>decoding</b>)
 * all data structures involved.
 *
 * The \c chv_t makes this possible in a simple and objectoriented way
 * that stimulates the <i>reuse</i> of code.
 *
 * Probably the best way of getting a grip about how to use it is to look
 * at the test function \c chv_test() defined at the end of chv.c.
 *
 * All functions works on arguments of the type chv_t*.
 *
 * A normal use could be described in the following steps:
 * -# Initialize chv_t by calling one of the initialization functions
 *    beginning with \c chv_init_...() with a (chv_t *) as argument.
 * -# Encode \c chv_enc_...() according to the protocol.
 * -# Send to network alt. write to a file.
 * -# Receive from network alt. read from file.
 * -# Decode \c chv_dec_...() according to the protocol.
 * -# Clear \c chv_t structure using \c chv_clear().
 *
 * To look at the contents of a \c chv_t for example at debugging it
 * is practical to use \c chv_print_x(). It prints each single
 * character in the vector as an \em ASCII-character if it is
 * printable, otherwise as a \em hexadecimal value.
 */

#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * The Char Vector structure.
 */
typedef struct
{
  size_t size;			/**< Size of the vector. */
  size_t room;			/**< Size of the allocated buffer. */
  unsigned char *buf;           /**< Buffer containing the chars. */
} chv_t;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Initialization
 *
 * One and only one of these functions MUST be called before any other
 * operations can be performed on the structure chv_t.
 */

/* @{ */

/*! Default Initialize \p chv. */
void chv_init(chv_t * chv);

/*! Initialize \p chv with a specific room \p room. */
void chv_init_room(chv_t * chv, size_t room);

/*! Initialize \p chv from a C-style string \p cstr. */
void chv_init_cstr(chv_t * chv, const char *cstr);

/*! Initialize \p chv from a string pointer \p str of length \p len.*/
void chv_init_cbuf(chv_t * chv, const unsigned char *str, size_t len);

/*! Initialize \p a to be a copy of \p b. */
void chv_init_clone(chv_t * a, const chv_t * b);

/*! Initialize \p a to be a copy of bytes contained in the file \p path. */
void chv_init_load(chv_t * chv, const char * path);

/*!
 * Initialize \p chv and then extract (move) the first len bytes from \p src_chv
 * to chv.
 */
void chv_init_extract_front(chv_t * chv, chv_t * src_chv, size_t len);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Getters
 *
 */

/* @{ */

unsigned char *chv_get_buf(chv_t * a);

size_t chv_get_size(chv_t * a);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

void chv_clone(chv_t * a, const chv_t * b);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Clearing
 *
 * One and only one of these functions MUST be called after all processing is
 * done. It basically deallocates all memory associated with a chv_t.
 */

/* @{ */

/*! Default clearing. */
void chv_clear(chv_t * chv);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Changing size.
 *
 */

/* @{ */

void chv_resize(chv_t * chv, size_t size);
void chv_incsize(chv_t * chv, size_t inc);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Appending
 *
 * Append to chv_t.
 */

/* @{ */

void chv_app_ch(chv_t * chv, char ch);
void chv_app_str(chv_t * chv, const char *str);
int chv_app_cbuf(chv_t * chv, const unsigned char *buf, size_t len);
void chv_app_chv(chv_t * dest, chv_t * src);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Pop (Copy) \p len bytes from \p chv int the area \p buf.
 *
 * \param[in,out] buf points to a preallocated area of at least size
 * \p len.
 *
 * \return number of bytes removed on success, or -1 on error.
 */
int chv_pop(chv_t * chv, unsigned char *buf, size_t len);

/*!
 * Drop (Forge/Remove) len bytes from \p chv.
 * \return number of bytes removed on success, or -1 on error.
 */
int chv_drop(chv_t * chv, size_t len);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Printing
 *
 * Print the contents in various ways.
 */

/* @{ */

void chv_fprint(FILE * stream, chv_t * chv);
void chv_print(chv_t * chv);

/*!
 * Print every single character in a chv_t as an ASCII-character if it is
 * printable, otherwise as a hexadecimal value. Useful when debugging.
 */
void chv_fprint_x(FILE * stream, chv_t * chv);

void chv_fprint_x_quoted_chars(FILE * stream, chv_t * chv);

void chv_print_x(chv_t * chv);

/*! Print the values of the members along with contents. */
void chv_fprintall(FILE * stream, chv_t * chv);

void chv_printall(chv_t * chv);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name File Saving and Loading.
 * They all return 0 on success, otherwise -1.
 */

/* @{ */

/*! Write \p chv to the file \p path. */
int chv_save(chv_t * chv, const char *path);

/*! Read \p chv from the file \p path. */
int chv_load(chv_t * chv, const char *path);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name \em Encode (Serialize) pointers to C types and some other structures.
 * They all return number of bytes written on success, or -1 on error.
 */
/* @{ */

int chv_enc_u8(chv_t * chv, const uint8_t * x);
int chv_enc_s8(chv_t * chv, const int8_t * x);

int chv_enc_u16be(chv_t * chv, const uint16_t * x);
int chv_enc_u16le(chv_t * chv, const uint16_t * x);
int chv_enc_s16be(chv_t * chv, const int16_t * x);
int chv_enc_s16le(chv_t * chv, const int16_t * x);

int chv_enc_u32be(chv_t * chv, const uint32_t * x);
int chv_enc_u32le(chv_t * chv, const uint32_t * x);
int chv_enc_s32be(chv_t * chv, const int32_t * x);
int chv_enc_s32le(chv_t * chv, const int32_t * x);

int chv_enc_u64be(chv_t * chv, const uint64_t * x);
int chv_enc_u64le(chv_t * chv, const uint64_t * x);
int chv_enc_s64be(chv_t * chv, const int64_t * x);
int chv_enc_s64le(chv_t * chv, const int64_t * x);

int chv_enc_f32be(chv_t * chv, const float * x);
int chv_enc_f32le(chv_t * chv, const float * x);

int chv_enc_f64be(chv_t * chv, const double * x);
int chv_enc_f64le(chv_t * chv, const double * x);

int chv_enc_sizebe(chv_t * chv, const size_t * x);
int chv_enc_sizele(chv_t * chv, const size_t * x);

/* ---------------------------- Group Separator ---------------------------- */

/*! Encode a byte buffer \p x of length (32-bit Big Endian) \p len into \p chv. */
static inline int chv_enc_cbuf_u32be(chv_t * chv, const unsigned char* buf, uint32_t len) {
  return (chv_enc_u32be(chv, &len) +
          chv_app_cbuf(chv, buf, len));
}

/*! Encode a byte buffer \p x of length (32-bit Little Endian) \p len into \p chv. */
static inline int chv_enc_cbuf_u32le(chv_t * chv, const unsigned char* buf, uint32_t len) {
  return (chv_enc_u32le(chv, &len) + chv_app_cbuf(chv, buf, len));
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Encode a byte buffer \p x of length (64-bit Big Endian) \p len into \p chv. */
static inline int chv_enc_cbuf_u64be(chv_t * chv, const unsigned char* buf, uint64_t len) {
  return (chv_enc_u64be(chv, &len) +
          chv_app_cbuf(chv, buf, len));
}

/*! Encode a byte buffer \p x of length (64-bit Little Endian) \p len into \p chv. */
static inline int chv_enc_cbuf_u64le(chv_t * chv, const unsigned char* buf, uint64_t len) {
  return (chv_enc_u64le(chv, &len) +
          chv_app_cbuf(chv, buf, len));
}

/* ---------------------------- Group Separator ---------------------------- */

/* @} */

/*!
 * \name \em Encode (Serialize) values of C types.
 * They all return number of bytes written on success, or -1 on error.
 */
/* @{ */
static inline int chv_enc_u8_v(chv_t * chv, uint8_t x) { uint8_t xU8 = x; return chv_enc_u8(chv, &xU8); }
static inline int chv_enc_bool_v(chv_t * chv, bool x) { uint8_t xU8 = x; return chv_enc_u8(chv, &xU8); }
static inline int chv_enc_u16be_v(chv_t * chv, uint16_t x) { uint16_t xU16 = x; return chv_enc_u16be(chv, &xU16); }
static inline int chv_enc_u16le_v(chv_t * chv, uint16_t x) { uint16_t xU16 = x; return chv_enc_u16le(chv, &xU16); }
static inline int chv_enc_u32be_v(chv_t * chv, uint32_t x) { uint32_t xU32 = x; return chv_enc_u32be(chv, &xU32); }
static inline int chv_enc_u32le_v(chv_t * chv, uint32_t x) { uint32_t xU32 = x; return chv_enc_u32le(chv, &xU32); }
static inline int chv_enc_u64be_v(chv_t * chv, uint64_t x) { uint64_t xU64 = x; return chv_enc_u64be(chv, &xU64); }
static inline int chv_enc_u64le_v(chv_t * chv, uint64_t x) { uint64_t xU64 = x; return chv_enc_u64le(chv, &xU64); }
static inline int chv_enc_f32be_v(chv_t * chv, float x) { float xF32 = x; return chv_enc_f32be(chv, &xF32); }
static inline int chv_enc_f32le_v(chv_t * chv, float x) { float xF32 = x; return chv_enc_f32le(chv, &xF32); }
static inline int chv_enc_f64be_v(chv_t * chv, double x) { double xF64 = x; return chv_enc_f64be(chv, &xF64); }
static inline int chv_enc_f64le_v(chv_t * chv, double x) { double xF64 = x; return chv_enc_f64le(chv, &xF64); }
static inline int chv_enc_sizebe_v(chv_t * chv, uint64_t x) { size_t xSIZE = x; return chv_enc_sizebe(chv, &xSIZE); }
static inline int chv_enc_sizele_v(chv_t * chv, uint64_t x) { size_t xSIZE = x; return chv_enc_sizele(chv, &xSIZE); }
/* @} */

/*!
 * \name Decoding.
 *
 * These functions Decode (Unserialize) Elementary C-types from a \c chv_t.
 *
 * They all return number of bytes read on success, or -1 on error.
 */
/* @{ */
/*! Decode a char \p x from \p chv. */
int chv_dec_u8(chv_t * chv, uint8_t *x);
int chv_dec_s8(chv_t * chv, int8_t *x);
/*! Decode an ushort \p x from \p chv. */
int chv_dec_u16be(chv_t * chv, uint16_t *x);
int chv_dec_u16le(chv_t * chv, uint16_t *x);
int chv_dec_s16be(chv_t * chv, int16_t *x);
int chv_dec_s16le(chv_t * chv, int16_t *x);
/*! Decode an uint \p x from \p chv. */
int chv_dec_u32be(chv_t * chv, uint32_t *x);
int chv_dec_u32le(chv_t * chv, uint32_t *x);
int chv_dec_s32be(chv_t * chv, int32_t *x);
int chv_dec_s32le(chv_t * chv, int32_t *x);
/*! Decode an uint64 \p x from \p chv. */
int chv_dec_u64be(chv_t * chv, uint64_t *x);
int chv_dec_u64le(chv_t * chv, uint64_t *x);
int chv_dec_s64be(chv_t * chv, int64_t *x);
int chv_dec_s64le(chv_t * chv, int64_t *x);
/*! Decode a float \p x from \p chv. */
int chv_dec_f32be(chv_t * chv, float *x);
int chv_dec_f32le(chv_t * chv, float *x);
/*! Decode a double \p x from \p chv. */
int chv_dec_f64be(chv_t * chv, double *x);
int chv_dec_f64le(chv_t * chv, double *x);
/*! Decode a size_t \p x from \p chv. */
int chv_dec_sizebe(chv_t * chv, size_t *x);
int chv_dec_sizele(chv_t * chv, size_t *x);
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*! Encode a C-style (zero-terminated) string \p x into \p chv. */
int chv_enc_cstr_lengthU32be(chv_t * chv, const char *str);
/*! Encode a C-style (zero-terminated) string \p x into \p chv. */
int chv_enc_cstr_lengthU32le(chv_t * chv, const char *str);

/*! Decode a C-style (zero-terminated) string \p x from \p chv. */
int chv_dec_cstr_lengthU32be(chv_t * chv, char **cstr_ret);
/*! Decode a C-style (zero-terminated) string \p x from \p chv. */
int chv_dec_cstr_lengthU32le(chv_t * chv, char **cstr_ret);
/*! Decode a C-style (zero-terminated) string \p x from \p chv. */
int chv_dec_cstr_lengthU32(chv_t * chv, char **cstr_ret, int little_endian_flag);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
