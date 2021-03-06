#include <ostream>

#include "../pathops.hpp"
#include "../memcmpn.h"
#include "../qsort_mt.h"
#include "../show.hpp"
#include "preg.hpp"
#include "ob.hpp"
#include "dup.hpp"

namespace semnet {

chash::chashid g_default_hid = chash::CHASH_SHA1_160;

csc g_default_ob_name("Undefined path");

Ob::Ob()
{
}

Ob::~Ob()
{
}

void
Ob::net_addM(const MRel& a, bool unique_flag) const
{
    net_addM(a.get_type(), a.data(), a.size(), unique_flag);
}

void
Ob::net_addM(const Refs& a, bool unique_flag) const
{
    net_addM(a.dataT(), a.dataD(), a.size(), unique_flag);
}

// Ob *
// Ob::net_nthr(REL_t rT, const Ob * curr, int idx, bool cyclic_flag)
// {
//     Ob * nobj = nullptr;
//     Obs hits = net_find_rToT(rT);
//     int i = hits.find(curr);
//     if (i >= 0) { nobj = hits.clamped_nthD(i + idx, cyclic_flag); }
//     else       { PWARN("curr not found"); }
//     return nobj;
// }

/* ---------------------------- Group Separator ---------------------------- */

bool Ob::is_tag() const { return g_oreg->m_tags.get(this); }
bool Ob::tag() { return (g_oreg->m_tags.insert(this) == 0); }
bool Ob::untag() { return (g_oreg->m_tags.remove(this) == 0); }

void parr_set_tag(Ob ** obs, size_t obs_N, bool tag)
{
    // \todo Optimize array insert to hash-table.
    for (size_t i = 0; i < obs_N; i++) { obs[i]->set_tag(tag); }
}

/* ---------------------------- Group Separator ---------------------------- */

bool Ob::is_lock() const { return g_oreg->m_lock_hash.get(this); }
bool Ob::lock() { return (g_oreg->m_lock_hash.insert(this) == 0); }
bool Ob::unlock() { return (g_oreg->m_lock_hash.remove(this) == 0); }

/* ---------------------------- Group Separator ---------------------------- */

bool Ob::is_obselete() const { return g_oreg->m_obs_hash.get(this); }
bool Ob::make_obselete() { return (g_oreg->m_obs_hash.insert(this) == 0); }

/* ---------------------------- Group Separator ---------------------------- */

tdepth_t
parr_get_tree_depth_max(Ob ** obs, size_t obs_N)
{
    tdepth_t max = 0;
    for (size_t i = 0; i < obs_N; i++) { max = std::max(max, obs[i]->get_tree_depth()); }
    return max;
}

theight_t
parr_get_tree_height_max(Ob ** obs, size_t obs_N)
{
    theight_t max = 0;
    for (size_t i = 0; i < obs_N; i++) { max = std::max(max, obs[i]->get_tree_height()); }
    return max;
}

/* ---------------------------- Group Separator ---------------------------- */

tdepth_t Ob::get_tree_depth(OB_t oT) const
{
    Obs hits = net_find_rToT(REL_PARENT, oT);
    tdepth_t tdepth = parr_get_tree_depth_max(hits.data(), hits.size());
    return tdepth+1;
}

theight_t Ob::get_tree_height(OB_t oT) const
{
    Obs hits = net_find_rToT(REL_CHILD, oT);
    theight_t theight = parr_get_tree_height_max(hits.data(), hits.size());
    return theight+1;
}

/* ---------------------------- Group Separator ---------------------------- */

bool Ob::is_related_to(REL_t prel, Ob * pA) const
{
    bool ret = false;
    Ob * pA_F = net_first(prel, pA);
    if ((pA_F == nullptr and pA == nullptr) or /* if root */
        pA_F == pA) { ret = true; }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

#include <sys/param.h>
#include <iomanip>

csc Ob::get_pathX(REL_t prel, OB_t oT, PPATH_FMT_t path_fmt) const
{
    const size_t cap = MAXPATHLEN; // capacity: maxlength of path
    char path_[cap+1];              // capacity plus nullptr
    size_t off = cap;              // offset initially at end
    path_[off] = '\0';              // last nil-terminator

    if (path_fmt == PPATH_FMT_FULL or
        path_fmt == PPATH_FMT_TYPED_FULL) { // if we flagged for full path (which includes parent)
        auto pathL = get_pathL();
        off -= pathL.size(); memcpy(path_+off, pathL.data(), pathL.size());
    }

    /* scan in direction given by \p prel
     * if prel is REL_PARENT => path should grow backwards in memory */
    Ob * curr = net_first(prel, oT); // start with first parent
    if (curr) {
        while (curr) {
            off -= 1; path_[off] = PATH_SEP;
            auto pathL = get_pathL();
            auto curr_pathL = curr->get_pathL();
            off -= curr_pathL.size(); memcpy(path_+off, curr_pathL.data(), curr_pathL.size());
            curr = curr->net_first(prel); // recurse upwards
        }
    } else {
        off -= 1; path_[off] = PATH_SEP;
    }

    if (path_fmt == PPATH_FMT_TYPED_LOCAL or
        path_fmt == PPATH_FMT_TYPED_FULL) { // if we want typed path_
        PTEST("PPATH_FMT_TYPED_... not tested\n");
        size_t tprefix_len = 0;
        const char * tprefix = get_typename(&tprefix_len);
        if (tprefix) {
            off -= 1; path_[off] = ':'; // type prefix separator
            off -= tprefix_len; memcpy(path_+off, tprefix, tprefix_len);
        }
    }

    return csc(path_+off, cap-off);
}

/* ---------------------------- Group Separator ---------------------------- */

std::ostream &
Ob::show(std::ostream& os) const
{
    tdepth_t tdepth = get_tree_depth();
    if (tdepth != TDEPTH_undefined_) {
        os_spaces(os, tdepth);
    }
    os << " " << get_typename() << " (" << this << ")";
    if (is_obselete()) { os << " [del]"; } else { os << " ___"; }
    os << std::endl;
    return os;
}

/* ---------------------------- Group Separator ---------------------------- */

int
net_connectM(REL_t rT0, Ob * ob0,
             REL_t rT1, Obs& obs1)
{
    return net_connectM(rT0, ob0,
                        rT1, obs1.data(), obs1.size());
}

int
net_disconnectS(REL_t rTA, const Ob * obA,
                REL_t rTB, const Ob * obB,
                bool obs_flag)
{
#ifndef NDEBUG
    if (not REL_duals(rTA, rTB)) { PERR("Dualism check failed: rTA=%d rTB=%d\n", rTA, rTB); }
#endif
    if        (obA != nullptr and obB != nullptr) {
        return (obA->net_remM(rTB, obB) +
                obB->net_remM(rTA, obA));
    } else if (obA != nullptr and obB == nullptr) {
        return (obA->net_remM(rTB, nullptr));
    } else if (obA == nullptr and obB != nullptr) {
        return (obB->net_remM(rTA, nullptr));
    } else {
        PERR("Both obA and obB are nullptr\n");
        return 0;
    }
}

bool
Ob::net_is_cyclic() const
{
    bool ret = false;
    PTODO("while loop that iterates network from \\c this using Ob::net_cloneAll() and uses a pointer-ght on local stack to log nodes already accessed\n");
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int Ob::check_preg_rmagic() const
{
    int ret = 0;
    if (g_oreg->get_rmagic()) {
        if (memcmp(g_oreg->get_rmagic(), g_oreg_rmagic, PREG_RMAGIC_SIZE) != 0) {
            PERR("g_oreg->get_rmagic() != g_oreg_rmagic\n");
        } else { ret = 1; }
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

void Ob::post_relate_cb(REL_t rT)
{
}

}
