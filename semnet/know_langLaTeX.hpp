/*!
 * \file know_langLaTeX.hpp
 * \brief Semnet Apriori Patterns for LaTeX Documents Setup Language.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

namespace semnet {
namespace patterns {

class Lang;
extern Lang* gp_LaTeX_lang;

/*! Load a Pattern matching the \em LaTeX Language. */
Lang* learn_LaTeX_lang();

}
}
