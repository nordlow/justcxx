#include "alt.hpp"
#include "file.hpp"
#include "lit.hpp"
#include "../phash.h"
#include "pmatchers.hpp"
#include "../saturate.hpp"
#include "../find_prefix.hpp"
#include "../rangerand.h"
#include "../algorithm_x.hpp"
#include <algorithm>
#include "../memory_x.hpp"
#include <boost/unordered_map.hpp>

namespace semnet {
namespace patterns {

virtual std::ostream& Alt::show(std::ostream& os) const {
    Multi::show(os);
    Multi::show_rand(os);
    return os;
}

Alt::Alt(uint8_t a, uint8_t b, bool complement)
    : Multi(), m_hashA(nullptr), m_complement(complement)
{
    add(gen::lit(a),
        gen::lit(b));
}

Alt::Alt(const char* datA[], const size_t sizeA[], size_t num) {
    for (size_t j = 0; j != num; j++) { // for all array elements
        add(gen::lit(reinterpret_cast<const unsigned char*>(datA[j]), sizeA[j]));
    }
}
Alt::Alt(const char * datA[], size_t num) {
    for (size_t j = 0; j != num; j++) { // for all array elements
        add(gen::lit(reinterpret_cast<const char*>(datA[j])));
    }
}

void
Alt::update_sample_range() const
{
    m_irange.undefine();
    for (auto sub : m_subs) {
        m_irange.include(sub->sample_range());
    }
}

void
Alt::prepare() const
{
    Super::prepare();
    if (not m_hashA) {      // if not yet hashed
        /* filter child literals */
        std::vector<std::pair<csc, Base*> > tmp(m_subs.size());
        size_t cnt = 0;             /* number constant subs */
        for (auto sub : m_subs) {
            auto key = sub->constant();
            if (key.empty()) {
                break;
            } else {
                tmp[cnt].first = key;
                tmp[cnt++].second = sub;
            }
        }

        /* if possible hash child literals */
        if (m_subs.size() == cnt) { /* if \em all \em sub-patterns are \em literals */
            m_hashA = std::make_unique<Hashes>(cnt);
            for (size_t i = 0; i < cnt; i++) { /* for all alternatives */
                m_hashA->insert(tmp[i]); /* insert alternative into hash */
            }
        }
    }
}

void
Alt::unprepare() const
{
    Super::unprepare();
    m_hashA.reset();
}

Base *
Alt::add(Base * sub)
{
    m_subs.insert(sub); sub->add_super(this);
    unprepare();
    return sub;
}

void
Alt::add(Base ** subs, size_t n)
{
    std::for_each(subs, subs+n, // C++11
                  [&] (Base * sub) {
                      m_subs.insert(sub); // downlink
                      sub->add_super(this); // uplink
                  });
    unprepare();
}

size_t
Alt::complexity(bir mults) const
{
    sat_size_t sum = sat_size_t(0);
    for (auto sub : m_subs) {   // \todo Use for_each lambda expression
        sum += sub->complexity(mults);
    }
    return sum.get();
}

Base*
Alt::hash_match(const char* buf, size_t len) const
{
    auto hit = m_hashA->find(csc(buf, len));
    return hit != m_hashA->end() ? hit->second : nullptr;
}

pHit
Alt::match_in_local(const char * buf, size_t len,
                    bir roi) const
{
    enforce_lte(roi.high(), len);

    /* match using lazy on-demand generation of hash table */
    prepare();
    if (m_hashA) {
        for (auto i = roi.low(); i != roi.high(); i++) { // TODO: Use for (auto i : roi)
            auto hitP = hash_match(buf + to_byte(i),
                                   to_byte(roi.high() - i)); // hit pattern
            if (hitP) {
                return bir(i, roi.bitsize());
            }
        }
    }

    /* generic match */
    /* \todo ADD \c homoF to \c net_find_rToT() and use to determine if only plits  */
    for (auto sub : m_subs) {
        auto hitS = sub->match_in(buf,len, roi);
        if (hitS.full()) {     /* one exact match is enough for now */
            return hitS;
        }
    }

    return pHit().undefine();
}

/* ---------------------------- Group Separator ---------------------------- */

csc
Alt::rand(bir ssr) const
{
    if (m_subs.size() == 0) { PERR("No subs!\n"); }
    return std::random_pick(m_subs)->rand(ssr);
}

Base::Skips8 &
Alt::intersect_skips(Skips8 & skips) const
{
    load_skips8();
    for (auto sub : m_subs) {    /* for each sub */
        if (sub->intersect_skips(skips).none()) { // if not bits left
            return *m_skips8.get();     // no use in further reducing
        }
    }
    return *m_skips8.get();
}

bool
Alt::detect_range() const
{
    bool range_flag = true; // flags for 1-byte plit range
    uchar prev = 0;     // previous plit byte
    for (auto sub : m_subs) {
        if (auto lit = dynamic_cast<Lit*>(sub)) {
            if (lit->bitsize() == 8) {
                if (sub == *begin(m_subs)) {
                    prev = *lit->data();
                    continue;
                } else {
                    if (prev + 1 == *lit->data()) { // if consecutive byte
                        continue;
                    }
                }
            }
        }
        range_flag = false; break; // bail out
    }
    return range_flag;
}

Base *
Alt::mutating_canonicalize()
{
    // TODO: Replace Alts in m_subs with their subs inplace and at the end.
    // TODO: Remove duplicate children.
    Base * ret = this;
    switch (m_subs.size()) {
    case 1: break;
    case 2: break;
    default: {
        PTODO("Use detect_range() and possibly convert to Range!\n");
        break;
    }
    }
    return ret;
}

// ---------------------------- Group Separator ----------------------------

namespace gen {

Alt* alt_cstrA(const char** valA, const char** nameA, size_t altM, const char* common_name)
{
    Alt * palt = new Alt();
    Base *litA[altM];      // sub-literals
    for (size_t i = 0; i < altM; i++) {
        Lit * dX = new Lit(valA[i]);
        litA[i] = dX;
        if (nameA) {
            dX->set_name(nameA[i]);
        } else {
            dX->set_name(common_name);
        }
    }
    palt->add(litA, altM);
    return palt;
}

}
}
}
