/*!
 * \file VATT_enum.hpp
 * \brief Object Attribute Types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "../utils.h"

/*!
 * Object Attribute Type Code.
 *
 * Currently only used to select \c VATT-specific relating objects in
 * some of the functions ob_net_op.*().
 */
typedef enum {
    VATT_BIT_any_,

    VATT_BIT_CURSOR,		///< Object is acting as a cursor.
    VATT_BIT_IND,		///< Object is indicated.
    VATT_BIT_SEL,		///< Object is selected.
    VATT_BIT_DEL,               ///< Object is <b>being</b> deleted.
    VATT_BIT_TAG,		///< Object is tagged.
    VATT_BIT_VIS,		///< Visible.
    VATT_BIT_DAM,		///< Damaged.
    VATT_BIT_STK,		///< Sticky.
    VATT_BIT_ALT,               ///< State of object shape has be altered.
    VATT_BIT_DYN,		///< Dynamic (Content).
    VATT_BIT_UIA,		///< User InterAction (UIA).
    VATT_BIT_SHIT,		///< Search Hit
    VATT_BIT_FIXED_SHAPE,	///< Respect Shape Type
    VATT_BIT_VIEWLIM,		///< Limited by current View

    VATT_BIT_NOT_CURSOR,
    VATT_BIT_NOT_IND,
    VATT_BIT_NOT_SEL,
    VATT_BIT_NOT_DEL,
    VATT_BIT_NOT_TAG,
    VATT_BIT_NOT_VIS,
    VATT_BIT_NOT_DAM,
    VATT_BIT_NOT_STK,
    VATT_BIT_NOT_ALT,
    VATT_BIT_NOT_DYN,
    VATT_BIT_NOT_UIA,
    VATT_BIT_NOT_SHIT,
    VATT_BIT_NOT_FIXED_SHAPE,
    VATT_BIT_NOT_VIEWLIM,

    VATT_BIT_num,
} VATT_BIT_t;

typedef enum {
    VATT_UINT8_any_,

    VATT_UINT8_INDPRG,		///< Indication Progress Counter.
    VATT_UINT8_SELPRG,		///< Selection Progress Counter.
    VATT_UINT8_RELPRG,		///< Relation Symbols Progress Counter.
    VATT_UINT8_RSZPRG,		///< Resize Symbols Progress Counter.
    VATT_UINT8_TRANSPRG,	///< Fade-In Progress Counter.
    VATT_UINT8_POPOUTPRG,	///< PopUp Progress Counter.
    VATT_UINT8_BEATPRG,		///< (Heart) Beat Progress Counter.

    VATT_UINT8_WOBBLE,		///< Wobble Progress Counter.
    VATT_UINT8_SLAY,		///< Automatically Box-Layouted Object.
    VATT_UINT8_ROTANG,		///< Rotation Angle. \see ROTTAB_SZ.

    VATT_UINT8_CTRLP_RESHAPE,   ///< Shape Control Point Selection Points (Bits) for RESIZE.
    VATT_UINT8_CTRLP_RECOLOR,   ///< Shape Control Point Selection Points (Bits) for RECOLOR.
    VATT_UINT8_SFRAME_RELDIM,   ///< Shape Frame Relative Dimension (Thickness).

    VATT_UINT8_num,
} __attribute__ ((packed)) VATT_UINT8_t;

typedef enum {
    VATT_UINT16_any_,

    VATT_UINT16_num,
} __attribute__ ((packed)) VATT_UINT16_t;

typedef enum {
    VATT_UINT32_any_,

    // \todo Move to \c vTheme having \c CHILD objects.
    VATT_UINT32_FG_COLOR,           ///< Foreground Color.
    VATT_UINT32_BG_COLOR,           ///< BackGround Color.
    VATT_UINT32_SFRAME_COLOR,       ///< Shape Frame Color.
    VATT_UINT32_SFRAME_OUTER_COLOR, ///< Shape Frame Outer Color

    VATT_UINT32_SELORD,         ///< Selection Order.

    VATT_UINT32_num,
} __attribute__ ((packed)) VATT_UINT32_t;
