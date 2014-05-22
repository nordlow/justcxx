/*!
 * \file crossover.hpp
 *
 * \brief Functions Calculating Various Kinds Of Crossovers mainly
 * used in Genetic Programming (GP).
 *
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <cstdlib>

/*!
 * Perform a Non-Cyclic Two-Way Random Crossover between the two
 * parent arguments \p p0 and \p p1 producing the offspring (children)
 * in the return arguments \p c0 and \p c1.
 */
template<typename T>
void noncyclic_twoway_random_crossover(const T & p0,
				       const T & p1,
				       T & c0,
				       T & c1)
{
    // read sizes
    const size_t s0 = p0.size();
    const size_t s1 = p1.size();

    // splitting indexes are selected at random
    const uint i0 = rand() % s0;
    const uint i1 = rand() % s1;

    c0.resize(i0 + s1-i1);
    c1.resize(i1 + s0-i0);

    // 1:st child
    std::copy(p0.begin(), p0.begin() + i0, // 1:st parent 1:st half to
	      c0.begin());	           // 1:st child  1:st half
    std::copy(p1.begin() + i1, p1.end(),   // 2:nd parent 2:nd half to
	      c0.begin() + i0);	           // 1:st child  2:nd half

    // 2:nd child
    std::copy(p0.begin() + i0, p0.end(),   // 1:st parent 2:nd half to
	      c1.begin());	           // 2:nd child  1:st half
    std::copy(p1.begin(), p1.begin() + i1, // 2:nd parent 1:st half to
	      c1.begin() + s0-i0);         // 2:nd child  2:nd half
}
