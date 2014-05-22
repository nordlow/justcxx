#include "vis.hpp"
#include "kdTree.h"

#ifndef NDEBUG
const char * g_shape_warn_sform_nodef = "Shape SFORM has not been defined yet!";
const char * g_shape_warn_sform = "Shape SFORM not supported!";
const char * g_shape_warn_bbox = "Could not calculate bounding box!";
const char * g_shape_warn_zerodat = "Shape data (sformD) is zero!";
const char * g_shape_warn_index_ctrlp_mask_index = "Vertex index beyond sizeof mask!";
#endif

/* ---------------------------- Group Separator ---------------------------- */

static kdTree * g_owner = NULL;

void vis_kdtree_init(kdTree * owner)
{
    g_owner = owner;
}

static void vis_unreg(vis_t& shp)
{
/*   if (g_owner and shp.sformD) { kdTree_rm_shape(g_owner, shp); } */
}

static void vis_reg(vis_t& shp)
{
/*   if (g_owner and shp.sformD) { kdTree_ins_shape(g_owner, shp); } */
}

static void
vis_clear_SFORM(vis_t& shp)
{
    if (shp.sformD) {
        switch (shp.sform) {
        case SFORM_POLY2f: {
            poly2f& poly = (poly2f*)shp.sformD;
            poly2f_clear(poly);
            break;
        }
        case SFORM_comb2_UNION:
        case SFORM_comb2_ISECT:
        case SFORM_comb2_DIFF: {
            vis_t ** comb2 = (vis_t**)shp.sformD;
            vis_clear(comb2[0]);
            vis_clear(comb2[1]);
            free(comb2);
            break;
        }
        default:
            break;
        }
        vis_unreg(shp);
        free(shp.sformD);
    }
}

static void
vis_clear_SFILL(vis_t& shp)
{
    switch ((SFILL_t)shp.sfill) {
    case SFILL_SINGLE:
        /* do nothing */
        break;
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA:
        FREEZ(shp.sfillD.grad);
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        cpal_delete(shp.sfillD.pal); shp.sfillD.pal = 0;
        break;
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, shp.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }

    shp.sfill = SFILL_undefined_;
}

void
vis_clear(vis_t& shp)
{
    vis_clear_SFORM(shp);
    vis_clear_SFILL(shp);
}

int
vis_setLayer(vis_t& shp, layer_t layer)
{
    int ret = 0;
    if (shp.slayer != layer) {
        vis_unreg(shp);
        shp.slayer = layer; ret = 1;
        vis_reg(shp);
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

void vis_set_SFILL_SINGLE(vis_t& shp, color_t color)
{
    if (shp.sfill != SFILL_SINGLE) {
        vis_clear_SFILL(shp);
        shp.sfill = SFILL_SINGLE;
    }
    shp.sfillD.single = color;
}

void vis_set_SFILL_GRAD(vis_t& shp, SFILL_t sfill,
                        color_t color0, color_t color1)
{
    if (not SFILL_is_GRAD(sfill)) { PERR("sfill:%d is not a GRAD\n", sfill); }
    if (shp.sfill != sfill) {
        vis_clear_SFILL(shp);
        shp.sfill = sfill;
    }
    shp.sfillD.grad = malloc(sizeof(color_pair_t));
    shp.sfillD.grad.x = color0;
    shp.sfillD.grad.y = color1;
}

void vis_set_SFILL_GRAD_pair(vis_t& shp, SFILL_t sfill,
                             const color_pair_t * grad)
{
    vis_set_SFILL_GRAD(shp, sfill, grad.x, grad.y);
}

void vis_set_SFILL_GRAD_default(vis_t& shp, SFILL_t sfill)
{
    if (shp.sfill == SFILL_SINGLE) {
        vis_set_SFILL_GRAD(shp, sfill,
                           shp.sfillD.single, shp.sfillD.single);
    } else if (SFILL_is_GRAD(shp.sfill)) { /* use current GRAD */
        vis_set_SFILL_GRAD_pair(shp, sfill, shp.sfillD.grad);
    } else if (SFILL_is_PAL(shp.sfill)) { /* use current PAL */
        if (shp.sfillD.pal.cN == 1) {
            vis_set_SFILL_GRAD(shp, sfill,
                               shp.sfillD.pal.cA[0],
                               shp.sfillD.pal.cA[0]);
        } else if (shp.sfillD.pal.cN >= 2) {
            vis_set_SFILL_GRAD(shp, sfill,
                               shp.sfillD.pal.cA[0],
                               shp.sfillD.pal.cA[shp.sfillD.pal.cN-1]);
        } else {
            PERR("Cannot handle case when elmsN is %zd\n", shp.sfillD.pal.cN);
        }
        vis_set_SFILL_GRAD(shp, sfill,
                           COLOR_RED, COLOR_YELLOW);
    } else {

    }
}

void vis_set_SFILL_PAL(vis_t& shp, SFILL_t sfill,
                       const cpal_t * pal)
{
    if (not SFILL_is_PAL(sfill)) { PERR("sfill:%d is not a PAL\n", sfill); }
    vis_clear_SFILL(shp); /* NOTE: WE NEED TO CLEAR FOR EVERY SFILL_PAL */
    shp.sfill = sfill;
    shp.sfillD.pal = cpal_clone(pal); /* BECAUSE WE REALLOCATE HERE */
}

void vis_set_SFILL_PAL3(vis_t& shp, SFILL_t sfill,
                        color_t color0,
                        color_t color1,
                        color_t color2)
{
    if (not SFILL_is_PAL(sfill)) { PERR("sfill:%d is not a PAL\n", sfill); }
    vis_clear_SFILL(shp); /* NOTE: WE NEED TO CLEAR FOR EVERY SFILL_PAL */
    shp.sfill = sfill;
    shp.sfillD.pal = cpal_new_from_3colors(color0,
                                            color1,
                                            color2); /* BECAUSE WE REALLOCATE HERE */
}

/* ---------------------------- Group Separator ---------------------------- */

int vis_setColor(vis_t& shp, color_t color, size_t idx)
{
    int ret = 0;
    switch ((SFILL_t)shp.sfill) {
    case SFILL_SINGLE: {
        shp.sfillD.single = color;
        ret = 1;
        break;
    }
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA: {
        if ((idx bitand 1) == 0) { shp.sfillD.grad.x = color; }
        else                     { shp.sfillD.grad.y = color; }
        ret = 1;
        break;
    }
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA: {
        shp.sfillD.pal.cA[idx % shp.sfillD.pal.cN] = color;
        ret = 1;
        break;
    }
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, shp.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }
    return ret;
}

int vis_rdColor(vis_t shp, size_t idx, color_t * color)
{
    int ret = 0;
    switch ((SFILL_t)shp.sfill) {
    case SFILL_SINGLE: {
        *color = shp.sfillD.single; ret = 1;
        break;
    }
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA: {
        if ((idx bitand 1) == 0) { *color = shp.sfillD.grad.x; ret = 1; }
        else                     { *color = shp.sfillD.grad.y; ret = 1; }
        break;
    }
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA: {
        *color = shp.sfillD.pal.cA[idx % shp.sfillD.pal.cN]; ret = 1;
        break;
    }
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, shp.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }
    return ret;
}

color_t vis_getColor(vis_t shp, size_t idx)
{
    color_t color = COLOR_BLACK;
    vis_rdColor(shp, idx, color);
    return color;
}

int vis_randRGB_SFILL(vis_t& shp)
{
    int ret = 0;
    switch ((SFILL_t)shp.sfill) {
    case SFILL_SINGLE: {
        shp.sfillD.single = color_randRGB();
        ret = 1;
        break;
    }
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA:
        shp.sfillD.grad.x = color_randRGB();
        shp.sfillD.grad.y = color_randRGB();
        ret = 2;
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        colorarray_randRGB(shp.sfillD.pal.cA[0], shp.sfillD.pal.cN);
        ret = shp.sfillD.pal.cN;
        break;
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, shp.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }
    return ret;
}

int vis_randRGBA_SFILL(vis_t& shp)
{
    int ret = 0;
    switch ((SFILL_t)shp.sfill) {
    case SFILL_SINGLE: {
        shp.sfillD.single = color_randRGBA();
        ret = 1;
        break;
    }
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA:
        shp.sfillD.grad.x = color_randRGBA();
        shp.sfillD.grad.y = color_randRGBA();
        ret = 2;
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        colorarray_randRGBA(shp.sfillD.pal.cA[0], shp.sfillD.pal.cN);
        ret = shp.sfillD.pal.cN;
        break;
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, shp.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }
    return ret;
}

int
vis_highlightInd(vis_t& shp, uint8_t prg)
{
    int ret = 0;
    /*!
     * Normalized Indication Progress.
     * The use of +1 turns division into a bitshift of 8
     * as (COLOR_COMP_MAX+1) is normally equal to 256 */
    const float norm_prg = ((float)prg + 1) / (256);
    switch (shp.sfill) {
    case SFILL_SINGLE: {
        shp.sfillD.single = color_highlightInd(shp.sfillD.single, norm_prg);
        ret = 1;
        break;
    }
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA:
        shp.sfillD.grad.x = color_highlightInd(shp.sfillD.grad.x, norm_prg);
        shp.sfillD.grad.y = color_highlightInd(shp.sfillD.grad.y, norm_prg);
        ret = 2;
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        colorarray_highlightInd(shp.sfillD.pal.cA[0], shp.sfillD.pal.cN,
                                norm_prg);
        ret = shp.sfillD.pal.cN;
        break;
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, shp.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }
    return ret;
}

int
vis_modA(vis_t& shp, uint8_t alpha)
{
    int ret = 0;
    switch (shp.sfill) {
    case SFILL_SINGLE: {
        shp.sfillD.single = color_modA(shp.sfillD.single, alpha);
        ret = 1;
        break;
    }
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA:
        shp.sfillD.grad.x = color_modA(shp.sfillD.grad.x, alpha);
        shp.sfillD.grad.y = color_modA(shp.sfillD.grad.y, alpha);
        ret = 2;
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        colorarray_modA(shp.sfillD.pal.cA[0], shp.sfillD.pal.cN, alpha);
        ret = shp.sfillD.pal.cN;
        break;
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, shp.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }
    return ret;
}

int
vis_scaleT(vis_t& shp, uint8_t alpha)
{
    int ret = 0;
    switch (shp.sfill) {
    case SFILL_SINGLE: {
        shp.sfillD.single = color_scaleT(shp.sfillD.single, alpha);
        ret = 1;
        break;
    }
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA:
        shp.sfillD.grad.x = color_scaleT(shp.sfillD.grad.x, alpha);
        shp.sfillD.grad.y = color_scaleT(shp.sfillD.grad.y, alpha);
        ret = 2;
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        colorarray_scaleT(shp.sfillD.pal.cA[0], shp.sfillD.pal.cN, alpha);
        ret = shp.sfillD.pal.cN;
        break;
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, shp.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
vis_set_copy_SFORM(vis_t& shp, vis_t b)
{
    int ret = 0;
    switch (b.sform) {
    case SFORM_BOX2f: {
        box2f& box = (box2f*)b.sformD;
        ret = vis_setBOX(shp, box);
        break;
    }
    case SFORM_ELL2f: {
        ell2f& ell = (box2f*)b.sformD;
        ret = vis_setELL(shp, ell);
        break;
    }
    case SFORM_POLY2f: {
        poly2f& poly = (poly2f*)b.sformD;
        ret = vis_setPOLY(shp, poly.v, poly.n);
        break;
    }
    case SFORM_TRI2f: {
        tri2f& tri = (tri2f*)b.sformD;
        ret = vis_setTRI(shp, tri.p);
        break;
    }
    case SFORM_QUAD2f: {
        vec2f& quad = (vec2f*)b.sformD;
        ret = vis_setQUAD(shp, quad);
        break;
    }
    case SFORM_SQR2f: {
        sqr2f& sqr = (sqr2f*)b.sformD;
        ret = vis_setSQR(shp, sqr);
        break;
    }
    case SFORM_CIR2f: {
        cir2f& cir = (cir2f*)b.sformD;
        ret = vis_setCIR(shp, cir);
        break;
    }
    case SFORM_CRING2f: {
        cring2f& cring = (cring2f*)b.sformD;
        ret = vis_setCRING(shp, cring);
        break;
    }
    case SFORM_CRARC2f: {
        crarc2f& crarc = (crarc2f*)b.sformD;
        ret = vis_setRSEG(shp, crarc);
        break;
    }
    case SFORM_SPHEREf: {
        spheref * sphere = (spheref*)b.sformD;
        ret = vis_setSPHERE(shp, sphere);
        break;
    }
    case SFORM_RBOX2f_ell: {
        rbox2f& rbox = (rbox2f*)b.sformD;
        ret = vis_setRBOX_ELL(shp, rbox.b, rbox.r, rbox.cornmask);
        break;
    }
    case SFORM_RBOX2f_lin: {
        rbox2f& rbox = (rbox2f*)b.sformD;
        ret = vis_setRBOX_LIN(shp, rbox.b, rbox.r, rbox.cornmask);
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, b.sform); ret = -1; break;
    }
    return ret;
}

int
vis_set_copy_SFILL(vis_t& shp, vis_t b)
{
    int ret = 0;
    switch ((SFILL_t)b.sfill) {
    case SFILL_SINGLE:
        vis_set_SFILL_SINGLE(shp, b.sfillD.single);
        break;
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA:
        vis_set_SFILL_GRAD_pair(shp, b.sfill, b.sfillD.grad);
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        vis_set_SFILL_PAL(shp, b.sfill, b.sfillD.pal);
        break;
    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, b.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }
    return ret;
}

int
vis_set_SFRAME(vis_t& shp, SFRAME_t sframe)
{
    int ret = 1;
    shp.sframe = sframe;
    return ret;
}

int
vis_set_copy_SFRAME(vis_t& shp, vis_t b)
{
    int ret = 1;
    shp.sframe = b.sframe;
    return ret;
}

void
vis_setShape(vis_t& shp, vis_t b)
{
    int ret = 0;
    ret += vis_set_copy_SFORM(shp, b);
    ret += vis_set_copy_SFILL(shp, b);
    ret += vis_set_copy_SFRAME(shp, b);
}

vis_t *
vis_clone(vis_t a)
{
    vis_t *shp = malloc(sizeof(vis_t));
    vis_init_vis(shp, a);
    return shp;
}

/* ---------------------------- Group Separator ---------------------------- */

int fit_in(vis_t& shp, box2f ext, bool morph_flag)
{
    int ret = 0;
    vis_unreg(shp);

    switch (shp.sform) {
    case SFORM_BOX2f:
    case SFORM_ELL2f:
        if (shp.sformD) {
            box2f& rect = (box2f*)shp.sformD;
            if (not  box2f_eq(rect, ext)) {
                *rect = *ext;
                ret = 1;
            }
        } else {
            box2f& rect = (box2f*)malloc(sizeof(box2f));
            *rect = *ext;
            shp.sformD = rect;
            ret = 1;
        }
        break;
    case SFORM_POLY2f:
        if (shp.sformD) {
            poly2f& poly = (poly2f*)shp.sformD;
            ret = fit_in(poly, ext);
        } else {
            PWARN("%s\n", g_shape_warn_zerodat);
        }
        break;
    case SFORM_TRI2f:
        if (shp.sformD) {
            tri2f& tri = (tri2f*)shp.sformD;
            ret = fit_in(tri, ext);
        } else {
            PWARN("%s\n", g_shape_warn_zerodat);
        }
        break;
    case SFORM_QUAD2f:
        if (shp.sformD) {
            vec2f& quad = (vec2f*)shp.sformD;
            box2f old_ext;
            if (vis_rdBnd_box2f(shp, old_ext)) {
                vec2f_relaffine(old_ext, ext, quad[0], quad[0]);
                vec2f_relaffine(old_ext, ext, quad[1], quad[1]);
                vec2f_relaffine(old_ext, ext, quad[2], quad[2]);
                vec2f_relaffine(old_ext, ext, quad[3], quad[3]);
                ret = 1;
            } else {
                PWARN("%s\n", g_shape_warn_bbox);
            }
        } else {
            PWARN("%s\n", g_shape_warn_zerodat);
        }
        break;
    case SFORM_SQR2f: {
        if (morph_flag) {
            ret = vis_setBOX(shp, ext);
        } else {
            sqr2f& sqr = NULL;
            if (shp.sformD) { sqr = (sqr2f*)shp.sformD; }
            else             { sqr = (sqr2f*)malloc(sizeof(sqr2f)); }
            ret = fit_in(sqr, ext);
        }
        break;
    }
    case SFORM_CIR2f: {
        if (morph_flag) {
            ret = vis_setELL(shp, ext);
        } else {
            cir2f& cir = NULL;
            if (shp.sformD) { cir = (cir2f*)shp.sformD; }
            else             { cir = (cir2f*)malloc(sizeof(cir2f)); }
            ret = fit_in(cir, ext);
        }
        break;
    }
    case SFORM_CRING2f: {
        cring2f& cring = NULL;
        if (shp.sformD) { cring = (cring2f*)shp.sformD; }
        else             { cring = (cring2f*)malloc(sizeof(cring2f)); }
        ret = fit_in(cring, ext);
        break;
    }
    case SFORM_CRARC2f: {
        crarc2f& crarc = NULL;
        if (shp.sformD) { crarc = (crarc2f*)shp.sformD; }
        else {
            crarc = (crarc2f*)malloc(sizeof(crarc2f));
            crarc.p = 0; crarc.q = M_PI_4; /* default to 0 to 45 [Deg] */
        }
        ret = fit_in(crarc, ext);
        break;
    }
    case SFORM_SPHEREf: {
        spheref * sphere = NULL;
        if (shp.sformD) { sphere = (spheref*)shp.sformD; }
        else             { sphere = (spheref*)malloc(sizeof(spheref)); }
        ret = spheref_fit_inBox2f(sphere, ext);
        break;
    }
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin: {
        if (shp.sformD) {
            rbox2f& rbox = (rbox2f*)shp.sformD;
            box2f old_ext;
            if (vis_rdBnd_box2f(shp, old_ext)) {
                rbox2f_relaffine(old_ext, ext, rbox, rbox);
                ret = 1;
            } else {
                PWARN("Could not calculate bounding box. Defaulting shape\n");
            }
        } else {
            PWARN("Rounded Box shape has no data allocated. Defaulting to \"golden rounding\"\n");
            rbox2f& rbox = (rbox2f*)malloc(sizeof(rbox2f));
            box2f_scale(ext, M_GOLDRAT, rbox.b); /* inner rectangle  */
            box2f_rdSDims(ext, (1-M_GOLDRAT)/2, rbox.r); /* round radius */
            ret = 1;
        }
        ret = 1;
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }

    vis_reg(shp);

    return ret;
}

int
center_at(vis_t& shp, vec2f cen)
{
    int ret = 0;

    vis_unreg(shp);

    switch (shp.sform) {
    case SFORM_BOX2f: {
        box2f& box = (box2f*)shp.sformD;
        box.center_at(cen);
        ret = 1;
        break;
    }
    case SFORM_SQR2f: {
        sqr2f& sqr = (sqr2f*)shp.sformD;
        sqr.c = *cen;
        ret = 1;
        break;
    }
    case SFORM_CIR2f: {
        cir2f& cir = (cir2f*)shp.sformD;
        cir.c = *cen;
        ret = 1;
        break;
    }
    case SFORM_CRING2f: {
        cring2f& cring = (cring2f*)shp.sformD;
        cring.c = *cen;
        ret = 1;
        break;
    }
    case SFORM_CRARC2f: {
        crarc2f& crarc = (crarc2f*)shp.sformD;
        crarc.r.c = *cen;
        ret = 1;
        break;
    }
    case SFORM_SPHEREf: {
        spheref * sphere = (spheref*)shp.sformD;
        vec3f_set(sphere.c, cen.x, cen.y, 0); /* z defaults to zero */
        ret = 1;
        break;
    }
    default: {
        vis_move(shp, cen - shp->outer_box().cen())
        break;
    }
    }

    vis_reg(shp);

    return ret;
}

int set_dim(vis_t& a, vec2f dim, bool outer_flag)
{
    int ret = 0;

    vis_unreg(shp);

    const float rad = outer_flag ? MIN(dim.x, dim.y) : MAX(dim.x, dim.y);

    switch (shp.sform) {
    case SFORM_BOX2f: {
        box2f& box = (box2f*)shp.sformD;
        box.set_dim(box, dim);
        ret = 1;
        break;
    }
    case SFORM_SQR2f: {
        sqr2f& sqr = (sqr2f*)shp.sformD;
        sqr.r = rad;
        ret = 1;
        break;
    }
    case SFORM_CIR2f: {
        cir2f& cir = (cir2f*)shp.sformD;
        cir.r = rad;
        ret = 1;
        break;
    }
    case SFORM_CRING2f: {
        cring2f& cring = (cring2f*)shp.sformD;
        cring2f_setRadO_autoscaledRadI(cring, rad);
        ret = 1;
        break;
    }
    case SFORM_CRARC2f: {
        crarc2f& crarc = (crarc2f*)shp.sformD;
        cring2f_setRadO_autoscaledRadI(crarc.r, rad);
        ret = 1;
        break;
    }
    case SFORM_SPHEREf: {
        spheref * sphere = (spheref*)shp.sformD;
        sphere.r = rad;
        ret = 1;
        break;
    }
    default: {
        box2f ext;
        if (vis_rdBnd_box2f(shp, ext)) {
            ext.set_dim(dim);
            ret = fit_in(shp, ext, 0);
        } else {
            ret = 0;
        }
        break;
    }
    }

    vis_reg(shp);

    return ret;
}

int
vis_scalexy_self(vis_t& shp, float fx, float fy, bool morph_flag)
{
    int ret = 0;
    box2f ext;
    if (vis_rdBnd_box2f(shp, ext) > 0) {
        box2f extN;			/* new extents */
        box2f_scalexy(ext, fx, fy, extN);
        fit_in(shp, extN, morph_flag);
        ret = 1;
    }
    return ret;
}

int vis_rdCtrlPD(vis_t shp,
                 vec2f& vecs_ret, int * vecsN_ret,
                 float * dims_ret, int * dimsN_ret)
{
    int ret = 0;
    *vecsN_ret = 0;               /* default */
    *dimsN_ret = 0;               /* default */
    switch (shp.sform) {
    case SFORM_BOX2f: {
        *vecsN_ret = 4; *dimsN_ret = 0; ret = 1;
        break;
    }
    case SFORM_ELL2f: {
        *vecsN_ret = 4; *dimsN_ret = 0; ret = 1;
        break;
    }
    case SFORM_POLY2f: {
        poly2f& poly = (poly2f*)shp.sformD;
        *vecsN_ret = poly.n; *dimsN_ret = 0; ret = 1;
        break;
    }
    case SFORM_TRI2f: {
        *vecsN_ret = 3; *dimsN_ret = 0; ret = 1;
        break;
    }
    case SFORM_QUAD2f: {
        *vecsN_ret = 4; *dimsN_ret = 0; ret = 1;
        break;
    }
    case SFORM_SQR2f: {
        *vecsN_ret = 1; *dimsN_ret = 1; ret = 1;
        break;
    }
    case SFORM_CIR2f: {
        *vecsN_ret = 1; *dimsN_ret = 1; ret = 1;
        break;
    }
    case SFORM_CRING2f: {
        *vecsN_ret = 1; *dimsN_ret = 2; ret = 1;
        break;
    }
    case SFORM_CRARC2f: {
        *vecsN_ret = 1; *dimsN_ret = 4; ret = 1;
        break;
    }
    case SFORM_SPHEREf: {
        *vecsN_ret = 1; *dimsN_ret = 1; ret = 1;
        break;
    }
    case SFORM_RBOX2f_ell: {
        *vecsN_ret = 6; *dimsN_ret = 0; ret = 1;
        break;
    }
    case SFORM_RBOX2f_lin: {
        *vecsN_ret = 6; *dimsN_ret = 0; ret = 1;
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }
    return ret;
}

int
vis_rdCtrlPM_SFORM(vis_t shp, float dirang,
                   vec2f mpos, uint8_t * mask,
                   bool cover_flag, bool set_flag)
{
#ifndef NDEBUG
    if (not shp.sformD) { PWARN("%s\n", g_shape_warn_zerodat); return -1; }
    if (not mask) { PWARN("mask is zero!\n"); return -1; }
#endif
    int ret = 0;

    if (cover_flag and !vis_includes(shp, dirang, mpos)) { return 0; }
    if (dirang) { PTODO("Handle shape rotation dirang:%f\n", dirang); }

    uint32_t rsz_i = UINT32_MAX;	/* index to nearest resize region */

    switch (shp.sform) {
    case SFORM_BOX2f: {
        const box2f& box = (box2f*)shp.sformD;
        ret = box2f_rdCtrlPM_SFORM(box, mpos, mask, rsz_i, set_flag);
        break;
    }
    case SFORM_ELL2f: {
        const ell2f& ell = (ell2f*)shp.sformD;
        ret = box2f_rdCtrlPM_SFORM(ell, mpos, mask, rsz_i, set_flag);
        break;
    }
    case SFORM_POLY2f: {
        const poly2f& poly = (poly2f*)shp.sformD;
        rsz_i = find_nearest(poly.v, poly.n, mpos);
        if (rsz_i >= 256) {
            PWARN("rsz_i >= 256. We need to extend precision of mask.");
        }
        *mask = int8_set_bitTo(*mask, rsz_i, set_flag);
        ret = 1;
        break;
    }
    case SFORM_TRI2f: {
        const tri2f& tri = (tri2f*)shp.sformD;
        rsz_i = find_nearest(tri.p, 3, mpos);
        *mask = int8_set_bitTo(*mask, rsz_i, set_flag);
        ret = 1;
        break;
    }
    case SFORM_QUAD2f: {
        const vec2f& quad = (vec2f*)shp.sformD;
        rsz_i = find_nearest(quad, 4, mpos);
        *mask = int8_set_bitTo(*mask, rsz_i, set_flag);
        ret = 1;
        break;
    }
    case SFORM_SQR2f: {
        const sqr2f& sqr = (sqr2f*)shp.sformD;
        rsz_i = (vec2f_dist2(sqr.c, mpos) < 0.5f& sqr.r) ? 0 : 1;
        *mask = int8_set_bitTo(*mask, rsz_i, set_flag);
        ret = 1;
        break;
    }
    case SFORM_CIR2f: {
        const cir2f& cir = (cir2f*)shp.sformD;
        const float cd = vec2f_dist2(cir.c, mpos); /* center distance */
        rsz_i = (cd < 0.5f& cir.r) ? 0 : 1;
        *mask = int8_set_bitTo(*mask, rsz_i, set_flag);
        ret = 1;
        break;
    }
    case SFORM_CRING2f: {
        const cring2f& cring = (cring2f*)shp.sformD;
        const float cd = vec2f_dist2(cring.c, mpos); /* center distance */
        if      (cd < 0.5f& cring.ri)                { rsz_i = 0; }
        else if (cd < mean(cring.ri, cring.ro)) { rsz_i = 1; }
        else                                           { rsz_i = 2; }
        *mask = int8_set_bitTo(*mask, rsz_i, set_flag);
        ret = 1;
        break;
    }
    case SFORM_CRARC2f: {
        const crarc2f& crarc = (crarc2f*)shp.sformD;
        const float cd = vec2f_dist2(crarc.r.c, mpos); /* center distance */
        if      (cd < 0.5f& crarc.r.ri)                 { rsz_i = 0; }
        else if (cd < mean(crarc.r.ri, crarc.r.ro)) { rsz_i = 1; }
        else                                             { rsz_i = 2; }
        *mask = int8_set_bitTo(*mask, rsz_i, set_flag);
        ret = 1;
        break;
    }
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin: {
        rbox2f& rbox = (rbox2f*)shp.sformD;
        ret = rbox2f_rdCtrlPM_SFORM(rbox, mpos, mask, rsz_i, set_flag);
        break;
    }
    case SFORM_SPHEREf: {
        const spheref * sphere = (spheref*)shp.sformD;
        vec3f mpos3 = { mpos.x, mpos.y, 0.0f };	      /* z defaults to 0 */
        const float cd = vec3f_dist2(sphere.c, mpos3); /* center distance */
        if      (cd < 0.5f& sphere.r) { rsz_i = 0; }
        else if (cd < sphere.r)        { rsz_i = 1; }
        else                            { rsz_i = 2; }
        *mask = int8_set_bitTo(*mask, rsz_i, set_flag);
        ret = 1;
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: {			/* default to bounding box */
        box2f ext; vis_rdBnd_box2f(shp, ext); /* read extents */
        vec2f dim; box2f_rdDim(ext, dim);
        box2f rlb; box2f_scale(ext, 1.0f / 3.0f, rlb); /* resize limiter box */
        ret = 0;
        break;
    }
    }

    return ret;
}

int
vis_rdCtrlPM_SFILL(vis_t shp, float dirang,
                   vec2f mpos, uint8_t * mask,
                   bool cover_flag, bool set_flag)
{
#ifndef NDEBUG
    if (not shp.sformD) { PWARN("%s\n", g_shape_warn_zerodat); return -1; }
    if (not mask) { PWARN("mask is zero!\n"); return -1; }
#endif
    int ret = 0;

    if (cover_flag and !vis_includes(shp, dirang, mpos)) { return 0; }
    if (dirang) { PTODO("Handle shape rotation dirang:%f\n", dirang); }

    uint32_t rsz_i = UINT32_MAX;	/* index to nearest resize region */

    switch (shp.sform) {
    case SFORM_BOX2f: {
        const box2f& box = (box2f*)shp.sformD;
        ret = box2f_rdCtrlPM_SFILL(box, shp.sfill, mpos, mask, rsz_i, set_flag);
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }

    return ret;
}

int
vis_shiftCtrlP(vis_t& shp, float dirang,
               vec2f mpos, vec2f mshift,
               vec2f mindim, const uint8_t * mask,
               vec2f PAR, bool outer_flag)
{
#ifndef NDEBUG
    if (not shp.sformD) { PWARN("%s\n", g_shape_warn_zerodat); return -1; }
    if (not mindim) { PWARN("mindim is zero!\n"); return -1; }
    if (not mask) { PWARN("mask is zero!\n"); return -1; }
#endif
    int ret = 0;

    vis_unreg(shp);

    if (dirang) { PTODO("Handle shape rotation dirang:%f\n", dirang); }

    switch (shp.sform) {
    case SFORM_BOX2f: {
        box2f& box = (box2f*)shp.sformD;
        box = shiftCtrlP(box, mpos, mshift, mindim, mask, PAR);
        break;
    }
    case SFORM_ELL2f: {
        ell2f& ell = (ell2f*)shp.sformD;
        ell = shiftCtrlP(ell, mpos, mshift, mindim, mask, PAR);
        break;
    }
    case SFORM_POLY2f: {
        poly2f& poly = (poly2f*)shp.sformD;
        if (mask) {
            ret = shiftCtrlP(poly.v, poly.n, mshift, mask);
        }
        break;
    }
    case SFORM_TRI2f: {
        tri2f& tri = (tri2f*)shp.sformD;
        if (mask) {
            ret = shiftCtrlP(tri.p, 3, mshift, mask);
        }
        break;
    }
    case SFORM_QUAD2f: {
        vec2f& quad = (vec2f*)shp.sformD;
        if (mask) {
            ret = shiftCtrlP(quad, 4, mshift, mask);
        }
        break;
    }
    case SFORM_SQR2f: {
        sqr2f& sqr = (sqr2f*)shp.sformD;
        sqr = shiftCtrlP(sqr, mpos, mshift, mindim, mask);
        break;
    }
    case SFORM_CIR2f: {
        cir2f& cir = (cir2f*)shp.sformD;
        cir = shiftCtrlP(cir, mpos, mshift, mindim, mask);
        break;
    }
    case SFORM_CRING2f: {
        cring2f& cring = (cring2f*)shp.sformD;
        cring = shiftCtrlP(cring, mpos, mshift, mindim, mask);
        break;
    }
    case SFORM_CRARC2f: {
        crarc2f& crarc = (crarc2f*)shp.sformD;
        crarc = shiftCtrlP(crarc, mpos, mshift, mindim, mask);
        break;
    }
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin: {
        rbox2f& rbox = (rbox2f*)shp.sformD;
        rbox = shiftCtrlP(rbox, mpos, mshift, mindim, mask);
        break;
    }
    case SFORM_SPHEREf: {
        spheref * sphere = (spheref*)shp.sformD;
        const vec3f mpos3 = { mpos.x, mpos.y, 0.0f }; /* z defaults to 0 */
        const vec3f mshift3 = { mpos.x, mpos.y, 0.0f }; /* z defaults to 0 */
        const vec3f mindim3 = { mpos.x, mpos.y, 0.0f }; /* z defaults to 0 */
        sphere = shiftCtrlP(sphere, mpos3, mshift3, mindim3, mask);
        if (PAR) { PTODO("Respect PAR: [%f:%f]\n", PAR.x, PAR.y); }
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: {			/* default to bounding box */
        box2f ext; vis_rdBnd_box2f(shp, ext); /* read extents */
        vec2f dim; box2f_rdDim(ext, dim);
        box2f rlb; box2f_scale(ext, M_1_3, rlb); /* resize limiter box */

        if (1) {
            PTEST("default now reuses shiftCtrlP(). Remove old alternative code if this works.\n");
            ext = shiftCtrlP(ext, mpos, mshift, mindim, mask, PAR);
        } else {
            if (mshift) {
                if (mpos.x < rlb.l(0)) {
                    if (mindim) {
                        if (dim(0) - mshift.x > mindim.x) { ext.l(0) += mshift.x; }
                    }
                } else if (mpos.x > rlb.u(0)) {
                    if (mindim) {
                        if (dim(0) + mshift.x > mindim.x) { ext.u(0) += mshift.x; }
                    }
                } else {			/* default to translate */
                    ext.l(0) += mshift.x;
                    ext.u(0) += mshift.x;
                }
                if (mpos.y < rlb.l(1)) {
                    if (mindim) {
                        if (dim(1) - mshift.y > mindim.y) { ext.l(1) += mshift.y; }
                    }
                } else if (mpos.y > rlb.u(1)) {
                    if (mindim) {
                        if (dim(1) + mshift.y > mindim.y) { ext.u(1) += mshift.y; }
                    }
                } else {			/* default to translate */
                    ext.l(1) += mshift.y;
                    ext.u(1) += mshift.y;
                }
            }
        }

        ret = fit_in(shp, ext, 0);
        break;
    }
    }

    vis_reg(shp);

    return ret;
}

/*! Change the \c SFORM of the shape \p shp to be \p next_sformtype. */
static int
vis_reSFORM(vis_t& shp, SFORM_t next_sformtype)
{
    int ret = 0;
    if (shp.sformD and
        shp.sform != next_sformtype) { /* if we morph the shape */
        vis_clear_SFORM(shp);
        shp.sformD = 0;
    }
    shp.sform = next_sformtype;
    return ret;
}

int
vis_setBOX(vis_t& shp, box2f box)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_BOX2f);
    if (not shp.sformD) { shp.sformD = (box2f*)malloc(sizeof(box2f)); } /* if morph */
    box2f_set_copy((box2f*)shp.sformD, box);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setELL(vis_t& shp, ell2f ell_arg)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_ELL2f);
    if (not shp.sformD) { shp.sformD = (box2f*)malloc(sizeof(box2f)); } /* if morph */
    box2f_set_copy((box2f*)shp.sformD, ell_arg);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setRBOX_ELL(vis_t& shp,
                box2f box, vec2f rad, CORNMASK_t cornmask)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_RBOX2f_ell);
    if (not shp.sformD) { shp.sformD = (rbox2f*)malloc(sizeof(rbox2f)); } /* if morph */

    rbox2f& rbox = (rbox2f*)shp.sformD;
    rbox.b = *box;
    rbox.r = *rad;
    rbox.cornmask = cornmask;
    ret = 1;

    vis_reg(shp);
    return ret;
}

int
vis_setRBOX_LIN(vis_t& shp,
                box2f box, vec2f rad, CORNMASK_t cornmask)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_RBOX2f_lin);
    if (not shp.sformD) { shp.sformD = (rbox2f*)malloc(sizeof(rbox2f)); } /* if morph */

    rbox2f& rbox = (rbox2f*)shp.sformD;
    rbox.b = *box;
    rbox.r = *rad;
    rbox.cornmask = cornmask;
    ret = 1;

    vis_reg(shp);
    return ret;
}

int
vis_setTRI(vis_t& shp, const vec2f tri[3])
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_TRI2f);
    if (not shp.sformD) { shp.sformD = malloc(sizeof(tri2f)); } /* if morph */
    tri2f_set_Points((tri2f*)shp.sformD, tri[0], tri[1], tri[2]);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setQUAD(vis_t& shp, const vec2f quad[4])
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_QUAD2f);
    if (not shp.sformD) { shp.sformD = malloc(4 * sizeof(vec2f)); } /* if morph */
    memcpy((vec2f*)shp.sformD, quad[0], 4 * sizeof(vec2f));
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setPOLY(vis_t& shp, vec2f vtx, size_t vtxN)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_POLY2f);
    /* calloc needes zero */
    if (not shp.sformD) { shp.sformD = calloc(sizeof(vis_t), 1); } /* if morph */
    poly2f_set((poly2f*)shp.sformD, vtx, vtxN);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setSQR(vis_t& shp, sqr2f sqr)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_SQR2f);
    if (not shp.sformD) { shp.sformD = malloc(sizeof(sqr2f)); } /* if morph */
    sqr2f_set_copy((sqr2f*)shp.sformD, sqr);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setCIR(vis_t& shp, cir2f cir)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_CIR2f);
    if (not shp.sformD) { shp.sformD = malloc(sizeof(cir2f)); } /* if morph */
    cir2f_set_copy((cir2f*)shp.sformD, cir);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setCRING(vis_t& shp, cring2f cring)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_CRING2f);
    if (not shp.sformD) { shp.sformD = malloc(sizeof(cring2f)); } /* if morph */
    cring2f_set_copy((cring2f*)shp.sformD, cring);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setRSEG(vis_t& shp, crarc2f crarc)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_CRARC2f);
    if (not shp.sformD) { shp.sformD = malloc(sizeof(crarc2f)); } /* if morph */
    crarc2f_set_copy((crarc2f*)shp.sformD, crarc);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setBOX3(vis_t& shp, box3f box)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_BOX3f);
    if (not shp.sformD) { shp.sformD = malloc(sizeof(box3f)); } /* if morph */
    box3f_set_copy((box3f*)shp.sformD, box);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_setSPHERE(vis_t& shp, const spheref * sphere)
{
    int ret = 0;
    vis_unreg(shp);
    vis_reSFORM(shp, SFORM_SPHEREf);
    if (not shp.sformD) { shp.sformD = malloc(sizeof(spheref)); } /* if morph */
    spheref_set_copy((spheref*)shp.sformD, sphere);
    ret = 1;
    vis_reg(shp);
    return ret;
}

int
vis_move(vis_t& shp, vec2f shift)
{
    int ret = 0;

    vis_unreg(shp);

    switch (shp.sform) {
    case SFORM_BOX2f:
    case SFORM_ELL2f:
        if (shp.sformD) {
            box2f& rect = (box2f*)shp.sformD;
            rect += shift;
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_POLY2f:
        if (shp.sformD) {
            poly2f& poly = (poly2f*)shp.sformD;
            poly += shift;
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_TRI2f:
        if (shp.sformD) {
            vec2f& tri = (vec2f*)shp.sformD;
            vec2f_add(tri[0], shift);
            vec2f_add(tri[1], shift);
            vec2f_add(tri[2], shift);
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_QUAD2f:
        if (shp.sformD) {
            vec2f& quad = (vec2f*)shp.sformD;
            vec2f_add(quad[0], shift);
            vec2f_add(quad[1], shift);
            vec2f_add(quad[2], shift);
            vec2f_add(quad[3], shift);
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_SQR2f:
        if (shp.sformD) {
            sqr2f& sqr = (sqr2f*)shp.sformD;
            sqr += shift;
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_CIR2f:
        if (shp.sformD) {
            cir2f& cir = (cir2f*)shp.sformD;
            cir += shift;
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_CRING2f:
        if (shp.sformD) {
            cring2f& cring = (cring2f*)shp.sformD;
            cring += shift;
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_CRARC2f:
        if (shp.sformD) {
            crarc2f& crarc = (crarc2f*)shp.sformD;
            crarc += shift;
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_SPHEREf:
        if (shp.sformD) {
            spheref* sphere = (spheref*)shp.sformD;
            vec3f shift3 = { shift.x, shift.y, 0.0f }; /* z defaults to zero */
            sphere.r() += shift3;
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin:
        if (shp.sformD) {
            rbox2f& rbox = (rbox2f*)shp.sformD;
            rbox += shift;
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }

    vis_reg(shp);

    return ret;
}

void
vis_init_undefined(vis_t& shp)
{
    shp.sform = SFORM_undefined_;
    shp.sformD = 0;
    shp.sfill = SFILL_SINGLE;	/* default to single color */
    shp.sfillD.single = COLOR_BLACK; /* default to black color */
    shp.slayer = 0;
    shp.sframe = SFRAME_NONE;
}

void
vis_init_vis(vis_t& shp, vis_t b)
{
    vis_init_undefined(shp);
    vis_setShape(shp, b);
}

/*! Init shape but leave data field empty. */
static void
vis_init_empty(vis_t& shp,
               SFORM_t sform, color_t shcol, layer_t layer)
{
    shp.sform = sform;
    shp.sformD = 0;
    shp.sfill = SFILL_SINGLE;
    shp.sfillD.single = shcol;
    shp.slayer = layer;
    shp.sframe = SFRAME_NONE;
}

void
vis_init(vis_t& shp,
         SFORM_t sform, box2f ext, color_t shcol, layer_t layer)
{
    vis_init_empty(shp, sform, shcol, layer);
    fit_in(shp, ext, 0);
}

void
vis_initBOX(vis_t& shp,
            box2f box, color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_BOX2f, shcol, layer);
    vis_setBOX(shp, box);
}

void
vis_initELL(vis_t& shp,
            ell2f ell, color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_ELL2f, shcol, layer);
    vis_setELL(shp, ell);
}

void
vis_initTRI(vis_t& shp,
            const vec2f tri[3], color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_TRI2f, shcol, layer);
    vis_setTRI(shp, tri);
}

void
vis_initQUAD(vis_t& shp,
             const vec2f quad[4], color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_QUAD2f, shcol, layer);
    vis_setQUAD(shp, quad);
}

void
vis_initRBOX_ELL(vis_t& shp,
                 box2f box, vec2f rad,
                 color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_RBOX2f_ell, shcol, layer);
    vis_setRBOX_ELL(shp, box, rad, CORNMASK_FULL);
}

void
vis_initRBOX_LIN(vis_t& shp,
                 box2f box, vec2f rad,
                 color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_RBOX2f_lin, shcol, layer);
    vis_setRBOX_LIN(shp, box, rad, CORNMASK_FULL);
}

void
vis_initSQR(vis_t& shp,
            sqr2f sqr,
            color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_SQR2f, shcol, layer);
    vis_setSQR(shp, sqr);
}

void
vis_initCIR(vis_t& shp,
            cir2f cir,
            color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_CIR2f, shcol, layer);
    vis_setCIR(shp, cir);
}

void
vis_initCRING(vis_t& shp,
              cring2f cring,
              color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_CRING2f, shcol, layer);
    vis_setCRING(shp, cring);
}

void
vis_initRSEG(vis_t& shp,
             crarc2f crarc,
             color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_CRARC2f, shcol, layer);
    vis_setRSEG(shp, crarc);
}

void
vis_initSPHERE(vis_t& shp,
               const spheref * sphere,
               color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_SPHEREf, shcol, layer);
    vis_setSPHERE(shp, sphere);
}

void
vis_initPOLY(vis_t& shp,
             vec2f vtx, size_t vtxN,
             color_t shcol, layer_t layer)
{
    vis_init_empty(shp, SFORM_POLY2f, shcol, layer);
    vis_setPOLY(shp, vtx, vtxN);
}

/* ---------------------------- Group Separator ---------------------------- */

void vis_init_comb2(vis_t& shp, SFORM_t sform,
                    vis_t& a, vis_t& b)
{
    vis_unreg(shp);

    shp.sfill = SFILL_SINGLE;	/* default to single color */
    shp.sfillD.single = COLOR_BLACK; /* default to black color */
    shp.slayer = 0;

    if (not SFORM_is_Combinator(sform)) { PERR("sform is not a Combinator\n"); }
    shp.sform = sform;
    vis_t ** comb2 = malloc(2 * sizeof(vis_t*));
    comb2[0] = a;
    comb2[1] = b;
    shp.sformD = comb2;

    shp.sframe = SFRAME_NONE;

    vis_reg(shp);
}

void vis_init_comb2_UNION(vis_t& shp, vis_t& a, vis_t& b)
{
    vis_init_comb2(shp, SFORM_comb2_UNION, a, b);
}

void vis_init_comb2_ISECT(vis_t& shp, vis_t& a, vis_t& b)
{
    vis_init_comb2(shp, SFORM_comb2_ISECT, a, b);
}

void vis_init_comb2_DIFF(vis_t& shp, vis_t& a, vis_t& b)
{
    vis_init_comb2(shp, SFORM_comb2_DIFF, a, b);
}

/* ---------------------------- Group Separator ---------------------------- */

box2f
vis_outer_box(vis_t shp)
{
    box2f ext;
#ifndef NDEBUG
    if (shp.sformD == 0) { PWARN("%s\n", g_shape_warn_zerodat); return ext; }
#endif
    switch (shp.sform) {
    case SFORM_BOX2f:
    case SFORM_ELL2f:
        if (shp.sformD) {
            box2f& rect = (box2f*)shp.sformD;
            ext = *rect;
        }
        break;
    case SFORM_POLY2f:
        if (shp.sformD) {
            poly2f& poly = (poly2f*)shp.sformD;
            poly2f_rdBnd_box2f(poly, ext);
        }
        break;
    case SFORM_TRI2f:
        if (shp.sformD) {
            tri2f& tri = (tri2f*)shp.sformD;
            tri2f_rdBnd_box2f(tri, ext);
        }
        break;
    case SFORM_QUAD2f:
        if (shp.sformD) {
            vec2f& quad = (vec2f*)shp.sformD;
            vec2f_quad_rdBnd_box2f(quad, ext);
        }
        break;
    case SFORM_SQR2f:
        if (shp.sformD) {
            sqr2f& sqr = (sqr2f*)shp.sformD;
            sqr2f_rdBnd_box2f(sqr, ext);
        }
        break;
    case SFORM_CIR2f:
        if (shp.sformD) {
            cir2f& cir = (cir2f*)shp.sformD;
            cir2f_rdBnd_box2f(cir, ext);
        }
        break;
    case SFORM_CRING2f:
        if (shp.sformD) {
            cring2f& cring = (cring2f*)shp.sformD;
            cring2f_rdBnd_box2f(cring, ext);
        }
        break;
    case SFORM_CRARC2f:
        if (shp.sformD) {
            crarc2f& crarc = (crarc2f*)shp.sformD;
            crarc2f_rdBnd_box2f(crarc, ext);
        }
        break;
    case SFORM_SPHEREf:
        if (shp.sformD) {
            spheref * sphere = (spheref*)shp.sformD;
            box3f ext3;
            spheref_rdBnd_box2f(sphere, ext3);
            box2f_set(ext,
                      ext3.l(0), ext3.l(1),
                      ext3.u(0), ext3.u(1)); /* z is ignored */
        }
        break;
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin:
        if (shp.sformD) {
            rbox2f& rbox = (rbox2f*)shp.sformD;
            ext = extend(rbox.b, rbox.r); /* extend box with radius */
        }
        break;
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); break;
    }

    return ext;
}

vec2f
vis_cen(vis_t shp)
{
    vec2f cen;
#ifndef NDEBUG
    if (shp.sformD == 0) { PWARN("%s\n", g_shape_warn_zerodat); return cen; }
#endif

    switch (shp.sform) {
    case SFORM_BOX2f:
    case SFORM_ELL2f:
        if (shp.sformD) {
            box2f& rect = (box2f*)shp.sformD;
            cen = rect.cen();
        }
        break;
    case SFORM_POLY2f:
        if (shp.sformD) {
            poly2f& poly = (poly2f*)shp.sformD;
            cen = poly.cen();
        }
        break;
    case SFORM_TRI2f:
        if (shp.sformD) {
            tri2f& tri = (tri2f*)shp.sformD;
            cen = tri.mid();
        }
        break;
    case SFORM_QUAD2f:
        if (shp.sformD) {
            vec2f& quad = (vec2f*)shp.sformD;
            vec2farray_rdCen(quad, 4, cen);
        }
        break;
    case SFORM_SQR2f:
        if (shp.sformD) {
            sqr2f& sqr = (sqr2f*)shp.sformD;
            cen = sqr.cen();
        }
        break;
    case SFORM_CIR2f:
        if (shp.sformD) {
            cir2f& cir = (cir2f*)shp.sformD;
            cen = cir.cen();
        }
        break;
    case SFORM_CRING2f:
        if (shp.sformD) {
            cring2f& cring = (cring2f*)shp.sformD;
            cen = cring.cen();
        }
        break;
    case SFORM_CRARC2f:
        if (shp.sformD) {
            crarc2f& crarc = (crarc2f*)shp.sformD;
            cen = crarc.cen();
        }
        break;
    case SFORM_SPHEREf:
        if (shp.sformD) {
            spheref * sphere = (spheref*)shp.sformD;
            cen = sphere.cen();
        }
        break;
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin:
        if (shp.sformD) {
            rbox2f& rbox = (rbox2f*)shp.sformD;
            cen = rbox.cen();
        }
        break;
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }
    return cen;
}

box2f
vis_getBnd_box2f(vis_t shp)
{
    box2f ext;
    vis_rdBnd_box2f(shp, ext);
    return ext;
}

vec2f
vis_getBLim(vis_t shp, int i)
{
    box2f ext;
    vis_rdBnd_box2f(shp, ext);
    return box2f_at(ext, i);
}

void
vis_rdBDims(vis_t shp, vec2f& dim)
{
    box2f ext;
    vis_rdBnd_box2f(shp, ext);
    box2f_rdDim(ext, dim);
}

vec2f
vis_getBDims(vis_t shp)
{
    vec2f a;
    vis_rdBDims(shp, a);
    return a;
}

/* ---------------------------- Group Separator ---------------------------- */

int
vis_area(vis_t shp)
{
    float area = nan;
#ifndef NDEBUG
    if (shp.sformD == 0) { PWARN("%s\n", g_shape_warn_zerodat); return area; }
#endif
    switch (shp.sform) {
    case SFORM_BOX2f:
    case SFORM_ELL2f:
        if (shp.sformD) {
            box2f& rect = (box2f*)shp.sformD;
            area = rect.area();
        }
        break;
    case SFORM_POLY2f:
    case SFORM_TRI2f:
    case SFORM_QUAD2f:
        if (shp.sformD) {
            box2f ext;
            if ((ret = vis_rdBnd_box2f(shp, ext)) == 1) {
                PTODO("Calculate exact area.\n");
                area = ext.area();
            }
        }
        break;
    case SFORM_SQR2f:
        if (shp.sformD) {
            sqr2f& sqr = (sqr2f*)shp.sformD;
            area = sqr.area();
        }
        break;
    case SFORM_CIR2f:
        if (shp.sformD) {
            cir2f& cir = (cir2f*)shp.sformD;
            area = cir.area();
        }
        break;
    case SFORM_CRING2f:
        if (shp.sformD) {
            cring2f& cring = (cring2f*)shp.sformD;
            area = cring.area();
        }
        break;
    case SFORM_CRARC2f:
        if (shp.sformD) {
            crarc2f& crarc = (crarc2f*)shp.sformD;
            area = crarc.area();
        }
        break;
    case SFORM_SPHEREf:
        if (shp.sformD) {
            box2f ext;
            if ((ret = vis_rdBnd_box2f(shp, ext)) == 1) {
                PTODO("Calculate exact area.\n");
                area = ext.area();
            }
        }
        break;
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin:
        if (shp.sformD) {
            rbox2f& rbox = (rbox2f*)shp.sformD;
            area = rbox.area();
        }
        break;
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

box2f inner_box(vis_t shp)
{
    box2f a;
#ifndef NDEBUG
    if (shp.sformD == 0) { PWARN("%s\n", g_shape_warn_zerodat); box2f_set_scalar(a, 0); return a; }
#endif
    int ret = 0;

    switch (shp.sform) {
    case SFORM_BOX2f:
        if (shp.sformD) {
            box2f& rect = (box2f*)shp.sformD;
            *a = *rect;
            ret = 1;
        }
        break;
    case SFORM_SQR2f:
        if (shp.sformD) {
            sqr2f& sqr = (sqr2f*)shp.sformD;
            *a = sqr.inner_box();
            ret = 1;
        }
        break;
    case SFORM_CIR2f:
        if (shp.sformD) {
            cir2f& cir = (cir2f*)shp.sformD;
            *a = cir.inner_box(1.0f);
            ret = 1;
        }
        break;
    case SFORM_CRING2f:
        if (shp.sformD) {
            cring2f& cring = (cring2f*)shp.sformD;
            *a = cring.inner_box();
            ret = 1;
        }
        break;
    case SFORM_CRARC2f:
        if (shp.sformD) {
            crarc2f& crarc = (crarc2f*)shp.sformD;
            *a = crarc.inner_box();
            ret = 1;
        }
        break;
    case SFORM_ELL2f:
        if (shp.sformD) {
            ell2f& ell = (ell2f*)shp.sformD;
            *a = ell.inner_box();
            ret = 1;
        }
        break;
    case SFORM_POLY2f:
    case SFORM_TRI2f:
    case SFORM_QUAD2f:
        /* PWARN("Defaulting to Bounding Box (BBox)\n"); */
        ret = vis_rdBnd_box2f(shp, a);
        break;
    case SFORM_SPHEREf:
        if (shp.sformD) {
            spheref * sphere = (spheref*)shp.sformD;
            box3f a3 = sphere.inner_box();
            box2f_set(a, a3.l(0), a3.l(1), a3.u(0), a3.u(1)); /* z is ignored */
            ret = 1;
        }
        break;
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin:
        if (shp.sformD) {
            rbox2f& rbox = (rbox2f*)shp.sformD;
            *a = rbox.b;
            ret = 1;
        }
        break;
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }

    return a;
}

int vis_rdICenRad(vis_t shp, vec2f& cen, vec2f& rad)
{
    int ret = 0;
    switch (shp.sform) {
    case SFORM_CIR2f: {
        if (shp.sformD) {
            cir2f& cir = (cir2f*)shp.sformD;
            *cen = cir.c;
            rad.x = rad.y = cir.r;
            ret = 1;
        }
        break;
    }
    case SFORM_CRING2f: {
        if (shp.sformD) {
            cring2f& cring = (cring2f*)shp.sformD;
            *cen = cring.c;
            rad.x = rad.y = cring.ro;
            ret = 1;
        }
        break;
    }
    case SFORM_CRARC2f: {
        if (shp.sformD) {
            crarc2f& crarc = (crarc2f*)shp.sformD;
            *cen = crarc.r.c;
            rad.x = rad.y = crarc.r.ro;
            ret = 1;
        }
        break;
    }
    case SFORM_SPHEREf: {
        if (shp.sformD) {
            spheref * sphere = (spheref*)shp.sformD;
            vec2f_set(cen, sphere.c(0), sphere.c(2)); /* z is ignored */
            rad.x = rad.y = sphere.r;
            ret = 1;
        }
        break;
    }
    default: {
        box2f ext;
        if ((ret = vis_rdBnd_box2f(shp, ext)) == 1) {
            cen = ext.cen();
            rad = ext.rad();
        }
        break;
    }
    }

    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

bool
vis_overlap(vis_t shp, box2f a)
{
#ifndef NDEBUG
    if (shp.sformD == 0) { PWARN("%s\n", g_shape_warn_zerodat); return 0; }
#endif
    int ret = 0;
    box2f ext;

    switch (shp.sform) {
    case SFORM_BOX2f:
        ret = box2f_overlap((const box2f*)shp.sformD, a);
        break;
    case SFORM_ELL2f:
    case SFORM_POLY2f:
    case SFORM_TRI2f:
    case SFORM_QUAD2f:
        PWARN("Using approximate bounding box!\n");
        vis_rdBnd_box2f(shp, ext);
        ret = box2f_overlap(ext, a);
        break;
    case SFORM_SQR2f:
        ret = sqr2f_overlap((const sqr2f*)shp.sformD, a);
        break;
    case SFORM_CIR2f:
        ret = cir2f_overlap((const cir2f*)shp.sformD, a);
        break;
    case SFORM_CRING2f:
        ret = cring2f_overlap((const cring2f*)shp.sformD, a);
        break;
    case SFORM_CRARC2f:
        ret = crarc2f_overlap((const crarc2f*)shp.sformD, a);
        break;
    case SFORM_SPHEREf: {
        ret = spheref_overlap((const spheref*)shp.sformD, a);
        break;
    }
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin: {
        PWARN("Using approximate bounding box!\n");
        vis_rdBnd_box2f(shp, ext);
        ret = box2f_overlap(ext, a);
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }

    return ret;
}

bool
vis_inside_box2f(vis_t shp, box2f a)
{
#ifndef NDEBUG
    if (shp.sformD == 0) { PWARN("%s\n", g_shape_warn_zerodat); return 0; }
#endif
    int ret = 0;
    box2f ext;

    switch (shp.sform) {
    case SFORM_BOX2f:
        ret = box2f_inside_box2f((const box2f*)shp.sformD, a);
        break;
    case SFORM_ELL2f:
    case SFORM_POLY2f:
    case SFORM_TRI2f:
    case SFORM_QUAD2f:
    case SFORM_SQR2f:
    case SFORM_CIR2f:
    case SFORM_CRING2f:
    case SFORM_CRARC2f:
    case SFORM_SPHEREf:
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin: {
        /** \NOTE this is exact actually! */
        vis_rdBnd_box2f(shp, ext);
        ret = box2f_inside_box2f(ext, a);
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }

    return ret;
}

bool
vis_includes(vis_t shp, float dirang, vec2f a)
{
#ifndef NDEBUG
    if (shp.sformD == 0) { PWARN("%s\n", g_shape_warn_zerodat); return 0; }
#endif
    int ret = 0;

    switch (shp.sform) {
    case SFORM_BOX2f: {
        const box2f& box = (box2f*)shp.sformD;
        ret = box2f_includes(box, a);
        break;
    }
    case SFORM_ELL2f: {
        const ell2f& ell = (box2f*)shp.sformD;
        ret = ell2f_includes(ell, dirang, a, FALSE);
        break;
    }
    case SFORM_POLY2f: {
        const poly2f& poly = (poly2f*)shp.sformD;
        ret = vec2f_poly_includes(poly.v, poly.n, a);
        break;
    }
    case SFORM_TRI2f: {
        const tri2f& tri = (tri2f*)shp.sformD;
        ret = vec2f_poly_includes(tri.p, 3, a);
        break;
    }
    case SFORM_QUAD2f: {
        const vec2f& quad = (vec2f*)shp.sformD;
        ret = vec2f_poly_includes(quad, 3, a);
        break;
    }
    case SFORM_SQR2f: {
        const sqr2f& sqr = (sqr2f*)shp.sformD;
        ret = sqr2f_includes(sqr, a);
        break;
    }
    case SFORM_CIR2f: {
        const cir2f& cir = (cir2f*)shp.sformD;
        ret = cir2f_includes(cir, a);
        break;
    }
    case SFORM_CRING2f: {
        const cring2f& cring = (cring2f*)shp.sformD;
        ret = cring2f_includes(cring, a);
        break;
    }
    case SFORM_CRARC2f: {
        const crarc2f& crarc = (crarc2f*)shp.sformD;
        ret = crarc2f_includes(crarc, a);
        break;
    }
    case SFORM_SPHEREf: {
        const spheref * sphere = (spheref*)shp.sformD;
        vec3f a3 = { a.x, a.y, 0.0f };
        ret = sphere.includes(a3);
        break;
    }
    case SFORM_RBOX2f_lin:
    case SFORM_RBOX2f_ell: {
        const rbox2f& rbox = (rbox2f*)shp.sformD;
        box2f ext;
        if (vis_rdBnd_box2f(shp, ext)) {
            if (not box2f_beside(ext, a)) {
                ret = rbox2f_includes(rbox, a, shp.sform == SFORM_RBOX2f_lin);
            } else {
                /* fast discardal for points outside bounding box */
            }
        } else {
            PWARN("%s\n", g_shape_warn_bbox);
        }
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }

    return ret;
}

bool
vis_includes(vis_t shp, box2f a)
{
#ifndef NDEBUG
    if (shp.sformD == 0) { PWARN("%s\n", g_shape_warn_zerodat); return 0; }
#endif
    int ret = 0;
    box2f ext;

    switch (shp.sform) {
    case SFORM_BOX2f:
        ret = box2f_includes((const box2f*)shp.sformD, a);
        break;
    case SFORM_POLY2f:
    case SFORM_TRI2f:
    case SFORM_QUAD2f: {
        PWARN("Using approximate bounding box!\n");
        if (vis_rdBnd_box2f(shp, ext)) {
            ret = box2f_includes(ext, a);
        } else {
            PWARN("%s\n", g_shape_warn_bbox);
        }
        break;
    }
    case SFORM_SQR2f: {
        const sqr2f& sqr = (sqr2f*)shp.sformD;
        ret = sqr2f_includes(sqr, a);
        break;
    }
    case SFORM_CIR2f: {
        const cir2f& cir = (cir2f*)shp.sformD;
        ret = cir2f_includes(cir, a);
        break;
    }
    case SFORM_CRING2f: {
        const cring2f& cring = (cring2f*)shp.sformD;
        ret = cring2f_includes(cring, a);
        break;
    }
    case SFORM_CRARC2f: {
        const crarc2f& crarc = (crarc2f*)shp.sformD;
        ret = crarc2f_includes(crarc, a);
        break;
    }
    case SFORM_SPHEREf: {
        const spheref * sphere = (spheref*)shp.sformD;
        ret = spheref_includes(sphere, a); /* z is ignored */
        break;
    }
    case SFORM_RBOX2f_ell:
    case SFORM_RBOX2f_lin: {
        PWARN("Using approximate bounding box!\n");
        if (vis_rdBnd_box2f(shp, ext)) {
            ret = box2f_includes(ext, a);
        } else {
            PWARN("%s\n", g_shape_warn_bbox);
        }
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); ret = -1; break;
    }

    return ret;
}

void
vis_fprint(FILE * stream, vis_t shp)
{
    fprintf(stream, "vis_t: type=%d", shp.sform);

    if (shp.sformD) {
        fprintf(stream, " data=");
        switch (shp.sform) {
        case SFORM_BOX2f: box2f_fprint(stream, (box2f*)shp.sformD); break;
        case SFORM_ELL2f: box2f_fprint(stream, (box2f*)shp.sformD); break;
        case SFORM_POLY2f: poly2f_fprint(stream, (poly2f*)shp.sformD); break;
        case SFORM_TRI2f: tri2f_fprint(stream, (tri2f*)shp.sformD); break;
        case SFORM_QUAD2f: vec2farray_fprint(stream, (vec2f*)shp.sformD, 4); break;
        case SFORM_SQR2f: sqr2f_fprint(stream, (sqr2f*)shp.sformD); break;
        case SFORM_CIR2f: cir2f_fprint(stream, (cir2f*)shp.sformD); break;
        case SFORM_CRING2f: cring2f_fprint(stream, (cring2f*)shp.sformD); break;
        case SFORM_CRARC2f: crarc2f_fprint(stream, (crarc2f*)shp.sformD); break;
        case SFORM_SPHEREf: spheref_fprint(stream, (spheref*)shp.sformD); break;
        case SFORM_RBOX2f_ell:
        case SFORM_RBOX2f_lin: rbox2f_fprint(stream, (rbox2f*)shp.sformD); break;
        case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
        default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); break;
        }
    }

    fprintf(stream, " sfill=");
    switch ((SFILL_t)shp.sfill) {
    case SFILL_SINGLE:
        color_fprint(stream, shp.sfillD.single);
        break;
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA:
        color_pair_fprint(stream, shp.sfillD.grad);
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        cpal_fprint(stream, shp.sfillD.pal);
        break;

    case SFILL_undefined_:
        PWARN("%s sfill:%d\n", g_shape_warn_sfill_nodef, shp.sfill);
        break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }

    fprintf(stream, " layer=%d", shp.slayer);
}

int chv_enc_vis(chv_t * chv, vis_t shp)
{
    int ret = 0;

    /* sform */
    ret += chv_enc_u8(chv, shp.sform);

    /* sfill */
    const SFILL_t sfill = shp.sfill;
    ret += chv_enc_u8(chv, sfill);

    /* sfill data */
    switch (shp.sfill) {
    case SFILL_SINGLE:
        ret += chv_enc_color(chv, shp.sfillD.single);
        break;
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADR:
    case SFILL_GRADA:
        ret += chv_enc_color(chv, shp.sfillD.grad.x);
        ret += chv_enc_color(chv, shp.sfillD.grad.y);
        break;
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA:
        ret += chv_enc_cpal(chv, shp.sfillD.pal);
        break;
    case SFILL_undefined_: PERR("%s\n", g_shape_warn_sfill_nodef); break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }

    /* slayer */
    ret += chv_enc_u32be(chv, shp.slayer);

    /* sframe */
    const uint8_t sframe = shp.sframe;
    ret += chv_enc_u8(chv, sframe);

    /* sform data */
    if (shp.sformD) {
        switch (shp.sform) {
        case SFORM_BOX2f: ret += chv_enc_box2f((box2f*)shp.sformD, chv); break;
        case SFORM_ELL2f: ret += chv_enc_ell2f((ell2f*)shp.sformD, chv); break;
        case SFORM_POLY2f: ret += chv_enc_poly2f((poly2f*)shp.sformD, chv); break;
        case SFORM_TRI2f: ret += chv_enc_tri2f((tri2f*)shp.sformD, chv); break;
        case SFORM_QUAD2f: ret += chv_enc_vec2farray(chv, (vec2f*)shp.sformD, 4); break;
        case SFORM_SQR2f: ret += chv_enc_sqr2f((sqr2f*)shp.sformD, chv); break;
        case SFORM_CIR2f: ret += chv_enc_cir2f((cir2f*)shp.sformD, chv); break;
        case SFORM_CRING2f: ret += chv_enc_cring2f((cring2f*)shp.sformD, chv); break;
        case SFORM_CRARC2f: ret += chv_enc_crarc2f((crarc2f*)shp.sformD, chv); break;
        case SFORM_SPHEREf: ret += chv_enc_spheref((spheref*)shp.sformD, chv); break;
        case SFORM_RBOX2f_ell:
        case SFORM_RBOX2f_lin: ret += chv_enc_rbox2f((rbox2f*)shp.sformD, chv); break;
        case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
        default: PERR("%s sform:%d\n", g_shape_warn_sform, shp.sform); break;
        }
    } else {
        PERR("Not shape data defined\n");
    }

    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int chv_dec_vis(chv_t * chv, vis_t& shp)
{
    int cnt = 0, ret = 0;

    shp.sformD = 0;	/* no data yet => call to vis_set...() below will work */
    /* vis_unreg(shp); */

    /* sform */
    if ((ret = chv_dec_u8(chv, shp.sform)) >= 0) cnt += ret; else return ret;

    /* sfill */
    uint8_t sfill;
    if ((ret = chv_dec_u8(chv, sfill)) >= 0) cnt += ret; else return ret;
    shp.sfill = sfill;

    /* sfill data */
    switch (shp.sfill) {
    case SFILL_SINGLE: {
        color_t single;
        ret += chv_dec_color(chv, single);
        vis_set_SFILL_SINGLE(shp, single);
        break;
    }
    case SFILL_GRADV:
    case SFILL_GRADH:
    case SFILL_GRADA:
    case SFILL_GRADR: {
        color_pair_t grad;
        ret += chv_dec_color(chv, grad(0));
        ret += chv_dec_color(chv, grad(1));
        vis_set_SFILL_GRAD_pair(shp, sfill, grad);
        break;
    }
    case SFILL_PALV:
    case SFILL_PALH:
    case SFILL_PALR:
    case SFILL_PALA: {
        cpal_t cpal; cpal_init(cpal);
        ret += chv_dec_cpal(chv, cpal);
        vis_set_SFILL_PAL(shp, sfill, cpal);
        cpal_clear(cpal);
        break;
    }
    case SFILL_undefined_: PERR("%s\n", g_shape_warn_sfill_nodef); break;
    default: PERR("%s sfill:%d\n", g_shape_warn_sfill, shp.sfill); break;
    }

    /* slayer */
    if ((ret = chv_dec_u32be(chv, shp.slayer)) >= 0) cnt += ret; else return ret;

    /* sframe */
    uint8_t sframe;
    if ((ret = chv_dec_u8(chv, sframe)) >= 0) cnt += ret; else return ret;
    shp.sframe = sframe;

    /* sform data */
    switch (shp.sform) {
    case SFORM_BOX2f: {
        box2f box;
        ret = chv_dec_box2f(box, chv);
        if (ret > 0) vis_setBOX(shp, box);
        break;
    }
    case SFORM_ELL2f: {
        ell2f ell;
        ret = chv_dec_ell2f(ell, chv);
        if (ret > 0) vis_setELL(shp, ell);
        break;
    }
    case SFORM_POLY2f: {
        poly2f poly;
        poly2f_init(poly, 0, 0);
        ret = chv_dec_poly2f(poly, chv);
        if (ret > 0) vis_setPOLY(shp, poly.v, poly.n);
        break;
    }
    case SFORM_TRI2f: {
        tri2f tri;
        ret = chv_dec_tri2f(tri, chv);
        if (ret > 0) vis_setTRI(shp, tri.p);
        break;
    }
    case SFORM_QUAD2f: {
        vec2f quad[4];
        ret = chv_dec_vec2farray(chv, quad, 4);
        if (ret > 0) vis_setQUAD(shp, quad);
        break;
    }
    case SFORM_SQR2f: {
        sqr2f sqr;
        ret = chv_dec_sqr2f(sqr, chv);
        if (ret > 0) vis_setSQR(shp, sqr);
        break;
    }
    case SFORM_CIR2f: {
        cir2f cir;
        ret = chv_dec_cir2f(cir, chv);
        if (ret > 0) vis_setCIR(shp, cir);
        break;
    }
    case SFORM_CRING2f: {
        cring2f cring;
        ret = chv_dec_cring2f(cring, chv);
        if (ret > 0) vis_setCRING(shp, cring);
        break;
    }
    case SFORM_CRARC2f: {
        crarc2f crarc;
        ret = chv_dec_crarc2f(crarc, chv);
        if (ret > 0) vis_setRSEG(shp, crarc);
        break;
    }
    case SFORM_SPHEREf: {
        spheref crarc;
        ret = chv_dec_spheref(crarc, chv);
        if (ret > 0) vis_setSPHERE(shp, crarc);
        break;
    }
    case SFORM_RBOX2f_ell: {
        rbox2f rbox;
        ret = chv_dec_rbox2f(rbox, chv);
        if (ret > 0) vis_setRBOX_ELL(shp, rbox.b, rbox.r, CORNMASK_FULL);
        break;
    }
    case SFORM_RBOX2f_lin: {
        rbox2f rbox;
        ret = chv_dec_rbox2f(rbox, chv);
        if (ret > 0) vis_setRBOX_LIN(shp, rbox.b, rbox.r, CORNMASK_FULL);
        break;
    }
    case SFORM_undefined_: PWARN("%s\n", g_shape_warn_sform_nodef); break;
    default: PWARN("%s sform:%d\n", g_shape_warn_sform, shp.sform); break;
    }
    if (ret >= 0) cnt += ret; else return ret;

    vis_reg(shp);

    return cnt;
}
