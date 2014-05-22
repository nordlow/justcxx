/*! \file gen_shapes.hpp
 * \brief Generate Popular Shapes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "utils.h"
#include "box.hpp"
#include "rottab.hpp"

/*! Layout The \em Letter \em X inside \p ext.
 * \param rxd relative x-dimension specifying thickness of letter.
 */
template<class T> void genLetterX(box2<T> ext, float rxd, vec2<T> wpolyX[8]);

/*! Layout an \em Arrow inside \p ext.
 * \param rxd relative x-dimension specifying thickness of letter.
 */
template<class T> void genArrow(box2<T> ext, vec2<T> head[3], vec2<T> tail[4]);

/*! Generate \em Directed \em Triangle from \p p to \p q with base radius of br put
 *  resulting 3-edged triangle in *tri. */
template<class T> void genDirTri(vec2<T> p, vec2<T> q, float br, vec2<T> tri[3]);

/*! Generate \em Directed \em Quad from \p p to \p q with source base radius of prad and
 *  with destination base radius of qrad. */
template<class T> void genDirQuad(vec2<T> p, vec2<T> q, float prad, float qrad,
                                   vec2<T> quad[4]);

/*! Generate \en 4-Edged \em Quad from \p box. */
template<class T> void genQuad(box2<T> box, vec2<T> quad[4]);

/*! Generate \em Rotated \em 4-Edged \em quad from \p box. */
template<class T> void genRotQuad(box2<T> box, ROTANG_t rotang, vec2<T> quad[4]);

/*! Generate \em Relation \em Triangle-to-Triangle \em Animation. */
template<class T> void genRelT2T(box2<T> boxS, box2<T> boxD,
                                  vec2<T> triS[3], vec2<T> triD[3]);

/*! Generate \em Relation \em Arrow \em Hexagon-to-Hexagon \em Animation. */
template<class T> void genRelArrowH2H(box2<T> boxS, box2<T> boxD,
                                       vec2<T> hexS[6], vec2<T> hexD[6]);
