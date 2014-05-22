/*!
 * \file ra_srp.hpp
 * \brief Resolution-Adaptive Steeered Response Power (RA-SRP).
 */

#pragma once

/*!
 * Calculate Steeered Response Power (RA-SRP).
 *
 * \param ss Sensors Signals
 * \param sp Sensors Positions [m]
 * \param sv Sensor Propagation Velocities [m/s]
 *
 * \param et Emitter Trajectory-(Polynomial)
 *
 * \param sr Sample Rate (Resolution) [Hz]
 */
template<class T = double>
T evaluate_RA_SRP(const T & ss,
                  const T & sp, const T & sv, const T & traj,
                  const T & sr)
{
    double srp = 0;
    // TODO Calculate \c dists from \p sp and \p et
    return srp;
}

/*!
 * Search for Best Steeered Response Power (RA-SRP) using Sample-Rate (Resolution) Adaptive Sampling.
 *
 * \param etr Emitter Trajectory Range
 * \param evr Emitter Absolute Velocity Range (Min/Max) [m/s]
 * \param srr Sample Rate (Resolution) Range [Hz]
 */
template<class T = double>
T search_best_RA_SRP(const T & ss,
                     const T & sp, const T & sv,
                     const T & etr,
                     const T & evr,
                     const T & srr)
{
    return evaluate_RA_SRP();
}
