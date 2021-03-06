#include "know_common.hpp"
#include "seq.hpp"
#include "rep.hpp"
#include "alt.hpp"
#include "any.hpp"
#include "lit.hpp"
#include "sit.hpp"
#include "../saturate.hpp"
#include "../algorithm_x.hpp"
#include "../enforce.hpp"
#include "pmatch.hpp"
#include <algorithm>

/* ---------------------------- Group Separator ---------------------------- */

namespace semnet {
namespace patterns {

virtual std::ostream&
Seq::show(std::ostream& os) const {
    Base::show(os);
    os << " cacheA:" << m_cacheA.get()
       << " cacheL:" << m_cacheL;
    Base::show_rand(os);
    return os;
}

void
Seq::prepare() const
{
    Super::prepare();
    if (not m_cacheA.get()) {
        /* find child patterns */
        // TODO: Check homoF from net_find_rToT() and create m_cacheA
    }
    // TODO: Where possible insert pAny's' between adjacent related Sits with same SIT-type in m_subs, typicall PRPOS_BOB
}

void
Seq::unprepare() const
{
    Super::unprepare();
    m_cacheA.reset();
}

/* ---------------------------- Group Separator ---------------------------- */

Base *
Seq::add(Base * sub)
{
    m_subs.push_back(sub); sub->add_super(this);
    unprepare();
    return sub;
}

Base *
Seq::add_at_BOB(Base * sub, bix off)
{
    using namespace gen;
    if (empty()) {              // if no arguments yet
        add(bob());
        if (off) {              // if offset
            add(anybits(off)); // we express it with padding
        }
        add(sub);               // finally add it
    } else {
        auto first = dynamic_cast<Sit*>(*begin(m_subs));
        if (first and
            first->value() == PRPOS_BOB) {
            auto ssr = this->sample_range();
            if (ssr.high() < off) {    // if \p this doesn't reach \c off
                add(anybits(off - ssr.high())); // pad it to \c off
            } else {
                enforce_eq(ssr.high(), off); // check sequence size
            }
        } else {
            PERR("Seq has no defined PRPOS?\n");
        }
        unprepare();
    }
    return sub;
}

void
Seq::add(Base ** subs, size_t n)
{
    m_subs.insert(end(m_subs), subs, subs+n); // downlinks
    std::for_each(subs, subs+n,                  // C++11
                  [=] (Base * sub) {
                      sub->add_super(this); // uplink
                  });
    unprepare();
}

/* ---------------------------- Group Separator ---------------------------- */

void
Seq::update_sample_range() const
{
    prepare();
    m_irange.zero();
    for (auto sub : m_subs) {
        m_irange.append(sub->sample_range());
    }
}

size_t
Seq::complexity(bir mults) const
{
    prepare();
    sat_size_t prod = sat_size_t(1);
    for (auto sub : m_subs) {   // \todo Use for_each lambda expression
        prod *= sub->complexity(mults);
    }
    return prod.get();
}

/* ---------------------------- Group Separator ---------------------------- */

#include <vector>

csc
Seq::rand(bir ssr) const
{
    prepare();

    if (m_subs.size() >= 1) {
        /* statistics of subs */
        std::vector<bir> ssrs(m_subs.size()); /* array of sample size ranges for subs */
        bir ssrU;           /* range union of instance maximum sizes for subs */
        bix numG = bix(0);       /* number of subs that are growable */

        /* pre-calculate sub limits */
        size_t i = 0;
        for (auto& sub : m_subs) {
            ssrU.include(ssrs[i] = sub->sample_range());
            if (ssrs[i].bitsize() >= 8) { numG += 8; }
            i++;
        }

        /* number of \em optional bytes we can spend on \em all the \em growable
           subren of \p psec to make a combuence that is exactly \p ssr.high() long */
        const auto extG = ssr.bitsize();

        /* on average the \em least number of bytes we can afford to \em spend on \em each growable sub */
        const auto extG_mean = numG > 0 ? extG / numG : bix(0) ;

        size_t j = 0;
        csc ret;
        for (auto& sub : m_subs) { // for each sub roi
            auto sr = ssrs[j];
            if (sr.defined() and    // if sub instance is defined and
                not sr.is_zero()) { // non-empty
                /* number of bytes that \em instance can \em grow, \c bix::inf()
                   if it can grow infinitely */
                auto incG_i = (sr.high() == bix::inf()) ? sr.high() : sr.bitsize();
                ret.append(sub->rand(bir(sr.low(),
                                         sr.low() + std::min(incG_i, extG_mean))));  /* append sub instance */
            }
            j++;
        }
        return ret;
    }
    return csc();               // undefined
}

/* ---------------------------- Group Separator ---------------------------- */

Base::Skips8 &
Seq::intersect_skips(Skips8& skips) const
{
    for (auto sub : m_subs) {
        if (sub->intersect_skips(skips).none()) { // if not bits left
            return *m_skips8.get();     // no use in further reducing
        }
    }
    return *m_skips8.get();
}

/* ---------------------------- Group Separator ---------------------------- */

pHit
Seq::match_in_local(const char* buf, size_t len,
                    bir roi) const
{
    prepare();
    auto off = roi.low();     // offset iterator
    pHit hit;
    for (auto sub : m_subs) { /* for each \c CHILD */
        bir roiS(off, roi.high());
        auto hitS = sub->match_in(static_cast<csc>(buf), roiS); // sub hit
        if (hitS.full()) { /* exact match */
            auto lenS = hitS.bitlength();
            if (lenS != 0) {
                if (len >= lenS) {
                    off += lenS;       /* advance in buffer */
                    hit.app(hitS); /* append to total match */
                } else {
                    enforce_lt(len, hitS.bitlength());
                }
            }
        } else {
            return hit;
        }
    }
    return hit;
}

Base *
Seq::mutating_canonicalize()
{
    Base * ret = this;
    // TODO: Replace Seqs in m_subs with their subs inplace. use replace_with
    PTODO("Convert serveral Sit(BOB, N_i) to Sit(BOB), Any(N_j - N_i), Sit(BOB)\n");
    return ret;
}

}
}
