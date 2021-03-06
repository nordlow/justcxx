/*! \file file.hpp
 * \brief File System File.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <boost/noncopyable.hpp>

#include "fileattr.hpp"
#include "obmr.hpp"
#include "OB_enum.hpp"
#include "../chash.hpp"
#include "../dfmt.h"
#include "../fkind.h"
#include "PMAGIC_enum.h"
#include "../substr_match.h"
#include "../statutils.h"
#include "../substr_match.hpp"
#include <vector>
#include <memory>

class pOp;

namespace semnet {
namespace filesystem {

/*! Default mode_t for creating \c Dir sub-files. */
#define PDIR_SUB_MODE (S_IWGRP | S_IWOTH)

class FileAttr;
class Dir;

/*!
 * Separator C-string for specifying file \em scope of tags, literals etc.
 * Proably ':' or '/'.
 */
#define FILE_SCOPE_OP_CSTR (":")

/*! SemNet File Handle (View).
 * Is only supposed to be created by factor methods typically in \c Dir
 * triggered by external notifications (Inotify). */
class File : public boost::noncopyable, public ObMr
{
    friend class Dir;
    friend class SymLink;
    friend class RegFile;
public:
    typedef std::vector<FileAttr> Attrs;

    /*!
     * File \em Copy Alternatives.
     * \see cp(1)
     */
    typedef enum {
        COPY_default_
    } COPY_t;

    /*!
     * File \em Rename Alternatives.
     * \see mv(1)
     */
    typedef enum {
        RENAME_default_
    } RENAME_t;

    virtual OB_t get_type() const { return OB_FILE_; };

    // TODO: Support wrappers for
    // int mkstemp(char *template);
    // int mkostemp (char *template, int flags);
    // int mkstemps(char *template, int suffixlen);
    // int mkostemps(char *template, int suffixlen, int flags);

    /*! Open \c this (if it is not already open).
     * \return 1 if \c this was openend, 0 if already opened, -1 if opening failed.
     */
    virtual int open() const { return -1; };

    /*! Close \c this (if it is not already closed).
     * \return 1 if file was closed, 0 if file was already closed, -1 if
     *         opening failed.
     */
    virtual int close() const { return -1; };

    /*! Open File Descriptor \p fd.
     * \param[in] flags must include one of the following access modes:
     * \c O_RDONLY, \c O_WRONLY, or \c O_RDWR. These request opening the
     * \c file read-only, write-only, or read/write, respectively.
     */
    int open_fd(int& fd, int flags = O_RDONLY, mode_t mode = PDIR_SUB_MODE) const;

    /*! Close File Descriptor \p fd. */
    int close_fd(int& fd) const;

    //! Get File \em Descriptor.
    virtual int get_fd() const { return -1; }

    ssize_t read(size_t len, char * dat, off64_t offset = 0);

    /*! \em Rename/Move \p this to \p newpath. */
    virtual int rename(const char * newpath, RENAME_t flags = RENAME_default_);

    /*! \em Copy \p this into \p newpath. */
    virtual int copy_into(const char * newpath, COPY_t flags = COPY_default_);
    /*! \em Copy \p this into \p dir. */
    virtual int copy_into(Dir * dir, COPY_t flags = COPY_default_);

    /*! \em Remove/Delete/Erase \p this.
     * \return 0 on success, -1 on error (see \c errno).
     */
    virtual int remove();

    /*! Signal that we are \em done using file for now. */
    virtual int done() const;

    /*! Get Parenting Directory. */
    Dir * get_parent() { return m_parent; }
    const Dir * get_parent() const { return m_parent; }

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Called when externally \em accessed. */
    virtual void ext_accessed_cb() { /* m_access_cnt++; */ }
    /*! Called when externally \em changed. */
    virtual void name_externally_changed_cb() { m_name_obs_flag = true; }

    virtual const csc& get_pathL() const { return m_name; }

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Update File.
     */
    virtual int update();

    /*!
     * Flush (fsync) changes made to \c this to disk.
     * \param data_only_flag true if we skip flushing file meta data.
     */
    virtual int sync(bool data_only_flag);

    /*! Update File Statistics.
     * \return > 0 if stat was updated, 0 if stat was already uptodate, -1 if
     *         stat failed.
     */
    int update_stat() const;

    virtual int load(bool recurse_flag = false, bool cscan_flag = false);
    virtual void unload() const;

    /*! \em Try to Recursively Load all the Parent-Directories and Bottom-Most
     * Dir/File contained in the \em Global Directory Path \p pathF.
     * \return the bottom-most file, or \c nullptr if not found.
     */
    static File * load_path(const csc& pathF);
    static File * load_path(const char * pathF);

    /*! Rescan single file magic of type \p pmagic and create new pkinds and set
     * xattr \p xns accordingly.
     *
     * \param[in] pmagic Type of Magic to scan for.
     * \param[out] magic_ret Magic String.
     */
    int rescan_magic_single (PMAGIC_t pmagic, const char* xns, csc * magic_ret = nullptr);

    /*!
     * Synchronize (with external changes)) file magic of type \p pmagic \c this.
     * \return 1 if magic was assured, 0 otherwise.
     */
    int update_magic(PMAGIC_t pmagic);

    /* ---------------------------- Group Separator ---------------------------- */

    bool is_readable()   const { return update_stat() >= 0 and stat_is_readable(m_stat.get()); }
    bool is_writable()   const { return update_stat() >= 0 and stat_is_writable(m_stat.get()); }
    bool is_executable() const { return update_stat() >= 0 and stat_is_executable(m_stat.get()); }
    bool is_deletable() const { return update_stat() >= 0 and stat_is_writable(m_stat.get()); }

    /* ---------------------------- Group Separator ---------------------------- */

    // See: http://0pointer.de/blog/projects/locking
    // See: http://www.gnu.org/software/hello/manual/libc/File-Locks.html
    virtual bool lock();
    virtual bool unlock();

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Get Real (Current) \em Modification Time.
     * \return modification time, or 0 if none was found.
     */
    time_t get_mtime() const { return (m_stat or update_stat() >= 0) ? m_stat.get()->st_mtime : 0; }

    /*! Get Real (current) \em Status Change Time.
     * \return modification time, or 0 if none was found.
     */
    time_t get_ctime () const { return (m_stat or update_stat() >= 0) ? m_stat.get()->st_ctime : 0; }

    /*! Get (Content) Byte Size. */
    off_t get_size() const { return (m_stat or update_stat() >= 0) ? m_stat.get()->st_size : 0; }

    /*! Get (Content) Bit Size. */
    bix bitsize() const { return static_cast<bix>(8*get_size()); }

    /*! Get Block Size.
     * \return block size
     */
    blksize_t get_blksize() const { return (m_stat or update_stat() >= 0) ? m_stat.get()->st_blksize : 0; }

    /*! Get Mode.
     * \return file size
     */
    mode_t get_mode() const { return (m_stat or update_stat() >= 0) ? m_stat.get()->st_mode : 0; }

    /*! \em Symbolic-Link File to destination.
     * \note Boost.Filesystem V2 Supports this: http://www.boost.org/doc/libs/1_49_0/libs/filesystem/v2/test/operations_test.cpp
     */

    /*! \em Hard-Link File to destination.
     * \note Boost.Filesystem V2 Supports this: http://www.boost.org/doc/libs/1_49_0/libs/filesystem/v2/test/operations_test.cpp
     */

    /*! Get Number of \em References (Hard Links) to \c this file, which is 1
     * for normal unlinked storage.
     * \return reference count. */
    nlink_t get_link_count() const { return (m_stat or update_stat() >= 0) ? m_stat.get()->st_nlink : 0;}

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Check if \c this is \em newer than \p a. \see Bash operator -nt */
    bool is_newer_than(const File * a) const { return get_mtime() < a->get_mtime(); }

    /*! Check if \c this is \em older than \p a. \see Bash operator -ot */
    bool is_older_than(const File * a) const { return get_mtime() > a->get_mtime(); }

    /* ---------------------------- Group Separator ---------------------------- */

protected:
    /*! Construct File. */
    File() : ObMr() { init(); }

    /*! Construct File from path \p pathL.     */
    File(const csc& pathL, class Dir * parent,
         FKIND_t fkind = FKIND_undefined_, struct stat * statp = nullptr)
        : ObMr(), m_name(pathL), m_parent(parent) {
        init(fkind, statp);
    }

    virtual ~File();

    /*! Set the \em Attribute named \p name to the value \p value of size
     * \p size using the flags \p flags.
     *
     * \param value name is a nullptr-terminated string. Warning: This means
     * that it is currently \em impossible to store arbitrary \em binary
     * coded data in extended attributes names.
     */
    virtual int set_attr(const char* name, const void* value, size_t size, int flags);

    int set_attr_csc(const char* name, const csc& value, int flags);

    /*! Check if the \em Attribute named \p name is present.
     * \return 1 if attribute is present, 0 if attribute is not present
     *         but can set, -1 otherwise.
     */
    virtual int has_attr (const char* name) const;

    /*! Read the \em Attribute named \p name into \p value of size \p size.
     * \return length of attribute upon success, -1 upon other error.
     */
    virtual int get_attr (const char* name, void* value, size_t size) const;

    virtual int get_attr_csc (const char* name, csc& value_ret) const;

    /*! Remove the \em Attribute named \p name.
     * \return 0 on success, -1 on error.
     */
    virtual int remove_attr (const csc& xan);

    /*! Remove xattr named \p xan if smaller than content size.
     * \return 1 if an existing attribute was removed, 0 otherwise.
     */
    int remove_attr_if_too_small (const csc& xan);

    const Attrs& get_attrs(bool rescan_flag = false);
    void unload_attrs();

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Set timestamp to \p t. */
    File * set_attr_timestamp_time (time_t t);

    /*! Get timestamp.
     * \return timestamp or 0 if no timestamp was present.
     */
    uint64_t get_attr_timestamp () const;

    void update_attr_timestamp_time (time_t t);

    int has_uptodate_attr_timestamp () const;

    virtual const uchar * get_chash(chash::chashid hid = chash::CHASH_SHA2_256) const {
        //hid = CHASH_SHA2_256;
        PTODO("Not implemented\n"); return nullptr;
    }

    /* ---------------------------- Group Separator ---------------------------- */

public:
    virtual std::ostream& show(std::ostream& os) const;

    virtual theight_t get_tree_height() const { return 0; } // Note: Simple!

    virtual int update_FKIND(bool force_flag = false);
    FKIND_t get_FKIND() const {
        const_cast<File*>(this)->update_FKIND();
        return m_fkind;
    }

    /*!
     * Check whether \c this is \em likely to be a \em backup file or not.
     */
    virtual bool is_probably_backup() const
    {
        auto pathL = get_pathL();
        /* backup files (overrides others by darking) */
        auto ret = (ends(pathL, '~') or
                    ends(pathL, ".bak", 4));
        // \todo Check for precence of FILE if FILE~ or FILE.bak exists
        // (optionally in ".." if parent dir is named ".backups" ".backup".
        return ret;
    }

    static size_t get_open_count();
    virtual csc path() const;

protected:
    void init(FKIND_t fkind = FKIND_undefined_,
              const struct stat * statp = nullptr);

    void log_open() const;
    void log_close() const;
protected:
    csc     m_name;             ///< Local File \em Path (Name).
    Dir * m_parent;            ///< Parent Directory.
    Attrs * m_attrs;            ///< (Extended) Attributes.

    mutable std::unique_ptr<struct stat> m_stat; ///< Only reflects filesystem state.

    mutable FKIND_t m_fkind:4;  ///< File Kind (Class).
    mutable bool m_update_attr_flag:1; ///< Flags that we should try to load from xattrs.
    mutable bool m_name_obs_flag:1; ///< Flags that name is obselete.
    mutable bool m_magic_obs_flag:1; ///< Flags that magic is obselete.
    static void close_oldest();
};

inline bool ob_is_file_link(const Ob * ob)
{
    int ret = 0;
    if (const auto file = dynamic_cast<const File*>(ob)) {
        ret = (file->get_FKIND() == FKIND_SPECIAL_LNK);
    }
    return ret;
}

}
}
