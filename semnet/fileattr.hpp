/*!
 * \file fileattr.hpp
 * \brief SemNet (File) Standard/Extended Attribute Wrapper.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "../fkind.h"
#include "PMAGIC_enum.h"
#include "../substr_match.h"
#include "../csc.hpp"

namespace semnet { namespace filesystem {

/*! Attribute Type Code. */
typedef enum
{
    FILEATTR_any_,
    FILEATTR_ATIME,                ///< Time of last \em access.
    FILEATTR_MTIME,                ///< Time of last \em modification.
    FILEATTR_CTIME,                ///< Time of last \em status change.
    FILEATTR_SIZE,                 ///< Size in bytes.
    FILEATTR_BLKSIZE,              ///< Blocksize of file system I/O.
    FILEATTR_XATTR,                ///< Extended Attribute.
    FILEATTR_undefined_,
} __attribute__ ((packed)) FILEATTR_t;

/*! Attribute (of Directory or File). */
class FileAttr
{
public:
    FileAttr(const csc& name, const csc& val)
        : m_name(name), m_val(val) { }
    const csc& get_name() const { return m_name; }
    const csc& get_value() const { return m_val; }
    std::ostream& show(std::ostream& os) const;
private:
    csc m_name;        ///< \em Name (Local Attribute Path).
    csc m_val;         ///< \em Value.
};

}
}
