/*!
 * \file REL_enum.hpp
 * \brief Object \em Relation \em Type Codes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "../utils.h"

/* ========================================================================= */

/*!
 * Object \em Relation Type Code.
 *
 * These describe the different kinds of one-way relations an Pattern
 * can contain together with a corresponding destination.
 *
 * Many relation types come in pairs such as: \c REL_PARENT - \c REL_CHILD.
 *
 * Such pairs are used to build links (\em two-way relations) between
 * two different patterns.
 *
 * \NOTE IMPORTANT: Relation types naming is hierarchical indicating that any
 * type name having childing types can be interpreted as a
 * <b>group</b> in operations matching a specified relation
 * <b>BELONGING</b> to this particular group.
 *
 * EXAMPLE: \c REL_LETTER can be treated as the group containing sub-relations
 * \c REL_LETTER_BOL, \c REL_LETTER_EOL, etc.
 */
typedef enum {
    REL_any_,			///< Meta-type to indicate \em ANY type relation or empty slot.

    REL_PARENT,                 ///< \em Parenting (superobject). Can be a \c vFile's parenting \c vDir.
    REL_CHILD,                  ///< \em Child (subobject). Can be a \c vDir's child \c vFile.

    REL_SUPER = REL_PARENT,     ///< \em Super.
    REL_SUB = REL_CHILD,        ///< \em Sub.

    REL_SIBLING,		///< Sibling, currently \c CHILD having the same \c PARENT.
    REL_SIBLING_PREV,		///< \em Next Sibling
    REL_PREV = REL_SIBLING_PREV, ///< \em Next
    REL_SIBLING_NEXT,		///< \em Previous Sibling
    REL_NEXT = REL_SIBLING_NEXT, ///< \em Sibling

    REL_SOURCE,                 ///< \em Source (of Link)
    REL_TARGET,                 ///< \em Target (of Link)

    REL_GROUP,                  ///< \em Group.
    REL_MEMBER,                 ///< \em Member.

    REL_CALLER,                 ///< Function \em Caller
    REL_CALLEE,                 ///< Function \em Callee

    REL_GENERALIZER,            ///< \em Generalizer
    REL_SPECIALIZER,            ///< \em Specializer

    REL_MASTER,			///< Controlling Master.
    REL_MASTER_TALKBUBBLE,      ///< Controlled Master of TalkBubble.
    REL_MASTER_PLOT,		///< Controlling Master Plot.
    REL_MASTER_PLOT_VT,		///< Controlling Master Plot Vertical Top.
    REL_MASTER_PLOT_VB,         ///< Controlling Master Plot Vertical Bottom.
    REL_MASTER_PLOT_HL,		///< Controlling Master Plot Horizontal Left.
    REL_MASTER_PLOT_HR,		///< Controlling Master Plot Horizontal Right.

    REL_SLAVE,			///< Slave.
    REL_SLAVE_TITLENAME,	///< Slave Name.
    REL_SLAVE_TYPENAME,		///< Slave Magic Text.
    REL_SLAVE_DOC,		///< Slave Documentation.
    REL_SLAVE_KCOMB,		///< Slave KeyBoard Combination.
    REL_SLAVE_ICON,		///< Slave Icon.
    REL_SLAVE_ATTR,		///< Slave Attribute (for Files).
    REL_SLAVE_TALKBUBBLE,	///< Slave TalkBubble.
    REL_SLAVE_PROGRESS,		///< Slave Progress Object.

    REL_SLAVE_AXTICK,		///< Slave AxTick Object.
    REL_SLAVE_AXTICK_VT,	///< Slave AxTick X Object.
    REL_SLAVE_AXTICK_VB,	///< Slave AxTick X Object.
    REL_SLAVE_AXTICK_HL,	///< Slave AxTick Y Object.
    REL_SLAVE_AXTICK_HR,	///< Slave AxTick Y Object.

    REL_SLAVE_LAYHINT,          ///< Layout Hint

    REL_SLAVE_LAYHINT_CADJ,
    REL_SLAVE_LAYHINT_CADJ_LEFT_GROW,
    REL_SLAVE_LAYHINT_CADJ_RIGHT_GROW,
    REL_SLAVE_LAYHINT_CADJ_TOP_GROW,
    REL_SLAVE_LAYHINT_CADJ_BOT_GROW,
    REL_SLAVE_LAYHINT_CADJ_LEFT_SHRINK,
    REL_SLAVE_LAYHINT_CADJ_RIGHT_SHRINK,
    REL_SLAVE_LAYHINT_CADJ_TOP_SHRINK,
    REL_SLAVE_LAYHINT_CADJ_BOT_SHRINK,

    REL_OPPOSITE_BWD,           ///< Opposite Backward
    REL_OPPOSITE_FWD,           ///< Opposite Forward

    REL_SLAVE_SELSYM,		///< Selection Triangle.
    REL_SLAVE_SELSYM_UL,	///< Selection Triangle Upper-Left.
    REL_SLAVE_SELSYM_UR,	///< Selection Triangle Upper-Right.
    REL_SLAVE_SELSYM_LL,	///< Selection Triangle Lower-Left.
    REL_SLAVE_SELSYM_LR,	///< Selection Triangle Lower-Right.

    REL_MODESYM_OWNEE,		///< Mode symbol Ownee.
    REL_MODESYM_OWNER,		///< Mode Symbol Owner.

    REL_SLAVE_RELSYM_FWD,       ///< Slave Relation Symbol Forward.
    REL_SLAVE_RELSYM_BWD,       ///< Slave Relation Symbol Backward.
    REL_MASTER_RELSYM_OWNER,	///< Master Relation Symbol Owner.
    REL_MASTER_RELSYM_TARGET,	///< Master Relation Symbol Target.

    REL_LIMITER_LOWER,		///< Lower Limiter.
    REL_LIMITEE_LOWER,		///< Lower Limitee.
    REL_LIMITER_UPPER,		///< Upper Limiter.
    REL_LIMITEE_UPPER,		///< Upper Limitee.

    /** \name Producer-Consumer */
    /* @{ */
    REL_PRODUCER,		///< Indata (at consumer).
    REL_CONSUMER,               ///< Consumer. Can be the texture/plot of a file-image.
    REL_PRODUCER_BINCALL,	///< ... Binary File Call Output.
    REL_CONSUMER_BINCALL,	///< ... Binary File Call Output.
    REL_PRODUCER_TALKBUBBLE,	///< ... Frustration Message.
    REL_CONSUMER_TALKBUBBLE,	///< ... Frustration Message.
    /* @} */

    REL_ORIGINAL,  ///< Relation from cloned object to its original.
    REL_COPY,	   ///< Relation from original object to its clone.

    /** \name (Geometrical) Alignment */
    /* @{ */
    REL_ALIGNER,		///< Aligner.
    REL_ALIGNEE,		///< Alignee.

    REL_ALIGNEE_V,		///< Alignee Vertical.
    REL_ALIGNEE_H,		///< Alignee Horizontal.

    REL_ALIGNEE_VT,		///< Alignee Vertical Top.
    REL_ALIGNEE_VB,		///< Alignee Vertical Bottom.
    REL_ALIGNEE_HL,		///< Alignee Horizontal Left.
    REL_ALIGNEE_HR,		///< Alignee Horizontal Right.

    REL_ALIGNER_VSCRIPT_SUB,	///< Aligner Subscript.
    REL_ALIGNEE_VSCRIPT_SUB,	///< Alignee Subscript.
    REL_ALIGNER_VSCRIPT_SUPER,	///< Aligner Superscript.
    REL_ALIGNEE_VSCRIPT_SUPER,	///< Alignee Superscript.
    /* @} */

    /** \name Dimension Fitting */
    /* @{ */
    REL_VFITTER,		///< Vertical Fitter.
    REL_VFITTEE,		///< Vertical Fittee.
    REL_HFITTER,		///< Horizontal Fitter.
    REL_HFITTEE,		///< Horizontal Fittee.
    /* @} */

    /** \name Integer (OB_VNUMZ) Layouting */
    /* @{ */
    REL_DIGIT,			///< Digit part of a number.
    /* @} */

    /** \name Text Layouting */
    /* @{ */
    REL_LETTER,			///< Letter anywhere on the line.

    REL_LETTER_DIGIT,		///< Numerical digit (0-9).

    REL_LETTER_WS,		///< White Space.
    REL_LETTER_WS_SPACE,	///< Space.
    REL_LETTER_WS_HTAB,		///< Horizontal Tab.
    REL_LETTER_WS_VTAB,		///< Vertical Tab.

    REL_LETTER_BOL,		///< Letter at Beginning Of Line.
    REL_LETTER_EOL,		///< Letter at End Of Line.
    REL_LETTER_EOL_MAC,		///< Letter at End Of Line (Mac-Style).
    REL_LETTER_EOL_UNIX,	///< Letter at End Of Line (Unix-Style).
    REL_LETTER_EOL_WIN,		///< Letter at End Of Line (Windows-Style).
    REL_LETTER_BOT,		///< Letter at Beginning Of Text.
    REL_LETTER_EOT,		///< Letter at End Of Text.

    REL_LETTER_BOI_C,           ///< Letter at Beginning Of C-Style Identifier.
    REL_LETTER_EOI_C,           ///< Letter at End Of C-Style Identifier.

    REL_LETTER_ABBREV,
    REL_LETTER_ABBREV_BEG,
    REL_LETTER_ABBREV_END,

    REL_MARK,			///< Mark/Marking. Marks the beginning of range selection. \em Selection of elements of structure.
    REL_CURSOR,			///< Cursor (symbol) of a line. \em Position in elements in structure.

    REL_TEXT,			///< Text of Letters.
    /* @} */

    /** \name Operation-Argument Result (Cache/Memoization) */
    /* @{ */
    REL_OP,			///< Operator
    REL_ARG,
    REL_ARG0,			///< First Operation Argument
    REL_ARG1,			///< Second Operation Argument

    REL_OP_CACHE,		///< Operation Output (Result) Cache (Memoization).
    REL_CACHING_OP,             ///< Caching Operation.
    /* @} */

    /** \name Multi-Resolution/Scale (Mip-Map) Representations. Used in for
        example Computer Vision. */
    /* @{ */
    REL_SCALE_UP2,              ///< Scaled Up by 2.
    REL_SCALE_DOWN2,            ///< Scaled Down by 2.
    /* @} */

    REL_VIEW_MODEL,             ///< View - \em Model.
    REL_MODEL_VIEW,             ///< Model - \em View.

    REL_NAMED_IMAGE_FEATURE,    ///< Named Image Feature.
    REL_IMAGE_FEATURE_NAME,     ///< Image Feature Name.

    // HTML 4 link types
    REL_ALTERNATE,
    REL_STYLESHEET,
    REL_START,
    REL_CONTENTS,
    REL_INDEX,
    REL_GLOSSARY,
    REL_COPYRIGHT,
    REL_CHAPTER,
    REL_SECTION,
    REL_SUBSECTION,
    REL_APPENDIX,
    REL_HELP,
    REL_BOOKMARK,

    /* ======================================================================= */

    REL_undefined_,	 ///< Special-relation to EMPTY relation SLOT.
    REL_NUM = REL_undefined_,
} __attribute__ ((packed)) REL_t;

extern REL_t g_REL_any_;

/* ========================================================================== */

/*!
 * Return non-zero if \p rel belongs to the PARENT-group.
 */
inline int
REL_is_PARENT(REL_t rel)
{
  return (rel == REL_PARENT);
}

/*!
 * Return non-zero if \p rel belongs to the CHILD-group.
 */
inline int
REL_is_CHILD(REL_t rel)
{
  return (rel == REL_CHILD);
}

/*!
 * Return non-zero if \p rel belongs to the SIBLING-group.
 */
inline int
REL_is_SIBLING(REL_t rel)
{
  return (rel == REL_SIBLING or
	  rel == REL_SIBLING_PREV or
	  rel == REL_SIBLING_NEXT);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Find out if \p rT matches or belongs to the catoory \p rM1.
 * \return true on match, false otherwise.
 */
bool REL_matchS(REL_t rT, REL_t rM1);

/*!
 * Find out if \p rT matches or belongs to the array of \p rM_N of relations
 * at \p rM.
 * \return true on match, false otherwise.
 */
bool REL_matchM(REL_t rT, const REL_t * rM, size_t rM_N);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Lookup relation type \p rel name as a C-string.
 *
 * \return C-string, 0 nullptr if relation type \p rel was unknown.
 */
const char * REL_getName(REL_t rel);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Lookup \em Dual Relation of \p rel.
 */
REL_t REL_getDual(REL_t rel);

/*! Perform Dualism-Check.
 *
 * Used when we want to assure that a pair of relation types in two-way relation
 * are compatible with each other.
 *
 * \return true if \p rel0 and \p rel1 are in dual, false otherwise.
 */
bool REL_duals(REL_t rel0, REL_t rel1);

bool REL_duals_check(REL_t rT0, REL_t rT1);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Return non-zero if \p rel belongs to the MASTER-group.
 */
inline bool
REL_is_MASTER_PLOT(REL_t rel)
{
    return (rel == REL_MASTER or
            rel == REL_MASTER_PLOT_VT or
            rel == REL_MASTER_PLOT_VB or
            rel == REL_MASTER_PLOT_HL or
            rel == REL_MASTER_PLOT_HR);
}

/*!
 * Return non-zero if \p rel belongs to the MASTER-group.
 */
inline bool
REL_is_MASTER(REL_t rel)
{
    return (rel == REL_MASTER or
            rel == REL_MASTER_TALKBUBBLE or
            rel == REL_MASTER_RELSYM_TARGET or
            rel == REL_MASTER_RELSYM_OWNER or
            REL_is_MASTER_PLOT(rel));
}

/*!
 * Return non-zero if \p rel belongs to the SLAVE_AXTICKS-group.
 */
inline bool
REL_is_SLAVE_AXTICK(REL_t rel)
{
    return (rel == REL_SLAVE_AXTICK or
            rel == REL_SLAVE_AXTICK_VT or
            rel == REL_SLAVE_AXTICK_VB or
            rel == REL_SLAVE_AXTICK_HL or
            rel == REL_SLAVE_AXTICK_HR);
}

/*!
 * Return non-zero if \p rel belongs to the SLAVE_LAYHINT-group.
 */
inline bool
REL_is_SLAVE_LAYHINT(REL_t rel)
{
    return (rel == REL_SLAVE_LAYHINT or

            rel == REL_SLAVE_LAYHINT_CADJ or

            rel == REL_SLAVE_LAYHINT_CADJ_LEFT_GROW or
            rel == REL_SLAVE_LAYHINT_CADJ_RIGHT_GROW or
            rel == REL_SLAVE_LAYHINT_CADJ_TOP_GROW or
            rel == REL_SLAVE_LAYHINT_CADJ_BOT_GROW or

            rel == REL_SLAVE_LAYHINT_CADJ_LEFT_SHRINK or
            rel == REL_SLAVE_LAYHINT_CADJ_RIGHT_SHRINK or
            rel == REL_SLAVE_LAYHINT_CADJ_TOP_SHRINK or
            rel == REL_SLAVE_LAYHINT_CADJ_BOT_SHRINK);
}

/*!
 * Return non-zero if \p rel belongs to the SLAVE_CADJ-group.
 */
inline bool
REL_is_SLAVE_CADJ(REL_t rel)
{
    return (rel == REL_SLAVE_LAYHINT_CADJ or

            rel == REL_SLAVE_LAYHINT_CADJ_LEFT_GROW or
            rel == REL_SLAVE_LAYHINT_CADJ_RIGHT_GROW or
            rel == REL_SLAVE_LAYHINT_CADJ_TOP_GROW or
            rel == REL_SLAVE_LAYHINT_CADJ_BOT_GROW or

            rel == REL_SLAVE_LAYHINT_CADJ_LEFT_SHRINK or
            rel == REL_SLAVE_LAYHINT_CADJ_RIGHT_SHRINK or
            rel == REL_SLAVE_LAYHINT_CADJ_TOP_SHRINK or
            rel == REL_SLAVE_LAYHINT_CADJ_BOT_SHRINK);
}

/*!
 * Return non-zero if \p rel belongs to the SELSYM-group.
 */
inline bool
REL_is_SLAVE_SELSYM(REL_t rel)
{
    return (rel == REL_SLAVE_SELSYM or
            rel == REL_SLAVE_SELSYM_UL or
            rel == REL_SLAVE_SELSYM_UR or
            rel == REL_SLAVE_SELSYM_LL or
            rel == REL_SLAVE_SELSYM_LR);
}

/*!
 * Return non-zero if \p rel belongs to the SLAVE-group.
 */
inline bool
REL_is_SLAVE(REL_t rel)
{
    return (rel == REL_SLAVE or
            rel == REL_SLAVE_TITLENAME or
            rel == REL_SLAVE_TYPENAME or
            rel == REL_SLAVE_DOC or
            rel == REL_SLAVE_KCOMB or
            rel == REL_SLAVE_ICON or
            rel == REL_SLAVE_ATTR or
            rel == REL_SLAVE_TALKBUBBLE or
            rel == REL_SLAVE_PROGRESS or
            rel == REL_SLAVE_RELSYM_FWD or
            rel == REL_SLAVE_RELSYM_BWD or
            REL_is_SLAVE_SELSYM(rel) or
            REL_is_SLAVE_CADJ(rel) or
            REL_is_SLAVE_AXTICK(rel));
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Return non-zero if \p rel belongs to the TEXT-group.
 */
inline bool
REL_is_TEXT(REL_t rel)
{
    return (rel == REL_TEXT);
}

/*!
 * Return non-zero if \p rel belongs to the CURSOR-group.
 */
inline bool
REL_is_CURSOR(REL_t rel)
{
    return (rel == REL_CURSOR);
}

/*!
 * Return non-zero if \p rel belongs to the LETTER_WS-group.
 */
inline bool
REL_is_LETTER_WS(REL_t rel)
{
    return (rel == REL_LETTER_WS or
            rel == REL_LETTER_WS_SPACE or
            rel == REL_LETTER_WS_HTAB or
            rel == REL_LETTER_WS_VTAB);
}

/*!
 * Return non-zero if \p rel belongs to the LETTER_EOL-group.
 */
inline bool
REL_is_LETTER_EOL(REL_t rel)
{
    return (rel == REL_LETTER_EOL or
            rel == REL_LETTER_EOL_MAC or
            rel == REL_LETTER_EOL_UNIX or
            rel == REL_LETTER_EOL_WIN);
}

/*!
 * Return non-zero if \p rel belongs to the LETTER-group.
 */
inline bool
REL_is_LETTER(REL_t rel)
{
    return (rel == REL_LETTER or

            rel == REL_LETTER_DIGIT or

            REL_is_LETTER_WS(rel) or
            REL_is_LETTER_EOL(rel) or

            rel == REL_LETTER_BOL or
            rel == REL_LETTER_BOT or
            rel == REL_LETTER_BOI_C or
            rel == REL_LETTER_EOI_C or
            rel == REL_LETTER_EOT or

            rel == REL_LETTER_ABBREV or
            rel == REL_LETTER_ABBREV_BEG or
            rel == REL_LETTER_ABBREV_END);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Return non-zero if \p rel belongs to the group of relations that
 * upon deletion of its related objects should be deleted as welll
 * (to the grave).
 */
inline bool
REL_followsDelete(REL_t rel)
{
    return (REL_is_SLAVE(rel) or
            REL_is_LETTER(rel));
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Return non-zero if \p rel belongs to the PRODUCER-group.
 */
inline bool
REL_is_PRODUCER(REL_t rel)
{
    return (rel == REL_PRODUCER or
            rel == REL_PRODUCER_BINCALL or
            rel == REL_PRODUCER_TALKBUBBLE);
}

/*!
 * Return non-zero if \p rel belongs to the CONSUMER-group.
 */
inline bool
REL_is_CONSUMER(REL_t rel)
{
    return (rel == REL_CONSUMER or
            rel == REL_CONSUMER_BINCALL or
            rel == REL_CONSUMER_TALKBUBBLE);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Return non-zero if \p rel belongs to the ALIGNER-group.
 */
inline bool
REL_is_ALIGNER(REL_t rel)
{
    return (rel == REL_ALIGNER or
            rel == REL_ALIGNER_VSCRIPT_SUB or
            rel == REL_ALIGNER_VSCRIPT_SUPER);
}

/*!
 * Return non-zero if \p rel belongs to the ALIGNEE_V-group.
 */
inline bool
REL_is_ALIGNEE_V(REL_t rel)
{
    return (rel == REL_ALIGNEE_V or
            rel == REL_ALIGNEE_VT or
            rel == REL_ALIGNEE_VB);
}

/*!
 * Return non-zero if \p rel belongs to the ALIGNEE_H-group.
 */
inline bool
REL_is_ALIGNEE_H(REL_t rel)
{
    return (rel == REL_ALIGNEE_H or
            rel == REL_ALIGNEE_HL or
            rel == REL_ALIGNEE_HR);
}

/*!
 * Return non-zero if \p rel belongs to the ALIGNEE-group.
 */
inline bool
REL_is_ALIGNEE(REL_t rel)
{
    return (rel == REL_ALIGNEE or
            REL_is_ALIGNEE_V(rel) or
            REL_is_ALIGNEE_H(rel) or
            rel == REL_ALIGNEE_VSCRIPT_SUB or
            rel == REL_ALIGNEE_VSCRIPT_SUPER);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Check if \p rel is a relation type that should be presented
 * \em graphically using a Relation \em Symbol (RELSYMs).
 *
 * \see ob_gen_RELSYMs()
 */
inline bool
REL_hasSYM(REL_t rel)
{
    return (rel == REL_PARENT or
            rel == REL_CHILD or
            rel == REL_ORIGINAL or
            rel == REL_COPY or
            rel == REL_PRODUCER or
            rel == REL_CONSUMER);
}

/*!
 * Generate a \c LETTER-rel from \p val.
 */
REL_t
REL_LETTER_from_wchar(wchar_t val);

/* ========================================================================= */

__attribute__ ((constructor))
void reldualtab_init();

/* ========================================================================= */
