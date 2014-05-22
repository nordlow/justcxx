/*!
 * \file CADJ.h
 * \brief Container Adjustments.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Container Grow/Shrink \em Adjustment Hints.
 *
 * Used by objects that spatially enclose its \c REL_CHILDREN / \c REL_SLAVE objects
 *
 * For a scrolled container this typically means that two kinds of \c CADJ_t are
 * needed
 * - growth/shrink of the container extents
 * - pan/zoom of the current window position and size of the container
 */

/*!
 * \name Container Adjustment Hint Masks.
 */
/* @{ */
#define CADJ_NONE         (0)	/**< No adjustments. */

#define CADJ_LEFT_GROW    (1 << 0) /**< Grow Left Side with content. */
#define CADJ_RIGHT_GROW   (1 << 1) /**< Grow Right Side with content. */
#define CADJ_TOP_GROW     (1 << 2) /**< Grow Top Side with content. */
#define CADJ_BOT_GROW     (1 << 3) /**< Grows Bottom Side with content. */

#define CADJ_LEFT_SHRINK  (1 << 4) /**< Shrink Left Side with content. */
#define CADJ_RIGHT_SHRINK (1 << 5) /**< Shrink Right Side with content. */
#define CADJ_TOP_SHRINK   (1 << 6) /**< Shrink Top Side with content. */
#define CADJ_BOT_SHRINK   (1 << 7) /**< Shrink Down Side with content. */
/* @} */

/*!
 * Container Adjustment Hint Type Code.
 */
typedef uint8_t CADJ_t;

/*!
 * Modify the cadj \p a with the new cadj \p b.
 * \return the modified cadj
 */
static inline CADJ_t CADJ_set(CADJ_t a, CADJ_t b) { return a | b; }

/*!
 * Modify the cadj \p a with the new cadj \p b.
 * \return the modified cadj
 */
static inline CADJ_t CADJ_clr(CADJ_t a, CADJ_t b) { return a & ~b; }

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
