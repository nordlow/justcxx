/*! \file fkind.h
 * \brief Directory/File Kind (Format).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://en.wikipedia.org/wiki/Alphabetical_list_of_file_extensions
 */

#pragma once
#include <unistd.h>
#include "xstr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * File Class Type Code.
 */
typedef enum {
  FKIND_any_,                   /**< Any of the following. */

  FKIND_REGULAR,                /**< Regular File of unknown content. */

  FKIND_DIR,                    /**< Directory. */

  /** \name Special Files */
  /* @{ */
  FKIND_SPECIAL_LNK,            /**< Link. */
  FKIND_SPECIAL_LNK_SOFT,       /**< Soft-Link. */
  FKIND_SPECIAL_LNK_HARD,       /**< Hard-Link. */
  FKIND_SPECIAL_CHR,            /**< Character Device. */
  FKIND_SPECIAL_BLK,            /**< Block Device. */
  FKIND_SPECIAL_FIFO,           /**< FIFO. */
  FKIND_SPECIAL_SOCK,           /**< Socket. */
  /* @} */

  FKIND_undefined_,           /**< Undefined. */
} __attribute__ ((packed)) FKIND_t;

/*!
 * Create from \c mode.
 */
FKIND_t FKIND_from_stat_mode(mode_t mode);

/*!
 * Lookup name of \p fkind as a constant static string.
 * \return the looked up name.
 */
const char * FKIND_getName(FKIND_t fkind);

/*! Check if \p fkind is a SPECIAL */
static inline int FKIND_is_SPECIAL(FKIND_t fkind)
{
  return (fkind == FKIND_SPECIAL_LNK ||
          fkind == FKIND_SPECIAL_LNK_SOFT ||
          fkind == FKIND_SPECIAL_LNK_HARD ||
          fkind == FKIND_SPECIAL_CHR ||
          fkind == FKIND_SPECIAL_BLK ||
          fkind == FKIND_SPECIAL_FIFO ||
          fkind == FKIND_SPECIAL_SOCK);
}

/*! Check if \p fkind is a regular */
static inline int FKIND_is_REGULAR(FKIND_t fkind)
{
  return (fkind >= FKIND_REGULAR);
}

#ifdef __cplusplus
}
#endif
