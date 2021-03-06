#include "obmr.hpp"
#include "rels.hpp"

#include "../stdio_x.h"
#include <iomanip>

namespace semnet {

size_t
ObMr::net_remM(REL_t rT, const Ob* rD) const
{
    return m_rels.remRs(rT, const_cast<Ob*>(rD));
}

size_t
ObMr::net_uniquifyM(REL_t rT, const Ob* rD) const
{
    return m_rels.uniquify(rT, const_cast<Ob*>(rD));
}

/* ---------------------------- Group Separator ---------------------------- */

void
ObMr::net_self_relate(REL_t rT) const
{
    m_rels.app(rT, const_cast<ObMr*>(this));
}

void
ObMr::net_addM(REL_t rT, const Ob* const * rDA, size_t rDN, bool unique_flag) const
{
#ifndef NDEBUG
    for (size_t i = 0; i < rDN; i++) {
        if (rDA[i] == nullptr) { PERR("rDA[%zd] nullptr\n", i); }
        if (unique_flag) {
            auto hit = m_rels.find(rT, rDA[i]);
            if (hit >= 0) { PERR("this already related to rDA[%zd]", i); }
        }
    }
#endif
    m_rels.app(rT, const_cast<Ob* const*>(rDA), rDN);
}

void
ObMr::net_addM(const REL_t * rTA, const Ob* const* rDA, size_t rDN, bool unique_flag) const
{
#ifndef NDEBUG
    for (size_t i = 0; i < rDN; i++) {
        if (rDA[i] == nullptr) { PERR("rDA[%zd] nullptr\n", i); }
        if (unique_flag) {
            auto hit = m_rels.find(rTA[i], rDA[i]);
            if (hit >= 0) { PERR("this already related to rDA[%zd]", i); } 
        }
    }
#endif
    m_rels.app(rTA, const_cast<Ob* const*>(rDA), rDN);
}

size_t ObMr::net_retargetM(const Ob* from, const Ob* to) const
{
    return m_rels.retarget(const_cast<Ob*>(from), const_cast<Ob*>(to));
}

size_t ObMr::net_retypeM(REL_t from, REL_t to) const
{
    return m_rels.retype(from, to);
}

/* ---------------------------- Group Separator ---------------------------- */

size_t
ObMr::net_disconnectM(REL_t rT, OB_t oT, const Ob* rD, bool obs_flag) const
{
    size_t i, pi;                   /* normal index and pack index */
    size_t c = 0;                   /* relation from \c this remove counter */
    size_t d = 0;                   /* relation to \c this remove counter */

    const REL_t inv_rT = REL_getDual(rT);

    for (i = pi = 0; i < m_rels.size(); i++) { // for all \c this-relations: \todo Replace with iterator logic.
        Ob*& rDi = m_rels[i];
        const OB_t oTi = rDi->get_type();
        REL_t& rTi = m_rels.nthT(i);
        if (rDi and	/* non-zero destination */
            REL_matchS(rTi, rT) and /* a general relation match */
            OB_matchS(oTi, oT) and /* a general relation match */
            (rD == nullptr or rD == rDi)) { /* all or a specific destination */
            if (rDi == this) {           // if self-relating
                d++;                     // balance counters
            } else {                     // if relates to other
                d += rDi->net_remM(inv_rT, this); // remove link from other to \c this
            }
            if (obs_flag) {     // if requested for deletion
                if (rDi) { rDi->make_obselete(); } // make obselete
            }
            rDi = nullptr;	/* mark as empty */
            c++;        /* increase remove counter */
        } else if (rDi) {    // zero destination
            m_rels[pi] = rDi;
            m_rels.nthT(pi) = rTi;
            pi++;
        }
    }

    if (c != d) { PERR("Unbalance: c=%zd not equal to d=%zd\n", c, d); }

    m_rels.truncate(pi);
    m_rels.m_packed = true;

    return pi;
}

/* ---------------------------- Group Separator ---------------------------- */

Ob*
ObMr::net_nth_rMoMdT(const REL_t * rM, size_t rM_N,
                     const OB_t * oM, size_t oM_N,
                     const Ob* rD_arg,
                     size_t nth) const
{
    Ob * hit = nullptr;
    size_t i, hc;		/* index and hit counter  */
    for (i = hc = 0; i < m_rels.size(); i++) {
        const Ob* rDi = m_rels[i];
        const REL_t rTi = m_rels.nthT(i);
        if (rDi and		/* non-zero destination */
            REL_matchM(rTi, rM,rM_N) and
            OB_matchM(rDi->get_type(), oM,oM_N) and
            (rD_arg == 0 or	/* we don't need to care about destination Ob*/
             rDi == rD_arg)) { /* matching relation */
            if (hc == nth) { hit = const_cast<Ob*>(rDi); break; } else { hc++; }
        }
    }
    return hit;
}

/* ---------------------------- Group Separator ---------------------------- */

Obs
ObMr::net_find_rMoM(const REL_t * rM, size_t rM_N,
                    const OB_t * oM, size_t oM_N,
                    VREC_t vrec) const
{
    Obs obs;
    for (size_t i = 0; i < m_rels.size(); i++) {
        const Ob* rDi = m_rels[i];
        const REL_t rTi = m_rels.nthT(i);
        if (rDi and		/* non-zero destination */
            OB_matchM(rDi->get_type(), oM,oM_N) and
            REL_matchM(rTi, rM,rM_N)) { /* matching relation */
            obs.push_back(const_cast<Ob*>(rDi));
        }
    }
    return obs;
}

/* ---------------------------- Group Separator ---------------------------- */

size_t
ObMr::net_count_rMoM(const REL_t * rM, size_t rM_N,
                     const OB_t * oM, size_t oM_N) const
{
    size_t i, mi;		/* normal index and match index */
    for (i = mi = 0; i < m_rels.size(); i++) {
        const Ob* rDi = m_rels[i];
        const REL_t rTi = m_rels.nthT(i);
        if (rDi and
            REL_matchM(rTi, rM, rM_N) and
            OB_matchM(rDi->get_type(), oM,oM_N)) { /* matching relation */
            mi++;
        }
    }
    return mi;
}

/* ---------------------------- Group Separator ---------------------------- */

std::ostream &
ObMr::show_relations(std::ostream& os) const
{
    ObMr* this2 = const_cast<ObMr*>(this); // Note: cast away constness for now
    this2->m_rels.sort();                          // so that we can sort them

    if (this2->m_rels.size()) {
        os << "  has relations: ";
        REL_t prev_rT = REL_any_;
        for (size_t i = 0; i < this2->m_rels.size(); i++) {
            const Ob* rDi = this2->m_rels[i];
            const REL_t rTi = this2->m_rels.nthT(i);
            if (rDi) {	/* non-zero destination */
                if (prev_rT != rTi) {
                    os << "  =[%s]=>" << REL_getName(rTi);
                } else {
                    os << ",";
                }
                os << "%s<%p>" << rDi->get_typename() << rDi;
                prev_rT = rTi;
            }
        }
    }
    os << std::endl;
    return os;
}

}
