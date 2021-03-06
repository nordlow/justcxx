#include <limits>
#include <ostream>
#include "rep.hpp"
#include "ran.hpp"
#include "any.hpp"
#include "../saturate.hpp"
#include "../rangerand.h"
#include "../algorithm_x.hpp"
#include "../math_x.hpp"
#include "../show.hpp"
#include "../enforce.hpp"

namespace semnet {
namespace patterns {

void
Rep::update_sample_range() const
{
    m_irange.zero();
    auto sub_ssr = m_sub->sample_range();
    if (pnw::is_max(range().first)) { /* if minimum repetition is infinite */
        m_irange.low() = (sub_ssr.low()) ? std::numeric_limits<bix>::max() : bix(0);
    } else {
        m_irange.low() = sub_ssr.low().mul(range().first);
    }
    if (pnw::is_max(range().second)) { /* if maximum repetition is infinite */
        m_irange.high() = (sub_ssr.high()) ? std::numeric_limits<bix>::max() : bix(0);
    } else {
        m_irange.high() = sub_ssr.high().mul(range().second);
    }
}

size_t
Rep::complexity(bir mults) const
{
    return iter_pow(m_sub->complexity(mults),
                    static_cast<bix>(range().second));
}

Base::Skips8 &
Rep::intersect_skips(Skips8& skips) const
{
    return fixed() ? skips : m_sub->intersect_skips(skips);
}

csc
Rep::rand(bir ssr) const
{
#if 0
    char bufI[to_padded_byte(ssr.high())];
    bix sizeI = bix(0);

    PUNIFORMITY_t uniformity_F = PUNIFORMITY_NONE;

    /* minimum instance size for prep child */
    auto minI_C = m_sub->sample_range().first;

    size_t sub_maxM = 0; /* maximum allowed repetition count for the particular \c child */
    if (fixed()) {     /* if this repetition is fixed */
        sub_maxM = range().first; /* we just copy it */
    } else {                        /* if reptition is within a range */
        if (minI_C >= bix(1)) {            /* if nothing strange about range.low() for Rep CHILD */
            sub_maxM = std::min(range().first / minI_C,
                                range().second);
        } else {
            PWARN("Isn't this a strange pattern (minI_C is zero)?\n");
            sub_maxM = range().first;
        }
    }
    size_t instM = size_rangerand(range().first, sub_maxM); /* instance multiplicity */

    if (instM >= 1) {              /* if any repetitions */

        /* sub pattern minimum size */
        const auto min_q = ssr.low() / instM;
        /* sub pattern maximum size */
        const auto max_q = ssr.high() / instM;
        /* first or last pattern extra size */
        const bix max_r = (PUNIFORMITY_NONE) ? ssr.high() % instM : bix(0);

        if (uniformity_F == PUNIFORMITY_CONTENTS) {
            PERR("Handle this case!");
        } else {
            for (size_t i = 0; i < instM; i++) {
                const bix minI_c = min_q;

                /* current sub pattern max. first instance gets the resting size */
                const bix maxI_c = (i == 0) ? max_q + max_r : max_q;

                const csc bufS = m_sub->rand(bir(minI_c,
                                                 maxI_c));
                const bix retS = bix(8*bufS.size());
                memcpy(bufI + to_byte(sizeI), bufS.data(), to_byte(retS));  /* copy sequency child */
                sizeI += retS;
            }
        }
    }
    return csc(bufI, sizeI);
#endif
    return csc();
}

pHit
Rep::match_in_local(const char* buf, size_t len,
                    bir roi) const
{
    enforce_lte(roi.high(), to_bit(len));
    auto req = m_sub->fixed_rep_lmatch_in(req_count(), buf,len, roi); // required
    auto nopt = opt_count();               // number of optional bytes
    return (nopt and req.defined() ?
            append(req,
                   m_sub->fixed_rep_lmatch_in(nopt, buf,len, roi)) :
            req);
}

Base*
Rep::mutating_canonicalize()
{
    if (m_sub and
        fixed()) {
        if (auto any = dynamic_cast<Any*>(m_sub)) {
            const auto num = req_count();
            return this->mutate_to((num == 1) ?
                                   any :
                                   any->mutate_to(anybits(num * any->bitsize())));
        }
    }
    return this;
}

std::ostream &
Rep::show(std::ostream& os) const
{
    Base::show(os);
    os << "Rep:#" << m_rrange << "-instance: ";
    Base::show_rand(os);
    return os;
}

namespace gen {

Rep * repREchar(Base * sub, const char re_char)
{
    Rep * ret = nullptr;
    switch (re_char) {
    case '*': ret = rep0oM(sub); break;
    case '?': ret = rep0o1(sub); break;
    case '+': ret = rep1oM(sub); break;
    default:  ret = rep0o1(sub); break; /* NOTE: default to 0 or 1 */
    }
    return ret;
}

}
}
}
