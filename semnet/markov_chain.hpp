/*!
 * \file markov_chain.hpp
 * \brief Markov Chain
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-10-25 21:24
 */

#pragma once

#include "spatt.hpp"

namespace semnet {
namespace patterns {

/*! Markov Chain
 *
 * Train to produce n-grams etc.
 */
class MarkovChain : public Base {
public:
    MarkovChain() : Base() { init(); }
    MarkovChain(Const Char * name) : Base(name) { }
private:
};

}
}
