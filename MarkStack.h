/*!
 * \file MarkStack.h
 * \brief Stack of Text Markups (subset of XML,HTML).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-07-10 04:54
 *
 * Currently used to parse text with HTML-style.
 */

#pragma once

#include <stdlib.h>
#include "stdio_x.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

#define MSTACK_DEPTH (1024)	/**< Mark Stack Depth */

/*!
 * (Text) Mark
 *
 * Specifies the region of a (text) mark.
 */
typedef struct {
  size_t off;		       /**< Offset to Beginning of Marking. */
  size_t len;		       /**< Length of Marking. */
} Mark;

/*!
 * Mark Stack
 */
typedef struct {
  Mark marks[MSTACK_DEPTH];
  size_t markN;
} MStack;

/*!
 * Initialize Mark Stack \p mstack
 * \param[out] mstack Mark Stack
 */
void mstack_init(MStack * mstack);

/*!
 * Clear Mark Stack \p mstack
 * \param[in] mstack Mark Stack
 */
void mstack_clear(MStack * mstack);

/*!
 * Push Mark at \p off of length \p len onto Mark Stack \p mstack
 * \param[in,out] mstack Mark Stack
 */
int mstack_push(MStack * mstack, const Mark * mark);

/*!
 * Pop Top Mark from Mark Stack \p mstack into \p off_ret, \p len_ret
 * \param[in,out] mstack Mark Stack
 * \param[out] mark_ret
 */
int mstack_pop_mark(MStack * mstack, Mark * mark_ret);

/*!
 * Go forward to next symbol in \p buf of length \p buf_len at offset
   \p off.
 *
 * \param[in,out] mstack Mark Stack
 * \param[in] buf Buffer.
 * \param[in] off Offset into Buffer.
 * \param[in] buf_len Buffer Length.
 * \param[in] entry_ch is '<' for HTML.
 * \param[in] exit_ch is '>' for HTML.
 * \return 0 if End Of String was found.
 */
int mstack_next(MStack * mstack,
                const char * buf, size_t off, size_t buf_len,
                char entry_ch, char exit_ch);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
