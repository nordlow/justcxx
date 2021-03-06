#include "ob_cmp.hpp"
#include "ob.hpp"
#include "../qsort_mt.h"

#if 0
int
ob_cmp_creation_time(const Ob * pA, const Ob * pB)
{
    int ret = 0;
    PTOWRITE("By reading specific metadata from imagefiles using Freeimage, FFMPEG, libid3, etc\n");
    return ret;
}

int
ob_cmp_stat(const Ob * pA, const Ob * pB, PSORT_t psort)
{
    int ret = 0;
    PTOWRITE("By reading using lstat()");
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
ob_cmp_name(const Ob * pA, const Ob * pB)
{
    int ret = 0;
    auto name0 = pA->get_pathL();
    auto name1 = pB->get_pathL();
    ret = strncmp(name0.c_str(), name1.c_str(),
                  std::min(name0.size(),
                           name1.size()));
    if (ret == 0) { ret = pnw::cmpsgn(name0.size(),
                                      name1.size()); }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
ob_cmp_csize(const Ob * pA, const Ob * pB)
{
    return 0;
    // return pnw::cmpsgn(pA->get_tree_csize(),
    //                    pB->get_tree_csize());
}

/* ---------------------------- Group Separator ---------------------------- */

int
ob_cmp_otype_chash_or_content(const Ob * pA, const Ob * pB)
{
    int ret = 0;
    if (pA->get_type() == pB->get_type()) {
        ret = pA->eq_content(pB);
    } else {
        ret = pA->cmp_otype(pB);
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
voidp_ob_cmp_std(const void * a, const void * b)
{
#ifndef NDEBUG
    if (a == nullptr) { PERR("Argument a is nullptr\n"); return 0; }
    if (b == nullptr) { PERR("Argument b is nullptr\n"); return 0; }
#endif
    const Ob * ao = *((const Ob**)a), * bo = *((const Ob**)b);
    if (ao->get_type() == bo->get_type()) {
        const int size_sgn = ob_cmp_csize(ao, bo); // compare size
        if (size_sgn == 0) {
            const int content_sgn = ob_cmp_otype_chash_or_content(ao, bo); // compare content (hash)
            if (content_sgn == 0) {
                const int addr_sgn = ptr_cmp(ao, bo); // compare address
                return addr_sgn;
                // if (addr_sgn == 0) {
                // } else {
                //     return addr_sgn;
                // }
            } else {
                return content_sgn;
            }
        } else {
            return size_sgn;
        }
    } else {
        return ao->cmp_otype(bo);         // compare type
    }
}

int
voidp_ob_cmp_type_addr(const void * a, const void * b)
{
    const Ob * ao = *((const Ob**)a), * bo = *((const Ob**)b);
    const int type_sgn = ao->cmp_otype(bo); // type
    if (type_sgn == 0) {
        const int addr_sgn = ptr_cmp(ao, bo); // address
        return addr_sgn;
    } else {
        return type_sgn;
    }
}

int
voidp_ob_cmp_name(const void * a, const void * b)
{
#ifndef NDEBUG
    if (a == nullptr) { PERR("Argument a is nullptr\n"); return 0; }
    if (b == nullptr) { PERR("Argument b is nullptr\n"); return 0; }
#endif
    const Ob * ao = *((const Ob**)a), * bo = *((const Ob**)b);
    const int sgn = ob_cmp_name(ao, bo);
    return sgn;
}

int
voidp_ob_cmp_tdepth(const void * a, const void * b)
{
#ifndef NDEBUG
    if (a == nullptr) { PERR("Argument a is nullptr\n"); return 0; }
    if (b == nullptr) { PERR("Argument b is nullptr\n"); return 0; }
#endif
    const Ob * ao = *((const Ob**)a), * bo = *((const Ob**)b);
    const int sgn = pnw::cmpsgn(ao->get_tree_depth(),
                                bo->get_tree_depth());
    return sgn;
}

int
voidp_ob_cmp_theight(const void * a, const void * b)
{
#ifndef NDEBUG
    if (a == nullptr) { PERR("Argument a is nullptr\n"); return 0; }
    if (b == nullptr) { PERR("Argument b is nullptr\n"); return 0; }
#endif
    const Ob * ao = *((const Ob**)a), * bo = *((const Ob**)b);
    const int sgn = pnw::cmpsgn(ao->get_tree_height(),
                                bo->get_tree_height());
    return sgn;
}

int
voidp_ob_cmp_creation_time(const void * a, const void * b)
{
    const Ob * ao = *((const Ob**)a), * bo = *((const Ob**)b);
    return ob_cmp_creation_time(ao, bo);
}

int
voidp_ob_cmp_status_change_time(const void * a, const void * b)
{
    const Ob * ao = *((const Ob**)a), * bo = *((const Ob**)b);
    return ob_cmp_stat(ao, bo, PSORT_STATUS_CHANGE_TIME);
}

int
voidp_ob_cmp_access_time(const void * a, const void * b)
{
    const Ob * ao = *((const Ob**)a), * bo = *((const Ob**)b);
    return ob_cmp_stat(ao, bo, PSORT_ACCESS_TIME);
}

int
voidp_ob_cmp_mod_time(const void * a, const void * b)
{
    const Ob * ao = *((const Ob**)a), * bo = *((const Ob**)b);
    return ob_cmp_stat(ao, bo, PSORT_MOD_TIME);
}

/* ---------------------------- Group Separator ---------------------------- */

void parr_sort(Ob ** obs, size_t obs_N,
               PSORT_t psort)
{
    switch (psort) {
    case PSORT_TDEPTH:
        qsort_mt(obs, obs_N, sizeof(Ob*), voidp_ob_cmp_tdepth, 0,0);
        break;
    case PSORT_THEIGHT:
        qsort_mt(obs, obs_N, sizeof(Ob*), voidp_ob_cmp_theight, 0,0);
        break;
    case PSORT_CREATION_TIME:
        qsort_mt(obs, obs_N, sizeof(Ob*), voidp_ob_cmp_creation_time, 0,0);
        break;
    case PSORT_STATUS_CHANGE_TIME:
        qsort_mt(obs, obs_N, sizeof(Ob*), voidp_ob_cmp_status_change_time, 0,0);
        break;
    case PSORT_ACCESS_TIME:
        qsort_mt(obs, obs_N, sizeof(Ob*), voidp_ob_cmp_access_time, 0,0);
        break;
    case PSORT_MOD_TIME:
        qsort_mt(obs, obs_N, sizeof(Ob*), voidp_ob_cmp_mod_time, 0,0);
        break;
    default:
        break;
    }
}

/* ---------------------------- Group Separator ---------------------------- */
#endif
