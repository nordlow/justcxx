#include "gen_shapes.hpp"
#include "mean.hpp"

void
genLetterX(box2<T> ext, float rxd, vec2<T> wpolyX[8])
{
    auto sep = (ext.u(0) - ext.l(0)) * rxd; /* separation */

    wpolyX[0](0) = ext.l(0);
    wpolyX[0](1) = ext.l(1);
    wpolyX[1](0) = ext.l(0) + sep;
    wpolyX[1](1) = ext.l(1);
    wpolyX[2](0) = ext.u(0);
    wpolyX[2](1) = ext.u(1);
    wpolyX[3](0) = ext.u(0) - sep;
    wpolyX[3](1) = ext.u(1);

    wpolyX[4](0) = ext.u(0) - sep;
    wpolyX[4](1) = ext.l(1);
    wpolyX[5](0) = ext.u(0);
    wpolyX[5](1) = ext.l(1);
    wpolyX[6](0) = ext.l(0) + sep;
    wpolyX[6](1) = ext.u(1);
    wpolyX[7](0) = ext.l(0);
    wpolyX[7](1) = ext.u(1);
}

void
genArrow(box2<T> ext, vec2<T> head[3], vec2<T> tail[4])
{
    auto dim = ext.dim();

    const float tw = dim(0) * M_GOLDRAT; /* tail width */
    const float ts = (dim(0) - tw)/2; /* tail space */
    const float hh = dim(1) * M_GOLDRAT; /* head height */

    head[0](0) = ext.l(0);
    head[0](1) = ext.l(1) + hh;

    head[1](0) = pnw::mean(ext.l(0), ext.u(0));
    head[1](1) = ext.l(1);

    head[2](0) = ext.u(0);
    head[2](1) = ext.l(1) + hh;

    tail[0](0) = ext.l(0) + ts;
    tail[0](1) = ext.u(1);

    tail[1](0) = ext.l(0) + ts;
    tail[1](1) = ext.l(1) + hh;

    tail[2](0) = ext.u(0) - ts;
    tail[2](1) = ext.l(1) + hh;

    tail[3](0) = ext.u(0) - ts;
    tail[3](1) = ext.u(1);
}

void genDirTri(vec2<T> p, vec2<T> q, float br, vec2<T> tri[3])
{
    vec2<T> d;
    vec2<T>_diff(q, p, &d);		/* vector from \p p to \p q */
    vec2<T>_normalize(&d, 2);	/* normalized */

    /* generate left base point */
    vec2<T>_rot90(&d, &tri[0]);	/* 90 Degrees clockwise */
    vec2<T>_mulS(&tri[0], br);
    vec2<T>_addxy(&tri[0], p(0), p(1));

    /* destination point */
    tri[1] = *q;

    /* generate right base point */
    vec2<T>_rotm90(&d, &tri[2]);	/* 90 Degrees counter-clockwise */
    vec2<T>_mulS(&tri[2], br);
    vec2<T>_addxy(&tri[2], p(0), p(1));
}

void genDirQuad(vec2<T> p, vec2<T> q, float prad, float qrad,
                vec2<T> quad[4])
{
    vec2<T> d;
    vec2<T>_diff(q, p, &d);		/* vector from \p p to \p q */
    vec2<T>_normalize(&d, 2);	/* normalized */

    /* generate "left" p point */
    vec2<T>_rot90(&d, &quad[0]);	/* 90 Degrees clockwise */
    vec2<T>_mulS(&quad[0], prad);
    vec2<T>_addxy(&quad[0], p(0), p(1));

    /* generate "right" p point */
    vec2<T>_rotm90(&d, &quad[3]);	/* 90 Degrees counter-clockwise */
    vec2<T>_mulS(&quad[3], prad);
    vec2<T>_addxy(&quad[3], p(0), p(1));

    /* generate "left" q point */
    vec2<T>_rot90(&d, &quad[1]);	/* 90 Degrees clockwise */
    vec2<T>_mulS(&quad[1], qrad);
    vec2<T>_addxy(&quad[1], q(0), q(1));

    /* generate "right" q point */
    vec2<T>_rotm90(&d, &quad[2]);	/* 90 Degrees counter-clockwise */
    vec2<T>_mulS(&quad[2], qrad);
    vec2<T>_addxy(&quad[2], q(0), q(1));
}

void genDirArrow(vec2<T> p, vec2<T> q, float prad, float qrad,
                 vec2<T> hex[6])
{
    vec2<T> d;
    vec2<T>_diff(q, p, &d);		/* vector from \p p to \p q */
    vec2<T>_normalize(&d, 2);	/* normalized */

    /* generate "left" p point */
    vec2<T>_rot90(&d, &hex[0]);	/* 90 Degrees clockwise */
    vec2<T>_mulS(&hex[0], prad);
    vec2<T>_addxy(&hex[0], p(0), p(1));

    /* generate "mid" p point */
    hex[1] = *p;			/* centered */

    /* generate "right" p point */
    vec2<T>_rotm90(&d, &hex[2]);	/* 90 Degrees counter-clockwise */
    vec2<T>_mulS(&hex[2], prad);
    vec2<T>_addxy(&hex[2], p(0), p(1));

    /* generate "right" q point */
    vec2<T>_rotm90(&d, &hex[3]);	/* 90 Degrees counter-clockwise */
    vec2<T>_mulS(&hex[3], qrad);
    vec2<T>_addxy(&hex[3], q(0), q(1));

    /* generate "mid" p point */
    hex[4] = d;			/* 0 Degrees straigh ahead */
    vec2<T>_mulS(&hex[4], qrad);
    vec2<T>_addxy(&hex[4], q(0), q(1));

    /* generate "left" q point */
    vec2<T>_rot90(&d, &hex[5]);	/* 90 Degrees clockwise */
    vec2<T>_mulS(&hex[5], qrad);
    vec2<T>_addxy(&hex[5], q(0), q(1));
}

void genQuad(box2<T> box, vec2<T> quad[4])
{
    vec2<T>array_set4v(quad,
                       box.l(0), box.l(1),
                       box.u(0), box.l(1),
                       box.u(0), box.u(1),
                       box.l(0), box.u(1));
}

void genRotQuad(box2<T> box, ROTANG_t rotang, vec2<T> quad[4])
{
    vec2<T>array_set4v(quad,
                       box.l(0), box.l(1),
                       box.u(0), box.l(1),
                       box.u(0), box.u(1),
                       box.l(0), box.u(1));

    /* rotate quad */
    auto cen = box.cen();
    mat2f_sub_mvmul_add(&quad[0], &g_rottab[rotang], &cen);
    mat2f_sub_mvmul_add(&quad[1], &g_rottab[rotang], &cen);
    mat2f_sub_mvmul_add(&quad[2], &g_rottab[rotang], &cen);
    mat2f_sub_mvmul_add(&quad[3], &g_rottab[rotang], &cen);
}

void genRelT2T(box2<T> boxS, box2<T> boxD,
               vec2<T> triS[3], vec2<T> triD[3])
{
    auto cenS = boxS.cen();     // source center
    auto dimS = boxS.dim();     // source dimensions
    auto cenD = boxD.cen();     // destination center

    /* source tri */
    for (int i = 0; i < 3; i++) { triS[i] = cenS; }

    /* destination tri */
    vec2<T>_genDirTri(&cenS, &cenD, std::min(dimS(0), dimS(1)) * 0.25f, triD);
}

void genRelArrowH2H(box2<T> boxS, box2<T> boxD,
                    vec2<T> hexS[6], vec2<T> hexD[6])
{
    auto cenS = boxS.cen();     // source center
    /* source hex */
    for (int i = 0; i < 6; i++) { hexS[i] = cenS; }
    /* destination hex */
    vec2<T>_genDirArrow(cenS, boxD.cen(),
                        pnw::mean_element(boxS.dim()),
                        pnw::mean_element(boxD.dim()) * M_GOLDRAT,
                        hexD);
}
