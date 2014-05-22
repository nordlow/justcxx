#include "LOD_tess.hpp"

size_t
tessEllSeg_R(vec2f * vtx,
             const vec2f& rad,
             size_t vi, size_t vN, uint mLOD)
{
    PTEST("\n");
    for (size_t i = 0; i < vN; i++, vi++) {
        const vec2f& udir = tesstab_cat(mLOD*vi); /* unit-direction */
        vtx[i] = udir * rad;
    }
    return vN;
}

/* ========================================================================= */

size_t
tessEllSeg_CR(vec2f * vtx,
              const vec2f& cen, const vec2f& rad,
              size_t vi, size_t vN, uint mLOD)
{
    for (size_t i = 0; i < vN; i++, vi++) {
        vtx[i] = tesstab_cat(mLOD*vi) * rad + cen;
    }
    return vN;
}

size_t
tessEllSeg_CRa(vec2f * vtx,
               const vec2f& cen,
               const vec2f& rad, ROTANG_t rotang,
               size_t vi, size_t vN, uint mLOD)
{
    for (size_t i = 0; i < vN; i++, vi++) {
        vtx[i] = g_rottab[rotang] * tesstab_cat(mLOD*vi) * rad + cen;
    }
    return vN;
}

/* ---------------------------- Group Separator ---------------------------- */

size_t
tessEllSegSineFlower_CR(vec2f * vtx,
                        const vec2f& cen, const vec2f& rad,
                        float amp, int frq, float phs,
                        size_t vi, size_t vN, uint mLOD)
{
    const float ifrq = frq * M_2PI / vN;
    for (size_t i = 0; i < vN; i++, vi++) {
        const float leaf_rad = amp * sinf(vi * ifrq + phs);
        vec2f new_rad; vec2f_sums(rad, leaf_rad, &new_rad);
        vec2f_pwprod_sum(cen, tesstab_cat(mLOD*vi), &new_rad, &vtx[i]);
    }
    return vN;
}

size_t
tessEllSegSineFlower_CRa(vec2f * vtx,
                         const vec2f& cen, const vec2f& rad,
                         float amp, int frq, float phs,
                         ROTANG_t rotang,
                         size_t vi, size_t vN, uint mLOD)
{
    const float ifrq = frq * M_2PI / vN;
    for (size_t i = 0; i < vN; i++, vi++) {
        const float leaf_rad = amp * sinf(vi * ifrq + phs);
        vec2f new_rad; vec2f_sums(rad, leaf_rad, &new_rad);
        mat2f_pwmul_mvmul_add(tesstab_cat(mLOD*vi), &new_rad, &g_rottab[rotang], cen, &vtx[i]);
    }
    return vN;
}

/* ---------------------------- Group Separator ---------------------------- */

size_t
tessEllSeg_CRR(vec2f * vtx,
               const vec2f& cen,
               const vec2f& radI, const vec2f& radO,
               size_t vi, size_t vN, uint mLOD)
{
    for (size_t i = 0; i < vN; i++, vi++) {
        const vec2f& udir = tesstab_cat(mLOD*vi); /* unit-direction */
        vec2f_pwprod_sum(cen, udir, radI, &vtx[2*i+0]);
        vec2f_pwprod_sum(cen, udir, radO, &vtx[2*i+1]);
    }
    return vN;
}

size_t
tessEllSeg_CRRa(vec2f * vtx,
                const vec2f& cen,
                const vec2f& radI, const vec2f& radO,
                ROTANG_t rotang,
                size_t vi, size_t vN, uint mLOD)
{
    for (size_t i = 0; i < vN; i++, vi++) {
        vec2f udir = *tesstab_cat(mLOD*vi); /* unit-direction */
        vec2f_mvmul_mat2f(&udir, &g_rottab[rotang]);
        vec2f_pwprod_sum(cen, &udir, radI, &vtx[2*i+0]);
        vec2f_pwprod_sum(cen, &udir, radO, &vtx[2*i+1]);
    }
    return vN;
}

/* ---------------------------- Group Separator ---------------------------- */

size_t
tessEllSeg_CCRR(vec2f * vtx,
                const vec2f& cenI, const vec2f& cenO,
                const vec2f& radI, const vec2f& radO,
                size_t vi, size_t vN, uint mLOD)
{
    for (size_t i = 0; i < vN; i++, vi++) {
        const vec2f& udir = tesstab_cat(mLOD*vi); /* unit-direction */
        vec2f_pwprod_sum(cenI, udir, radI, &vtx[2*i+0]);
        vec2f_pwprod_sum(cenO, udir, radO, &vtx[2*i+1]);
    }
    return vN;
}

/* ========================================================================= */
