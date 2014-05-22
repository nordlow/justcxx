/*!
 * \file pso.hpp
 * \brief Particle Swarm Optimization.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-11 00:18
 *
 * \see http://www.scholarpedia.org/article/Particle_swarm_optimization
 * \see https://en.wikipedia.org/wiki/Particle_swarm_optimization
 * \see https://en.wikipedia.org/wiki/Swarm_intelligence
 * \see SwarmIntelligence.pdf
 *
 * A particle p is described by:
 * - \c s its position; e.g.: \c x, \c y
 * - \c v its velocity; e.g. (for discrete case) angle and distance of next step
 * - \c f its performance; e.g.: value of the function at its location
 *
 * A particle perceives
 * - performances, Y=f(x), and
 * - positions, x,
 * of its neighboring particles.
 *
 * It can also tell which is the best particle among its neighbors (\c gbest)
 *
 * A particle remembers the position where it obtained the best performance so
 * far (\c pbest). Its Global Minimum.
 *
 * Initialization
 * - Swarm size: Typically 20 particles for problems with dimensionality 2 - 200
 * - Initial position of each particle: Random
 * - Neighborhood topology: Global, geographical or social (list based)
 * - Neighborhood size: Typically 3 to 5
 * - Set max velocity to v_max; if v(t+1) is larger, clip it to v_max
 *
 * Iterate until best solution is found or no further improvement.
 */

#pragma once

/*! Particle Swarm Optimization. */
template<class T>
inline
void pso()
{
}
