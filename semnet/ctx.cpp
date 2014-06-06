#include "ctx.hpp"

namespace semnet {
namespace patterns {

virtual
std::ostream& Ctx::show(std::ostream& os) const {
    Wrap::show(os);
    os << " ctx: " << m_ctx;
    Base::show_rand(os);
    return os;
}

Base*
Ctx::mutating_canonicalize()
{
    if (m_sub) {
        auto ctxS = dynamic_cast<Ctx*>(m_sub);
        if (ctxS and
            m_ctx == ctxS->m_ctx) { // \c this and its \c sub are same context
            return mutate_to(ctxS);
        }
    }
    return this;
}

pHit
Ctx::match_in(const char* buf, size_t len,
              bir roi) const
{
    return match_in_special(buf, len, roi);
}

}
}
