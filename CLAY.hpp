/*!
 * \file CLAY.hpp
 * \brief Container Layout (CLay) - A Generalization of Layout Attributes
 *        to the containers:
 *        - N-d Arrays (Vector/Array/List, Matrix, Grid)
 *        - Text
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Container Layout Hints Fitting. */
typedef enum __attribute__ ((packed)) {
    CLAY_FIT_NONE,                /**< Fitting Ignored. */
        CLAY_FIT_WIDTH,               /**< Fit to width and clipped at height. */
        CLAY_FIT_HEIGHT,              /**< Fit to height and clipped at width. */
        CLAY_FIT_INSIDE               /**< Fit to both width and height (inside of extents), that is show all. */
        } CLAY_FIT_t;

/*! Container Layout Horizontal Alignment. */
typedef enum __attribute__ ((packed)) {
    CLAY_HALIGN_NONE,             /**< Horizontally Align Ignore */
        CLAY_HALIGN_LEFT,             /**< Horizontally Align Left */
        CLAY_HALIGN_RIGHT,            /**< Horizontally Align Right */
        CLAY_HALIGN_CENTER,           /**< Horizontally Align Centered */
        CLAY_HALIGN_FILL_SPACES,      /**< Horizontally Align Fill-Expand Spaces */
        } CLAY_HALIGN_t;

/*! Container Layout Vertical Alignment. */
typedef enum __attribute__ ((packed)) {
    CLAY_VALIGN_NONE,             /**< Horizontally Align Ignore */
        CLAY_VALIGN_TOP,              /**< Vertically Align Top */
        CLAY_VALIGN_MIDDLE,           /**< Vertically Align Middle */
        CLAY_VALIGN_BOTTOM,           /**< Vertically Align Bottom */
        CLAY_VALIGN_FILL,             /**< Vertical Align Fill-Expand */
        } CLAY_VALIGN_t;

/*! Container Layout Splitting. */
typedef enum __attribute__ ((packed)) {
    CLAY_SPLIT_NONE,              /**< Horizontally Align Ignore */
        CLAY_SPLIT_LINES,             /**< Split Lines at Word Boundaries */
        CLAY_SPLIT_WORDS,             /**< Split Words anywhere in word. */
        CLAY_SPLIT_INTERPUNCT,        /**< Split at Interpunctuation Symbols. */
        } CLAY_SPLIT_t;

/*!
 * Container Abbreviation Type Code.
 */
typedef enum __attribute__ ((packed)) {
    CLAY_ABBREV_NONE,                 /**< No abbrevation. \em Whole text scaled. Example: "Whole" */

        CLAY_ABBREV_BEG,                  /**< \em Beginning of first row. Example: "Beg..." */
        CLAY_ABBREV_BEG_END,              /**< \em Beginning and \em End of first row. Example: "Beg...End" */

        CLAY_ABBREV_FIRST,                /**< \em First Letter of first row. Example: "F" */
        CLAY_ABBREV_LAST,                 /**< \em Last Letter of first row. Example: "L" */
        } __attribute__ ((packed)) CLAY_ABBREV_t;

/*! Check if \p abbrev fits on a Single Row. */
inline bool CLAY_ABBREV_isSingleRow(CLAY_ABBREV_t abbrev)
{
    return (abbrev == CLAY_ABBREV_BEG ||
            abbrev == CLAY_ABBREV_BEG_END ||
            abbrev == CLAY_ABBREV_FIRST ||
            abbrev == CLAY_ABBREV_LAST);
}

/*! Container Layout Structure. */
class CLay
{
public:
    CLay(CLAY_FIT_t fit = CLAY_FIT_NONE,
         CLAY_HALIGN_t halign = CLAY_HALIGN_NONE,
         CLAY_VALIGN_t valign = CLAY_VALIGN_NONE,
         CLAY_SPLIT_t split = CLAY_SPLIT_NONE,
         CLAY_ABBREV_t abbrev = CLAY_ABBREV_NONE)
    {
        this->fit = fit;
        this->halign = halign;
        this->valign = valign;
        this->split = split;
        this->abbrev = abbrev;
    }

    CLay(CLAY_HALIGN_t halign,
         CLAY_VALIGN_t valign = CLAY_VALIGN_NONE,
         CLAY_SPLIT_t split = CLAY_SPLIT_NONE,
         CLAY_ABBREV_t abbrev = CLAY_ABBREV_NONE)
    {
        this->fit = CLAY_FIT_NONE;
        this->halign = halign;
        this->valign = valign;
        this->split = split;
        this->abbrev = abbrev;

    }

    CLay(CLAY_VALIGN_t valign,
         CLAY_SPLIT_t split = CLAY_SPLIT_NONE,
         CLAY_ABBREV_t abbrev = CLAY_ABBREV_NONE)
    {
        this->fit = CLAY_FIT_NONE;
        this->halign = CLAY_HALIGN_NONE;
        this->valign = valign;
        this->split = split;
        this->abbrev = abbrev;
    }

    CLay(CLAY_SPLIT_t split,
         CLAY_ABBREV_t abbrev = CLAY_ABBREV_NONE)
    {
        this->fit = CLAY_FIT_NONE;
        this->halign = CLAY_HALIGN_NONE;
        this->valign = CLAY_VALIGN_NONE;
        this->split = split;
        this->abbrev = abbrev;
    }

    CLay(CLAY_ABBREV_t abbrev)
    {
        this->fit = CLAY_FIT_NONE;
        this->halign = CLAY_HALIGN_NONE;
        this->valign = CLAY_VALIGN_NONE;
        this->split = CLAY_SPLIT_NONE;
        this->abbrev = abbrev;
    }

public:
    CLAY_FIT_t fit:2;             /**< Fitting. */
    CLAY_HALIGN_t halign:3;       /**< Horizontal Alignment. */
    CLAY_VALIGN_t valign:3;       /**< Vertical Alignment. */
    CLAY_SPLIT_t split:2;         /**< Splitting. */
    CLAY_ABBREV_t abbrev:3;       /**< Abbreviation. */
};

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Modify the clay \p a with the new clay \p b.
 * \return the modified clay \p a style.
 */
inline CLay CLAY_mod(CLay a, const CLay b)
{
    if (b.fit != CLAY_FIT_NONE) { a.fit = b.fit; }
    if (b.halign != CLAY_HALIGN_NONE) { a.halign = b.halign; }
    if (b.valign != CLAY_VALIGN_NONE) { a.valign = b.valign; }
    if (b.split != CLAY_VALIGN_NONE) { a.split = b.split; }
    return a;
}

inline int CLAY_eq(CLay a, CLay b)
{
    return (a.fit == b.fit &&
            a.halign == b.halign &&
            a.valign == b.valign &&
            a.split == b.split);
}

1inline int CLAY_neq(CLay a, CLay b)
{
    return (a.fit != b.fit ||
            a.halign != b.halign ||
            a.valign != b.valign ||
            a.split != b.split);
}

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
