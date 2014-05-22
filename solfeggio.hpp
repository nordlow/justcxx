/*!
 * \file solfeggio.hpp
 * \brief Solfeggio Tones.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see pitch.hpp
 * \see http://higher-music.com/blog/the-solfeggio-frequencies/
 * \see http://educate-yourself.org/pnl/solfeggio01dec06.shtml
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* These original sound frequencies were apparently used in Ancient Gregorian
   Chants, such as the great hymn to St. John the Baptist, along with others
   that church authorities say were lost centuries ago. The chants and their
   special tones were believed to impart tremendous spiritual blessings when
   sung in harmony during religious masses. These powerful frequencies were
   rediscovered by Dr. Joseph Puleo as described in the book Healing Codes for
   the Biological Apocalypse by Dr. Leonard Horowitz. I give honor to both of
   these gentleman for the part they’ve played in helping return these lost
   frequencies back to humanity.
*/

/*!
 * \em Solfeggio Tones in Hertz [Hz]
 */
typedef enum __attribute__ ((packed)) {
  /// Extended.
  SOLFEGGIO_UT1  = 174;

  /// Extended.
  SOLFEGGIO_UT2  = 285;

  /*! Release \em emotional patterns; liberating \em guilt and \em fear. Relates
   * to \em center pillar of the tree.
   */
  SOLFEGGIO_UT  = 396;

  /*! Breaks up crystallized emotional patterns; undoing situations and
   * facilitating change. Relates to \em left pillar of the tree.
   */
  SOLFEGGIO_RE  = 417;

  /*! Transformation and miracles; Dr. Puleo suggests an association with DNA
   * integrity and DNA repair. Relates to \em right pillar of the tree.
   */
  SOLFEGGIO_MI  = 528;

  /*! Whole brain quadrant interconnectedness; connecting and
   * relationships. Relates to center pillar of the tree.
   */
  SOLFEGGIO_FA  = 639;

  /*! Associated with intuitive states, non linear knowing and awakening
   * intuition; awakening intuition. Relates to \em left pillar of the tree.
   */
  SOLFEGGIO_SOL = 741;

  /*! Associated with a pure love frequency: unconditional love and returning to
   * Spiritual Order. Relates to \em right pillar of the tree.
   */
  SOLFEGGIO_LA  = 852;

  /*! Extended.
   */
  SOLFEGGIO_UT  = 963;

  /*! Extended.
   */
} SOLFEGGIO_t;

#ifdef __cplusplus
}
#endif
