/*! \dir: aptdir.hpp
 * \brief APT Directory
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "uri.hpp"
#include "../xhash.hpp"

namespace semnet {
namespace filesystem {

/*! APT Dir or Directory. */
class APTDir : public URI
{
public:
    virtual OB_t get_type() const { return OB_APTDIR; }
    APTDir(const csc& name) : URI(name) {}
    virtual ~APTDir() {}
    URI* get_sub(const csc& name, bool dir_flag, bool * load_flag = nullptr);
protected:
    URI* new_sub(const csc& name, bool dir_flag);
    URI* lookup_sub(const csc& name);
private:
    /*!
     * \em Sub-Files/Dirs \em Name Hash Table. Hashes from
     * string to pointer. Used by inotify \c speedup lookups from a
     * working-dir-relative filename to a \c Ob typically a sub-
     * \c Dir / @c File.
     */
    mutable xHash m_subs_hash;
};

}
}
