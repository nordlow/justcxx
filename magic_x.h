/*! \file magic_x.h
 * \brief Database for libmagic.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-06-20 17:13
 */

#pragma once
#include <inttypes.h>
#ifdef HAVE_MAGIC_H
#  include <magic.h>
#  endif

typedef uint32_t magicDB_index_t;		/**< Precision for magic indexing. */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_MAGIC_H
int
magic_open_and_load_stubborn(magic_t * cookie, int flags);
#endif

/*! Initialize Magic Database. */
int magicDB_init(void);

/*! Register string \p magic_str in Magic Database and return an index. */
magicDB_index_t magicDB_reg(const char * magic_str);

/*! Lookup C-string at index \p i in Magic Database and return it.
 * Time complexity is O(1) for fast drawing.
 */
const char * magicDB_lookup(magicDB_index_t i);

/*! Clear Magic Database. */
void magicDB_clear(void);

/*! Print Magic Database. */
void magicDB_print(void);

/*!
 * Get file magic from \p fname.
 */
const char * magicDB_get(const char * fname);

#ifdef __cplusplus
}
#endif
