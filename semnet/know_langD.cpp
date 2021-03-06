#include "know_langC.hpp"
#include "know_langD.hpp"
#include "lang.hpp"
#include "rep.hpp"

namespace semnet { namespace patterns {

Lang* gp_D_lang = nullptr;

Lang* learn_D_lang()
{
    if (gp_D_lang) { return gp_D_lang; }
    return gp_D_lang = gen::lang("D", C::gp_declaration_list); // superset of C
}

}}
