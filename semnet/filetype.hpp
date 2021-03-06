/*!
 * \file filetype.hpp
 * \brief File Type Specifier.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "../cc_features.h"
#include "../dfmt.h"
#include "../csc.hpp"
#include "../range.hpp"
#include "patt.hpp"
#include "OP_enum.hpp"
#include <unordered_set>
#include <vector>

namespace semnet { namespace filesystem {

class File;
class RegFile;

/*! File Type Pattern. */
class FileType {
    friend class RegFile;
public:
    /*! \em Preferred File Type Recognition Style.
     *
     * \similar \c precog in filedb.el
     * \see \c Sit and \c PRPOS_t
     * \see \c PCTX_t
     */
    typedef enum {
        PRECOG_any_,              ///< \em Any - Use what is available (default).
        PRECOG_default = PRECOG_any_, ///< \em Default is alias for Any.
        PRECOG_NAME,              ///< In \em Name of File.
        PRECOG_CONTENTS,          ///< In \em Contents of File.
        PRECOG_NAME_AND_CONTENTS, ///< In \em Name \em and \em Contents of \em File.
        PRECOG_NAME_OR_CONTENTS,  ///< In \em Name \em or \em Contents of \em File.
    } __attribute__ ((packed)) PRECOG_t;

    typedef std::unordered_set<FileType*> Reg; ///< Set of Registered File Types.

    FileType(patterns::Base * nfmt = nullptr,
             patterns::Base * cfmt = nullptr,
             const char * name = nullptr,
             const char * doc = nullptr,
             DFMT_t dfmt = DFMT_any_,
             PRECOG_t precog = PRECOG_NAME_AND_CONTENTS);

    virtual ~FileType() { ms_reg.erase(this); }

    FileType * set_name(const char * name) { m_name.assign(name); return this; }
    FileType * set_doc(const char * doc) { m_doc.assign(doc); return this; }

    virtual const csc& get_pathL() const { return m_name; }

    bool match(RegFile* file,
                bir roi = bir::full(),
                PRECOG_t override_precog = PRECOG_any_) const;

    static FileType * of(RegFile* file,
                         bir roi = bir::full(),
                         PRECOG_t override_precog = PRECOG_any_);

    DFMT_t get_dfmt() const { return m_dfmt; }
    bool contains(DFMT_t dfmt) const { return DFMT_match(get_dfmt(), dfmt); }
    bool is_audio() const { return DFMT_is_AUDIO(m_dfmt); }
    bool is_image() const { return DFMT_is_IMAGE(m_dfmt); }
    bool is_video() const { return DFMT_is_VIDEO(m_dfmt); }

    void add_op(OP_t op) { m_ops.push_back(op); }
#if HAVE_CXX11_INITIALIZER_LISTS
    void add_ops(const std::initializer_list<OP_t>& ops) { m_ops.insert(m_ops.end(), ops); }
#endif
    void set_ops(const std::vector<OP_t>& ops) { m_ops = ops; }
    const std::vector<OP_t>& get_ops() const { return m_ops; }

private:
    void init();

private:
    patterns::Base* m_nfmt;     ///< Name Format.
    patterns::Base* m_cfmt;     ///< Contents Format (Programming Language).
    csc m_name;                 ///< Name of pattern if any.
    csc m_doc;                  ///< Documentation of pattern if any.

    static Reg ms_reg;          ///< Registered File Types.
    DFMT_t m_dfmt;              ///< Data Format Code.
    PRECOG_t m_precog:3;        ///< \em Preferred Recognition.

    std::vector<OP_t> m_ops;    ///< File Type Operations.
};

namespace gen {

inline
FileType* filetype_N(patterns::Base* nfmt,
                     DFMT_t dfmt = DFMT_any_,
                     const char * name = nullptr,
                     const char * doc = nullptr,
                     FileType::PRECOG_t precog = FileType::PRECOG_NAME)
{
    return new FileType(nfmt, nullptr, name, doc, dfmt);
}

inline
FileType* filetype_C(patterns::Base* cfmt,
                     DFMT_t dfmt = DFMT_any_,
                     const char * name = nullptr,
                     const char * doc = nullptr,
                     FileType::PRECOG_t precog = FileType::PRECOG_CONTENTS)
{
    return new FileType(nullptr, cfmt, name, doc, dfmt, precog);
}

/*!
 * Create \em File \em Type with a file \em name format pattern \p nfmt \em and
 * file \em content format pattern \p cfmt of type \c DFMT_t.
 */
inline
FileType* filetype_NaC(patterns::Base* nfmt,
                       patterns::Base* cfmt,
                       DFMT_t dfmt = DFMT_any_,
                       const char * name = nullptr,
                       const char * doc = nullptr,
                       FileType::PRECOG_t precog = FileType::PRECOG_NAME_AND_CONTENTS)
{
    return new FileType(nfmt, cfmt, name, doc, dfmt);
}

/*!
 * Create \em File \em Type with a file \em name format pattern \p nfmt \em or
 * file \em content format pattern \p cfmt of type \c DFMT_t.
 */
inline
FileType* filetype_NoC(patterns::Base* nfmt,
                       patterns::Base* cfmt,
                       DFMT_t dfmt = DFMT_any_,
                       const char * name = nullptr,
                       const char * doc = nullptr,
                       FileType::PRECOG_t precog = FileType::PRECOG_NAME_OR_CONTENTS)
{
    return new FileType(nfmt, cfmt, name, doc, dfmt);
}

}
}
}
