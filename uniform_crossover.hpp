/*!
 * \file uniform_crossover.hpp
 * \brief Uniform Crossover.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-12-03 13:32
 */

#pragma once

#include <vector>

namespace pnw
{

// ============================================================================

/*! Bitwise Uniform (Genetic) Crossover.
 *
 * Return \c a where each bit in \p a is set to the corresponding bit in either
 * \p b or \p c depending on the corresponding bit in \p d (which is normally
 * randomized)
 *
 * Used in \em Genetic Algorithms to \em mate data or code (solutions).
 * \see http://en.wikipedia.org/wiki/Crossover_(genetic_algorithm)
 */
template<typename T>
inline spure
T uniform_crossover_bits(const T & b, const T & c, const T & d)
{
    return ((b ^ c) bitand d) bitor (b bitand c);
}

// ============================================================================

}
