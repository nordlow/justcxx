/*!
 * \file SFILL_enum.h
 * \brief Shape Fill Type Code
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-09-10 13:40
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Shape <em>Fill</em> Type Code.
 */
typedef enum {
  SFILL_SINGLE,			/**< Single Color. */

  SFILL_GRADV,			/**< Vertical (Linear) Gradient. */
  SFILL_GRADH,			/**< Horizontal (Linear) Gradient. */
  SFILL_GRADR,			/**< Radial (Linear) Gradient. */
  SFILL_GRADA,			/**< Angular (Linear) Gradient. */

  SFILL_PALV,			/**< Vertical (Linear) Palette. */
  SFILL_PALH,			/**< Horizontal (Linear) Palette. */
  SFILL_PALR,			/**< Radial (Linear) Palette. */
  SFILL_PALA,			/**< Angular (Linear) Palette. */

  SFILL_undefined_,        /**< Undefined Filling. */
} __attribute__ ((packed)) SFILL_t;

static inline
int SFILL_is_GRAD(SFILL_t sfill)
{
  return (sfill == SFILL_GRADV ||
	  sfill == SFILL_GRADH ||
	  sfill == SFILL_GRADR ||
	  sfill == SFILL_GRADA);
}

static inline
int SFILL_is_PAL(SFILL_t sfill)
{
  return (sfill == SFILL_PALV ||
	  sfill == SFILL_PALH ||
	  sfill == SFILL_PALR ||
	  sfill == SFILL_PALA);
}

#ifndef NDEBUG
extern const char * g_shape_warn_sfill_nodef;
extern const char * g_shape_warn_sfill;
#endif

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
