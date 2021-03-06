/*!
 * \file OB_enum.hpp
 * \brief Object Type Code.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "../utils.h"
#include "../pnw_types.h"
#include "../MATCH_enum.h"
#include "pmatch.hpp"

namespace semnet {

/*! Object Type Code. */
typedef enum {
    OB_any_,                 ///< Abstract Meta-type to indicate \em any kind of object.

    /*! Any Kind of \em Concrete File System Entity, such as Directory (\c Dir),
     * File (\c PREALFILE) Attribute (\c PATTR) (Name, Attribute, Extended Attribute),
     * File Content (\c PFCONT).
     */
    OB_URI,                     ///< Uniform Resource Index
    OB_PATH,                    ///< (File/Directory/Other1) Path

    OB_DEB,                     ///< \em Debian Package.
    OB_APTDIR,                  ///< \em APT Directory
    OB_APTFILE,                 ///< \em APT File

    /*!
     * File of \em Any Kind present in a File-System, including
     * directories and symbolic linkes.
     */
    OB_FILE_,                   ///< \em Any kind of File (Abstract).
    OB_DIR,			///< \em Directory File.
    OB_REALFILE,                ///< \em Real File Containing Data.
    OB_SYMLINKFILE,             ///< \em Symbolic Link File.
    OB_ELFFILE,                 ///< \em ELF File.
    OB_COMPFILE,                 ///< \em Compressed File.

    OB_FILETYPE,                ///< \em File Type Pattern.
    OB_DIRTYPE,                 ///< \em Directory Type Pattern.

    OB_ELFSYMTAB,               ///< ELF Symbol Table.
    OB_ELFSYM,                  ///< ELF Symbol.

    OB_FILEATTR,			///< \em Attribute (of for example File or Directory).

    // ---------

    OB_DUP,			///< \em Duplicate.

    // ---------

    OB_PATT_,			///< Pattern (Abstract).
    OB_ANY,			///< \em Any Kind of Pattern of given length.
    OB_COMB,			///< \em All Sub-Patterns (\em Non-Consecutive).
    OB_BIT,			///< \em Bit Constant Pattern.
    OB_LIT,			///< \em Literal/Constant/Terminal.
    OB_ALT,			///< \em Alternative (Range) Pattern.
    OB_REP,			///< (Sequencial) \em Repetition Pattern.
    OB_SIT,			///< \em Situation (Context) (Determined by position only) Pattern.

    OB_REQ,			///< (Pattern) \em Requirement (Limitation).

    OB_LOP,                    ///< \em Low-Level \em Operation.
    OB_GOP,                    ///< \em Graph (Network) \em Operation.
    OB_FOP,                    ///< \em File \em Operation.

    OB_NUMBER,			///< \em Number (Array).

    // ---------

    OB_HIT,			///< Pattern (Match) \em Hit.

    OB_SCAN,			///< Pattern Scan of (Hits).

    // Visual Objects

    OB_VSYM,			///< Symbol.

    OB_VGLYPH,			///< Glyph.

    OB_VTEX,			///< Texture.

    OB_VVIS,			///< General Visualizer.

    OB_VTXT,			///< Text.

    OB_VDIR,			///< (File System) Directory.
    OB_VFILE,			///< (File System) Regular file.

    /// \name Atoms.
    /* @{ */
    OB_VBIT,			///< Boolean value (Bit, checkbox).
    OB_VNUMZ,			///< Big Integer (Z).
    /* @} */

    /// \name Structures.
    /* @{ */
    OB_VSET,		  ///< Set (Vector, Matrix, ...) of Objects (stored as relation).
    /* @} */

    OB_VOP,			///< Operation Function (Operator).
    OB_VAPP,			///< Applier (of Operation).

    OB_VBUTT,			///< Button.

    OB_VPBAR,			///< Progress Bar.

    OB_VTIMER,			///< Timer.
    OB_VTIME,			///< Time.
    OB_VDATE,			///< Date.
    OB_VCLOCK,			///< Clock.

    OB_VPERM,			///< Permission.

    OB_UNIT,

    OB_undefined_,
    OB_NUM = OB_undefined_,
}  __attribute__ ((packed)) OB_t;

#define OB_FIRST (OB_t) (OB_any_ + 1)
#define OB_LAST (OB_t) (OB_undefined_ - 1)

extern OB_t g_OB_any_;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Check if \p oT is a File System \em File.
 * \return 1 if true, 0 otherwise.
 */
inline bool
OB_is_FILE(OB_t oT)
{
    return (oT == OB_FILE_ or
            oT == OB_DIR or
            oT == OB_REALFILE or
            oT == OB_SYMLINKFILE or
            oT == OB_ELFFILE);
}

/*!
 * Check if \p oT is a file.
 * \return 1 if true, 0 otherwise.
 */
inline bool
OB_is_PATT(OB_t oT)
{
    return (oT == OB_ANY or
            oT == OB_COMB or
            oT == OB_BIT or
            oT == OB_LIT or
            oT == OB_ALT or
            oT == OB_REP or
            oT == OB_SIT);
}

/*!
 * Check if \p oT is a \em cached (temporary) information that can be
 * deleted if needed.
 *
 * \return 1 if true, 0 otherwise.
 */
inline bool
OB_isCache(OB_t oT)
{
    return (oT == OB_DUP or
            oT == OB_ELFSYMTAB or
            oT == OB_ELFSYM or
            oT == OB_FILEATTR);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Find out if \p oT matches or belongs to the category \p oM1.
 * \return true on match, false otherwise.
 */
bool OB_matchS(OB_t oT, OB_t oM1);

/*!
 * Find out if \p oT matches or belongs to the array of \p oM_N of relations
 * at \p oM.
 * \return true on match, false otherwise.
 */
bool OB_matchM(OB_t oT, const OB_t * oM, size_t oM_N);

/* ---------------------------- Group Separator ---------------------------- */

const char * OB_getName(OB_t oT);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Lookup \c OB_t from str and return it.
 *
 * \param[in] case_flag is non-zero for Case-Sensitive Lookup.
 */
OB_t
OB_lookup(const char * str, bool case_flag);

/*!
 * Complete \c OB_t from \p str and return it.
 *
 * \param[in] pmm Type of Pmatch.
 * \param[out] results_ret is resulting return value
 * \param[out] match_idxB_ret is matching indexes
 */
int
OB_completeName(char * str, pMode pmm,
                 int results_ret[OB_NUM],
                 uint match_idxB_ret[OB_NUM]);

/* ---------------------------- Group Separator ---------------------------- */

}
