/*! \file OP_enum.hpp
 * \brief Operation Types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "../utils.h"

/*! Operation Type Code.
 *
 * Effect of Operation depends on which Interaction Mode (\c IAMODE_t)
 * we are currently in.
 *
 * \see \c vapp_update_unary() for explanations of use.
 *
 * \note If you add constants here you must also add them
 * correspondingly to \c OP_getName().
 */
typedef enum {
    OP_CREATE,                  ///< Create Content

    OP_NEW,                     ///< New

    OP_CLONE,                   ///< Clone
    OP_CLONE_RELS,              ///< Clone Relations

    OP_DELETE,                  ///< Delete (forward)
    OP_REVIVE,                  ///< Revive object (if it is currently being deleted)
    OP_BACKSPACE,               ///< Delete (backward)

    OP_COPY_TO,                 ///< Copy To.
    OP_MOVE_TO,                 ///< Move To.

    OP_COPY_AND_OVERWRITE_TO,   ///< Copy and Overwrite To.
    OP_MOVE_AND_OVERWRITE_TO,   ///< Move and OVerwrite To.

    OP_RENAME,                  ///< Rename object.
    OP_REPLACE,                 ///< Replace object with object

    OP_IND,                     ///< Indicate Object
    OP_UNIND,                   ///< Unindicate Object

    OP_SOLESEL,                 ///< Unselect all other and then select
    OP_SEL,                     ///< Select object
    OP_UNSEL,                   ///< Unselect object
    OP_TOGGLESEL,               ///< Toggle selection of object
    OP_RANGESEL,                ///< Range Select

    OP_MARK,                    ///< Mark Object
    OP_UNMARK,                  ///< Unmark Object

    OP_MERGE_SELS,              ///< Merge Previous with Current Selection into Current Selection

    OP_RELNAV,                  ///< Navigate to a relating object

    OP_SEL_CHILDs,              ///< Select all object children
    OP_SEL_LETTERs,             ///< Select all \c REL_LETTERs

    OP_STICK,                   ///< Stick object (to window)
    OP_UNSTICK,                 ///< Unstick object (from window)

    OP_MAP,                     ///< Map object (to world view). See Ob::map
    OP_UNMAP,                   ///< Unmap object (from world view). See Ob::map

    OP_UIA,                     ///< Make object User-InterActable (UIA) (Non-Greyed-Out)
    OP_UNUIA,                   ///< Make object non-User-InterActable (UIA) (Greyed-Out)

    OP_LOCK,                    ///< Lock Object (Unmodifiable)
    OP_UNLOCK,                  ///< UnLock Object (Modifiable)

    OP_SHIT,                    ///< Search Hit object
    OP_UNSHIT,                  ///< Search UnHit object

    OP_IOSYNC,                  ///< General function to keep object up-to-date with its external resources */

    OP_CONVERT,                 ///< Convert object from one type to another

    OP_CONVERT_TO_JPEG,         ///< Convert to JPEG.

    OP_SHOW,                    ///< Show
    OP_HIDE,                    ///< Hide

    OP_SHOW_RELS,               ///< Generate object relation symbols
    OP_HIDE_RELS,               ///< Delete object relation symbols

    OP_SHOW_GRID,               ///< Show Grid
    OP_HIDE_GRID,               ///< Hide Grid

    OP_SHOW_AXTICKS,            ///< Show AxTicks
    OP_HIDE_AXTICKS,            ///< Hide AxTicks

    OP_SHOW_LAYHINTS,           ///< Show Layhints
    OP_HIDE_LAYHINTS,           ///< Hide Layhints

    OP_LINE_GRID,               ///< Show Grid as lines
    OP_CHECKER_GRID,            ///< Show grid as a checkerboard

    OP_LAY_LETTERs,             ///< Layout \c REL_LETTERs

    OP_SLAY_RING,               ///< Ring Layout
    OP_SLAY_SUNBURST,           ///< Circular Ring-Segment Layout
    OP_SLAY_HGRID,              ///< Horizontal Grid Layout
    OP_SLAY_VGRID,              ///< Vertical Grid Layout
    OP_SLAY_RGRID,              ///< Square Grid Layout
    OP_SLAY_HTREE,              ///< Horizontal Tree Layout
    OP_SLAY_VTREE,              ///< Vertical Tree Layout
    OP_SLAY_FREE,               ///< Set Free Layout

    OP_MD5SUM,                  ///< Calculate MD5-checksum.

    OP_COMPLETE,                ///< General Completion Operation

    OP_RAISE,                   ///< Put in upmost layer
    OP_LOWER,                   ///< Put in lowermost layer

    OP_VLINEUP,                 ///< Vertically Line Up
    OP_HLINEUP,                 ///< Horizontally Line Up
    OP_RGRIDUP,                 ///< Rectangular Grid Up

    OP_SHUFFLE_CENTERS,         ///< Shuffle Centers
    OP_CYCLE_CENTERS_LEFT,      ///< Cycle Centers Left
    OP_CYCLE_CENTERS_RIGHT,     ///< Cycle Centers Right
    OP_SHUFFLE_EXTENTS,         ///< Shuffle Extents
    OP_AVERAGE_EXTENTS,         ///< Average Extents

    OP_SUBSCRIPT,               ///< Subscript
    OP_SUPERSCRIPT,             ///< Superscript

    OP_HALIGN_LEFT,             ///< Horizontally Align to Left Edge
    OP_HALIGN_CENTER,           ///< Horizontally Align to Center
    OP_HALIGN_RIGHT,            ///< Horizontally Align to Right Edge

    OP_VALIGN_TOP,              ///< Vertically Align to Top
    OP_VALIGN_MIDDLE,           ///< Vertically Align to Middle
    OP_VALIGN_BOTTOM,           ///< Vertically Align to Bottom

    OP_SPLIT_LINES,             ///< Split lines.
    OP_SPLIT_WORDS,             ///< Split words.

    OP_CADJ_GROW,               ///< Grow after relating objects.
    OP_CADJ_SHRINK,             ///< Shrink after relating objects.

    OP_PRINT,                   ///< Print object information to \c stdout.
    OP_PRINT_MET,               ///< Print object metrics to \c stdout.
    OP_PRINT_SHAPE,             ///< Print object shape to \c stdout.

    OP_PLOAD,                   ///< Progressive Load (from disk to memory or from memory to GPU).

    OP_SYNC,                    ///< Synchronize

    OP_ROT90_C,                 ///< Rotate 90 Degrees Clockwise
    OP_ROT90_CC,                ///< Rotate 90 Degrees Counter-Clockwise

    OP_VARWIDTH,                ///< Make Variable Width
    OP_FIXWIDTH,                ///< Make Fixed Width
    OP_SANSSERIF,               ///< Make Sans Seriffed
    OP_SERIF,                   ///< Make Seriffed

    OP_REGULAR,                 ///< Make Regular (not Bold)
    OP_BOLD,                    ///< Make Bold (not Roman)
    OP_STRAIGHT,                ///< Make Straight (not Italic)
    OP_ITALIC,                  ///< Make Italic (not Upright)
    OP_NO_UNDERLINE,            ///< Make Not Underlined
    OP_UNDERLINE,               ///< Make Underlined

    OP_COMPARE,                 ///< Compare
    OP_EQUAL,                   ///< Check for Equality

    OP_SET,                     ///< Set
    OP_CLEAR,                   ///< Clear
    OP_TOGGLE,                  ///< Toggle
    OP_NEGATE,                  ///< Negate

    OP_TRANSPOSE,               ///< Transpose
    OP_REVERSE,                 ///< Reverse

    OP_INSERT,                  ///< Insert

    OP_FADEIN,                  ///< Fade-In (transparency)
    OP_FADEOUT,                 ///< Fade-Out (transparency)

    OP_FOCUS_VIEW,              ///< Focus View on Object
    OP_CENTER_VIEW,             ///< Center View on Object

    /** Focus all non-sticky (user-defined) objects contained in registry */
    OP_FOCUS_VIEW_WORLD,

    OP_FOCUS_OBJECT,            ///< Focus Object to View.
    OP_CENTER_OBJECT,           ///< Center Object to View.

    /*!
     * Operations affecting the shape <em>form/<em> type.
     */
    /* @{ */
    OP_SFORM_BOX,               /** Turn shape into a rectangle */
    OP_SFORM_ELL,               /** Turn shape into an ellipse */
    OP_SFORM_RBOX_ell,          /** Turn shape into a Elliptically-Rounded Rectangle */
    OP_SFORM_RBOX_cir,          /** Turn shape into a Circularly-Rounded Rectangle */
    OP_SFORM_RBOX_lin,          /** Turn shape into a Line-Rounded Rectangle */
    OP_SFORM_SQR,               /** Turn shape into a Square */
    OP_SFORM_CIR,               /** Turn shape into a Circle */
    OP_SFORM_CRING,             /** Turn shape into a CircleRing */
    OP_SFORM_CRINGRSEG,         /** Turn shape into a Ring-Segment */
    OP_SFORM_SPHERE,            /** Turn shape into a Sphere */
    OP_SFORM_TRI,               /** Turn shape into a Regular Triangle */
    OP_SFORM_ROMB,              /** Turn shape into a Regular Romb */
    OP_SFORM_PENT,              /** Turn shape into a Regular Pentagon */
    OP_SFORM_HEX,               /** Turn shape into a Regular Hexagon */
    OP_SFORM_HEPT,              /** Turn shape into a Regular Heptagon */
    OP_SFORM_OCT,               /** Turn shape into a Regular Octagon */
    OP_SFORM_POLY,              /** Turn shape into a Regular Polygon */
    OP_SFORM_COLORWHEEL,        /** Turn shape into a Color Wheel */
    OP_SFORM_GOPWHEEL,          /** Turn shape into a GOP Wheel */
    OP_SFORM_SINEFLOWER,        /** Turn shape into a SineFlower */
    /* @} */

    /*!
     * Operations affecting the shape <em>fill</em> type.
     */
    /* @{ */
    OP_SFILL_SINGLE,
    OP_SFILL_GRADH,
    OP_SFILL_GRADV,
    OP_SFILL_GRADR,
    OP_SFILL_GRADA,
    /* @} */

    /*!
     * Operations affecting the shape <em>frame</em> type.
     */
    /* @{ */
    OP_SFRAME_NONE,
    OP_SFRAME_INNER_BOXED,
    OP_SFRAME_OUTER_BOXED,
    /* @} */

    /*!
     * Operations affecting the general state of the DVis.
     */
    /* @{ */
    OP_TOGGLE_FULLSCREEN,
    OP_TOGGLE_WIDESCREEN,
    OP_SFX_ON,
    OP_SFX_OFF,
    OP_OBS_ON,
    OP_OBS_OFF,
    OP_DRAW_ON,
    OP_DRAW_OFF,
    OP_KERNING_ON,
    OP_KERNING_OFF,
    OP_MULTISAMPLE_ON,
    OP_MULTISAMPLE_OFF,
    /* @} */

    /*!
     * Non-Selection Based Operations.
     */
    /* @{ */
    OP_NEW_TESTSET,
    /* @} */

    OP_SORT_CHILDs_TYPE,
    OP_SORT_CHILDs_SIZE,

    OP_BEAT_ON,

    /*!
     * Currently applied on VTimers.
     */
    /* @{ */
    OP_START,
    OP_STOP,
    OP_RESTART,
    /* @} */

    OP_RANDOMIZE,               ///< Randomize.

    OP_COUNT,                   ///< Count Objects.

    OP_ADD,                     ///< Add.
    OP_SUB,                     ///< Subtract.
    OP_MUL,                     ///< Multiply.
    OP_DIV,                     ///< Divide.

    OP_MIN,                     ///< Minimum.
    OP_MAX,                     ///< Maximum.

    OP_SUM,                     ///< Sum.
    OP_PROD,                    ///< Product.

    OP_STATBAR_SHOW,            ///< Show statbar
    OP_STATBAR_HIDE,            ///< Hide statbar
    OP_STATBAR_TOGGLE,          ///< Toggle statbar

    OP_PLOT,                    ///< Plot

    OP_EXECUTE,                 ///< Execute.

    OP_MIPMAP_ON,               ///< MimMap On.
    OP_MIPMAP_OFF,              ///< MipMap Off.

    OP_IAMODE_CYCLIC_ON,
    OP_IAMODE_CYCLIC_OFF,

    OP_MORPH_ON,
    OP_MORPH_OFF,

    OP_STRETCH_ON,
    OP_STRETCH_OFF,

    OP_SHOW_OBBX,
    OP_HIDE_OBBX,

    OP_SAVE_WORLD,
    OP_LOAD_WORLD,

    OP_PRINT_KCHORDS,
    OP_PRINT_DSTATS,
    OP_PRINT_MBOX,
    OP_PRINT_VREG,

    OP_VCS_STATUS,
    OP_VCS_COMMIT,
    OP_VCS_UPDATE,

    OP_QUIT,

    OP_NONE,
    OP_NUM,                     ///< Number of different operations.

} __attribute__ ((packed)) OP_t;

/*!
 * Perform Dualism-Check of \p vop0 and \p vop1.
 * \return 1 if \p vrel0 and \p vrel1 are opposite operations, 0 otherwise.
 */
int
P_duals(OP_t vop0, OP_t vop1);

/*!
 * Perform Dualism-Check of \p vop0 and \p vop1 as the dual \p a and \p b.
 * \return 1 if \p vrel0 and \p vrel1 are opposite operations, 0 otherwise.
 */
int
OP_dualsAs(OP_t vop0, OP_t vop1,
	    OP_t a, OP_t b);

/*!
 * Return non-zero if \p op belongs to the group of Box LAYouters.
 */
inline int
OP_is_SLAY(OP_t vop)
{
    return (vop == OP_SLAY_RING or
            vop == OP_SLAY_SUNBURST or
            vop == OP_SLAY_HGRID or
            vop == OP_SLAY_VGRID or
            vop == OP_SLAY_RGRID or
            vop == OP_SLAY_HTREE or
            vop == OP_SLAY_VTREE or
            vop == OP_SLAY_FREE);
}

/*!
 * Return non-zero if \p op belongs to the group of \c SFORMs.
 */
inline int
OP_is_SFORM(OP_t vop)
{
    return (vop == OP_SFORM_BOX or
            vop == OP_SFORM_ELL or
            vop == OP_SFORM_RBOX_ell or
            vop == OP_SFORM_RBOX_cir or
            vop == OP_SFORM_RBOX_lin or
            vop == OP_SFORM_SQR or
            vop == OP_SFORM_CIR or
            vop == OP_SFORM_CRING or
            vop == OP_SFORM_CRINGRSEG or
            vop == OP_SFORM_SPHERE or
            vop == OP_SFORM_TRI or
            vop == OP_SFORM_ROMB or
            vop == OP_SFORM_PENT or
            vop == OP_SFORM_HEX or
            vop == OP_SFORM_HEPT or
            vop == OP_SFORM_OCT or
            vop == OP_SFORM_POLY or
            vop == OP_SFORM_COLORWHEEL or
            vop == OP_SFORM_GOPWHEEL or
            vop == OP_SFORM_SINEFLOWER);
}

/*!
 * Return non-zero if \p op belongs to the \c FOCUS-group.
 */
inline int
OP_is_FOCUS(OP_t vop)
{
    return (vop == OP_FOCUS_VIEW or
            vop == OP_CENTER_VIEW or
            vop == OP_FOCUS_VIEW_WORLD or
            vop == OP_FOCUS_OBJECT or
            vop == OP_CENTER_OBJECT);
}

/*!
 * Return non-zero if \p op belongs to the \c DELETE-group.
 */
inline int
OP_is_DELETE(OP_t vop)
{
    return (vop == OP_DELETE or
            vop == OP_BACKSPACE);
}

const char *
OP_getName(OP_t vop);

/* ========================================================================= */
