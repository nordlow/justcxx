/*!
 * \file atrans.h
 * \brief Animation Transition Handling using lookup tables.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Frame 1 results in x = 1/sampN and lastfram results in x = 1.0f;
 *
 * Basic Usage:
 * - First call \c MATTabf_init() to set things up.
 * - Then lookup values with \c MATTabf_lookup() and
 * - When you are done call \c MATTabf_clear() free stuff.
 */

#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Animation Transition Type Code.
 */
typedef enum {
  ATRANS_LINEAR,
  ATRANS_COSSTEP,
  ATRANS_COSBUMP,
  ATRANS_SINSHAKE,
  ATRANS_RIPPEL
} ATRANS_t;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Determine whether transition type att is jerk, that is if its final
 * entry in the lookup table is (very close to) zero.
 */
static inline int
ATRANS_isJerk(ATRANS_t att)
{
  return ((att == ATRANS_COSBUMP) ||
	  (att == ATRANS_SINSHAKE));
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Animation Transition (Lookup) Table of float.
 */
typedef struct {
  ATRANS_t att;
  size_t sampN;			/**< Number of samples. */
  float * samps;		/**< Samples. */
} ATTabf;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Initialize Animation Transition (Lookup) Table \p attab.
 */
void
ATTabf_init(ATTabf * attab, ATRANS_t att, size_t sampN);

/*!
 * Lookup value at index \p idx in Animation Transition (Lookup) Table \p attab.
 */
float
ATTabf_lookup(ATTabf * attab, size_t idx);

/*!
 * Clear Animation Transition (Lookup) Table \p attab.
 */
void
ATTabf_clear(ATTabf * attab);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Multiple Animation Transition (Lookup) Table of float.
 */
typedef struct {
  ATTabf linear;		/**< Linear */
  ATTabf cosbump;		/**< Cosinus Bump */
  ATTabf cosstep;		/**< Cosinus Step */
  ATTabf sinshake;		/**< Sinus Shake */
  ATTabf rippel;		/**< Rippel */
} MATTabf;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Initialize Multiple Animation Transition (Lookup) Table \p mattab.
 */
static inline void
MATTabf_init(MATTabf * mattab, size_t sampN)
{
  ATTabf_init(&mattab->linear, ATRANS_LINEAR, sampN);
  ATTabf_init(&mattab->cosstep, ATRANS_COSSTEP, sampN);
  ATTabf_init(&mattab->cosbump, ATRANS_COSBUMP, sampN);
  ATTabf_init(&mattab->sinshake, ATRANS_SINSHAKE, sampN);
  ATTabf_init(&mattab->rippel, ATRANS_RIPPEL, sampN);
}

/*!
 * Lookup value at progress \p prg in
 * Multiple Animation Transition (Lookup) Table \p mattab.
 * \param[in] prg progress between within [0, 1].
 */
static inline float
MATTabf_lookup(MATTabf * mattab, ATRANS_t att, float prg)
{
  float y;
  switch (att) {
  case ATRANS_LINEAR:
    y = ATTabf_lookup(&mattab->linear, prg * mattab->linear.sampN);
    break;
  case ATRANS_COSSTEP:
    y = ATTabf_lookup(&mattab->cosstep, prg * mattab->cosstep.sampN);
    break;
  case ATRANS_COSBUMP:
    y = ATTabf_lookup(&mattab->cosbump, prg * mattab->cosbump.sampN);
    break;
  case ATRANS_SINSHAKE:
    y = ATTabf_lookup(&mattab->sinshake, prg * mattab->sinshake.sampN);
    break;
  case ATRANS_RIPPEL:
    y = ATTabf_lookup(&mattab->rippel, prg * mattab->rippel.sampN);
    break;
  default:
    y = 0;
    break;
  }
  return y;
}

/*!
 * Clear Multiple Animation Transition (Lookup) Table \p mattab.
 */
static inline void
MATTabf_clear(MATTabf * mattab)
{
  ATTabf_clear(&mattab->linear);
  ATTabf_clear(&mattab->cosstep);
  ATTabf_clear(&mattab->cosbump);
  ATTabf_clear(&mattab->sinshake);
  ATTabf_clear(&mattab->rippel);
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
