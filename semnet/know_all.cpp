#include "know_all.hpp"
#include "alt.hpp"
#include "lang.hpp"

namespace semnet {
namespace patterns {

void learn_all()
{
    using namespace semnet::patterns::gen;
    learn_common();
    C::learn_C98_lang();
    learn_Cxx_lang();
    learn_LaTeX_lang();
    gp_Computer_lang = lang("Computer Languages",
                            alt(C::gp_lang,
                                gp_Cxx_lang,
                                gp_LaTeX_lang));
    learn_dfmts();
    learn_dfmts_dirtypes();
}

}
}
