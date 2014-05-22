/*!
 * \file soundFXs.h
 * \brief Sound Effects (FXs).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Map to freedesktop XDG Base Directory Specification
 *       - http://standards.freedesktop.org/basedir-spec/basedir-spec-0.6.html
 *       - http://0pointer.de/public/sound-theme-spec.html
 *       - http://0pointer.de/public/sound-naming-spec.html
 */

#pragma once

#include "xstr.h"

#ifdef HAVE_CANBERRA_H
#  include "canberra.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * \name Pending Sound Effects to be sent to Audiolizer.
 */

/* @{ */

/*!
 * Sound Effect.
 */
typedef struct {
  float fq;			/**< Frequency [Hz] */
  float tdur_sec;		/**< Time Duration [s] */
  float azim;			/**< Azimuth Angle */
  float amp;			/**< Amplitude */
} SoundFX;

void soundFXs_init(void);

void soundFXs_clear(void);

/*!
 * Register Sound Effect.
 *
 * \return 1 upon success, 0 otherwise.
 */
void soundFXs_add(float fq, float tdur_sec, float azim, float amp);

void soundFXs_add_xstr(const xStr * effect_name);

/* @} */

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
