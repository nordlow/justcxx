#include "know_langCxx.hpp"
#include "seq.hpp"
#include "rep.hpp"
#include "lang.hpp"
#include "lit.hpp"

#include <cstdlib>

namespace semnet {
namespace patterns {

Lit * gp_Cxx_declaration_list = nullptr;
Lang* gp_Cxx_lang = nullptr;
Lang* gp_Cxx_11_lang = nullptr;

Lang* learn_Cxx_lang()
{
    using namespace gen;
    if (gp_Cxx_lang) { return gp_Cxx_lang; }
    gp_Cxx_declaration_list = lit("C++ Dummy Literal");
    gp_Cxx_11_lang = lang("C++11", gp_Cxx_declaration_list);
    return gp_Cxx_lang = lang("C++", gp_Cxx_declaration_list);
}

}
}
