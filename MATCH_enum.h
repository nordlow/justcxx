/*! \file MATCH_enum.h
 * \brief Literal Matching Types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-10-22 09:39
 */

#pragma once
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! Pattern Matching \em Genre Type Code. */
typedef enum {
  MATCH_FULL,                  /**< \em Full Word Match (at offset of given length): Prefix and Suffix */
  MATCH_COMPLETE = MATCH_FULL,
  MATCH_WHOLE    = MATCH_FULL,
  MATCH_first    = MATCH_FULL,

  MATCH_PREFIX,                /**< \em Prefix (Forward) Word Match (after offset) */
  MATCH_SUFFIX,                /**< \em Suffix (Backward) Word Match (before offset) */

  MATCH_PARTIAL,               /**< \em Partial Word (Sub-String) Match (anywhere after offset) */
  MATCH_SUBSET   = MATCH_PARTIAL,
  MATCH_DEFAULT  = MATCH_PARTIAL, /**< \em Default Pattern Matching Types. */

  MATCH_ACRONYM,               /**< \em Acronym (or Flexible) Match. Match characters in the given order possibly non-adjacently. */
  MATCH_FLEXIBLE = MATCH_ACRONYM,

  MATCH_REGEXP,                /**< \em Regular Expression Match. */

  MATCH_FUZZY,                 /**< \em Fuzzy Match. */
  MATCH_last    = MATCH_FUZZY,

  MATCH_undefined_,            /**< \em Undefined. */

  MATCH_num      = MATCH_undefined_ /**< \em Number of Pattern Matching Types. */
} __attribute__((packed)) MATCH_t;

#ifdef __cplusplus
}
#endif
