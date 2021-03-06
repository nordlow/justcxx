/*!
 * \file know_langCxx.hpp
 * \brief semnetDB Apriori Patterns for C++ Language
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-03-18 16:19
 */

#pragma once

namespace semnet {
namespace patterns {

class Rep;
class Seq;
class Base;
class Lang;
class Lit;

/* ---------------------------- Group Separator ---------------------------- */

extern Lit * gp_Cxx_declaration_list;
extern Lang* gp_Cxx_lang;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Load a Pattern matching \em C++ Language.
 */
Lang* learn_Cxx_lang();

}
}
