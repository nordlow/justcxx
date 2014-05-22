/*! \file pop.hpp
 * \brief Pop Out or In Rounded Boxes from Boxes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-07-04 19:05
 */

#pragma once

#include "box.hpp"
#include "rbox.hpp"

/*! Pop-Out \p a Single-Side-Rounded box \p b from the box \p a from the
 * edge of \p a given by \p boxedge.
 * \param[in] boxA
 * \param[out] boxB
 * \param[in] prg Pop-Out Progress.
 * \param[in] rposE Relative Position Along Edge. Use 0.5f for \em centered pop-out.
 */
template<class T>
int
box2f_rlay_popOut_rbox2f(box2f boxA, rbox2f& rboxB,
                         vec2f rdim, vec2f rradI,
                         float prg, float rposE,
                         BOXEDGE_t boxedge)
{
    int ret = 0;

    vec2f dimA; box2f_rdDim(boxA, &dimA);
    vec2f dimB; vec2f_set(&dimB, dimA(0) * rdim(0), dimA(1) * rdim(1));

    box2f boxC;
    CORNMASK_t cornmask = CORNMASK_FULL;
    switch (boxedge) {
    case BOXEDGE_TOP: {
        vec2f cenB;			/* Bottom Center of rboxB */
        vec2f_set(&cenB, boxA.l(0) + dimA(0) * rposE, boxA.l(1));
        boxC = box_bcen_dim(cenB, dimB); /* Bottom Center& Dim => Ext */
        cornmask = CORNMASK_TOP;
        break;
    }
    case BOXEDGE_BOT: {
        vec2f cenT;			/* Top Center of rboxB */
        vec2f_set(&cenT, boxA.l(0) + dimA(0) * rposE, boxA.u(1));
        boxC = box_tcen_dim(cenT, dimB); /* Top Center& Dim => Ext */
        cornmask = CORNMASK_BOTTOM;
        break;
    }
    case BOXEDGE_LEFT: {
        vec2f cenR;			/* Right Center of rboxB */
        vec2f_set(&cenR, boxA.l(0), boxA.u(1) - dimA(1) * rposE);
        boxC = box_rcen_dim(cenR, dimB); /* Right Center& Dim => Ext */
        cornmask = CORNMASK_LEFT;
        break;
    }
    case BOXEDGE_RIGHT: {
        vec2f cenL;			/* Left Center of rboxB */
        vec2f_set(&cenL, boxA.u(0), boxA.u(1) - dimA(1) * rposE);
        boxC = box_lcen_dim(cenL, dimB); /* Left Center& Dim => Ext */
        cornmask = CORNMASK_RIGHT;
        break;
    }
    default:
        PWARN("Cannot handle boxedge %d. Defaulting boxC to boxA\n", boxedge);
        boxC = *boxA;
        cornmask = CORNMASK_FULL;
        break;
    }

    rbox2f_set_OBox_IRRad(rboxB, &boxC, rradI, cornmask, CORNSTYLE_ELL_MIN);

    return ret;
}

/*! Pop-Out \p a Single-Side-Rounded box \p b from the box \p a from the
 * edge of \p a given by \p boxedge.
 * \param[in] boxA
 * \param[out] boxB
 * \param[in] prg Pop-Out Progress.
 * \param[in] rposE Relative Position Along Edge. Use 0.5f for \em centered pop-out.
 */
template<class T>
int
box2f_rlay_popOut_box2f(box2f boxA, box2f& boxB,
                        vec2f rdim,
                        float prg, float rposE,
                        BOXEDGE_t boxedge)
{
    int ret = 0;

    vec2f dimA; box2f_rdDim(boxA, &dimA);
    vec2f dimB; vec2f_set(&dimB, dimA(0) * rdim(0), dimA(1) * rdim(1));

    switch (boxedge) {
    case BOXEDGE_TOP: {
        vec2f cenB;			/* Bottom Center of rboxB */
        vec2f_set(&cenB, boxA.l(0) + dimA(0) * rposE, boxA.l(1));
        boxB = box_bcen_dim(cenB, dimB); /* Bottom Center& Dim => Ext */
        break;
    }
    case BOXEDGE_BOT: {
        vec2f cenT;			/* Top Center of rboxB */
        vec2f_set(&cenT, boxA.l(0) + dimA(0) * rposE, boxA.u(1));
        boxB = box_tcen_dim(cenT, dimB); /* Top Center& Dim => Ext */
        break;
    }
    case BOXEDGE_LEFT: {
        vec2f cenR;			/* Right Center of rboxB */
        vec2f_set(&cenR, boxA.l(0), boxA.u(1) - dimA(1) * rposE);
        boxB = box_rcen_dim(cenR, dimB); /* Right Center& Dim => Ext */
        break;
    }
    case BOXEDGE_RIGHT: {
        vec2f cenL;			/* Left Center of rboxB */
        vec2f_set(&cenL, boxA.u(0), boxA.u(1) - dimA(1) * rposE);
        boxB = box_lcen_dim(cenL, dimB); /* Left Center& Dim => Ext */
        break;
    }
    default:
        PWARN("Cannot handle boxedge %d. Defaulting boxB to boxA\n", boxedge);
        *boxB = *boxA;
        break;
    }

    return ret;
}

/*! Pop-In \p a Single-Side-Rounded box \p b from the box \p a from the
 * edge of \p a given by \p boxedge.
 * \param[in] prg Pop-In Progress.
 * \param[in] rposE Relative Position Along Edge. Use 0.5f for \em centered pop-in.
 */
template<class T>
int
box2f_rlay_popIn_rbox2f(box2f boxA, rbox2f& rboxB,
                        vec2f rdim,
                        float prg, float rposE,
                        BOXEDGE_t boxedge);
