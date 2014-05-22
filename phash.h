/*!
 * \file phash.h
 * \brief Hash Table Wrappers.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-02-05 13:36
 *
 * Provides Wrappers for C Primitives Types.
 */

/* ========================================================================= */

#pragma once

#if defined(HAVE_CONFIG_H)
#  include "config.h"
#endif

#include "pnw_types.h"

#define HAVE_GHT_HASH_TABLE_H (1) /* Always present now that we include libghthash in this directory. */
#  include "libghthash/src/ght_hash_table.h" /* Note: Use local variant. */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

static inline void *phash_get_cbuf(ght_hash_table_t *p_ht,
                                   unsigned int i_key_size, const void *p_key_data)
{
  return ght_get(p_ht, i_key_size, p_key_data);
}

/* pointer */
static inline void *phash_get_at_ptr(ght_hash_table_t * p_ht, void* key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_ptr(ght_hash_table_t * p_ht, void* data, void* key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_ptr(ght_hash_table_t * p_ht, void* key) { return ght_remove(p_ht, sizeof(key), &key); }

/* char */
static inline void *phash_get_at_char(ght_hash_table_t * p_ht, char key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_char(ght_hash_table_t * p_ht, void * data, char key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_char(ght_hash_table_t * p_ht, char key) { return ght_remove(p_ht, sizeof(key), &key); }

/* short */
static inline void *phash_get_at_short(ght_hash_table_t * p_ht, short key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_short(ght_hash_table_t * p_ht, void * data, short key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_short(ght_hash_table_t * p_ht, short key) { return ght_remove(p_ht, sizeof(key), &key); }

/* int */
static inline void *phash_get_at_int(ght_hash_table_t * p_ht, int key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_int(ght_hash_table_t * p_ht, void * data, int key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_int(ght_hash_table_t * p_ht, int key) { return ght_remove(p_ht, sizeof(key), &key); }

/* float */
static inline void *phash_get_at_float(ght_hash_table_t * p_ht, float key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_float(ght_hash_table_t * p_ht, void * data, float key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_float(ght_hash_table_t * p_ht, float key) { return ght_remove(p_ht, sizeof(key), &key); }

/* double */
static inline void *phash_get_at_double(ght_hash_table_t * p_ht, double key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_double(ght_hash_table_t * p_ht, void * data, double key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_double(ght_hash_table_t * p_ht, double key) { return ght_remove(p_ht, sizeof(key), &key); }

/* long double */
static inline void *phash_get_at_ldouble(ght_hash_table_t * p_ht, ldouble key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_ldouble(ght_hash_table_t * p_ht, void * data, ldouble key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_ldouble(ght_hash_table_t * p_ht, ldouble key) { return ght_remove(p_ht, sizeof(key), &key); }

/* ========================================================================= */

/* int8_t */
static inline void *phash_get_at_int8(ght_hash_table_t * p_ht, int8_t key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_int8(ght_hash_table_t * p_ht, void * data, int8_t key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_int8(ght_hash_table_t * p_ht, int8_t key) { return ght_remove(p_ht, sizeof(key), &key); }

/* int16_t */
static inline void *phash_get_at_int16(ght_hash_table_t * p_ht, int16_t key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_int16(ght_hash_table_t * p_ht, void * data, int16_t key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_int16(ght_hash_table_t * p_ht, int16_t key) { return ght_remove(p_ht, sizeof(key), &key); }

/* int32_t */
static inline void *phash_get_at_int32(ght_hash_table_t * p_ht, int32_t key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_int32(ght_hash_table_t * p_ht, void * data, int32_t key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_int32(ght_hash_table_t * p_ht, int32_t key) { return ght_remove(p_ht, sizeof(key), &key); }

/* int64_t */
static inline void *phash_get_at_int64(ght_hash_table_t * p_ht, int64_t key) { return ght_get(p_ht, sizeof(key), &key); }
static inline int phash_insert_at_int64(ght_hash_table_t * p_ht, void * data, int64_t key) { return ght_insert(p_ht, data, sizeof(key), &key); }
static inline void *ght_remove_int64(ght_hash_table_t * p_ht, int64_t key) { return ght_remove(p_ht, sizeof(key), &key); }

/* ========================================================================= */

/*!
 * \em Coarse-Grained Rehashing of \p p_ht.
 *
 * Note: this is exactly what \c ght_set_rehash() does for
 * automatically but we want this only in idle times.
 * \return 1 if rehashing was performed, 0 otherwise.
 */
static inline int ght_rehash_coarse(ght_hash_table_t * p_ht)
{
  int ret = 0;
  unsigned int num = ght_size(p_ht);
  if        (num > 2 * ght_table_size(p_ht)) {
    ght_rehash(p_ht, num); /* expand-fit */
    ret = 1;
  } else if (2 * num < ght_table_size(p_ht)) {
    ght_rehash(p_ht, num); /* shrink-fit */
    ret = 1;
  }
  return ret;
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
