/*!
 * \file pitch.hpp
 * \brief Musical Pitches.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-01-27 13:58
 * \see solfeggio.hpp
 * \see https://en.wikipedia.org/wiki/A440_%28pitch_standard%29
 */

#pragma once

/*!
 * \em Musical Pitches in Hertz.
 */
typedef enum __attribute__ ((packed)) {
    PITCH_KAMMERTON_C  = 415;   ///< Baroque pitch for strings.
    PITCH_CLASSICAL_C  = 430;   ///< Classical.
    PITCH_STD_C  = 440;         ///< Ben Johnston's.
    PITCH_CHORTON_C  = 466;     ///< Church Voices (Chorton).
} PITCH_t;
