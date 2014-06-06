/*! \file dir.hpp
 * \brief semnetDB Directory
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "file.hpp"
#include "patt.hpp"
#include "../statutils.h"
#include "../vcs.hpp"
#include <iosfwd>
#include <unordered_map>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <algorithm>
#include <memory>
#include <system_error>

#ifdef HAVE_SYS_INOTIFY_H
struct inotify_event;
#endif

#define PDIR_CACHED_PATHF (1)

namespace semnet {
namespace filesystem {

class Dir;

/*! SemNet File System Directory Handle. */
class Dir : public File {
    friend class File;
    friend class semnet::pReg;
public:
    typedef std::unordered_map<int, Dir*> Watches;
    typedef std::unordered_map<csc, File*> Subs;

    typedef Subs::iterator iterator;
    typedef Subs::const_iterator const_iterator;

    //typedef tree_iterator; // \todo Support File System Tree Iterators (should check for cyclee using a unordered_map accessed)
    //typedef const_tree_iterator; // \todo Support File System Tree Iterators (should check for cyclee using a unordered_map accessed)

    virtual OB_t get_type() const { return OB_DIR; };

    /*! Get \em Root Directory. */
    static Dir* root();

    /*! Get \em Home Directory of current user. */
    static Dir* home(const csc * user = nullptr);

/* ---------------------------- Group Separator ---------------------------- */

    /*! Open \c this (if it is not already open).
     * \return 1 if \c this was openend, 0 if already opened, -1 if opening failed.
     */
    virtual int open() const;

    /*! Close \c this (if it is not already closed).
     * \return 1 if file was closed, 0 if file was already closed, -1 if
     *         opening failed.
     */
    virtual int close() const;

    //! Get Directory \em Descriptor.
    int get_fd() const { if (not m_ds) { open(); } return m_ds ? ::dirfd(m_ds) : -1; }

/* ---------------------------- Group Separator ---------------------------- */

    /// @group External File System Operations.
    /// \{
    // TODO: Disable inotify for \c this and modify m_subs directly.

    /*! Create Sub-Directory to \c this with local name \p pathL.
     * \return 0 on success, -1 on error and see \c errno. */
    int mkdir_sub(const csc& pathL, mode_t mode = PDIR_SUB_MODE);

    /*! Create a Unique Temporary Sub Directory of \c this.
     * \param[in] prefix of directory name.
     * \see POSIX.1-2008 mkdtemp() */
    csc mkdtemp_sub(const csc& pathL, mode_t mode = PDIR_SUB_MODE,
                    const csc& prefix = "") {
        csc templ = prefix + "XXXXXX"; // template
        auto ret = mkdtemp(const_cast<char*>(templ.data()));
        if (ret) {
            return ret;
        } else {
            throw std::system_error();
        }
    }
    /*! Open/Create Sub-File/Directory to \c this with local name \p pathL.
     * \return 0 on success, -1 on error and see \c errno. */
    int open_sub(const csc& pathL, int flags, mode_t mode = PDIR_SUB_MODE);
    /*! Create Sub-File/Directory to \c this with local name \p pathL.
     * \return 0 on success, -1 on error and see \c errno. */
    int create_sub(const csc& pathL, mode_t mode = PDIR_SUB_MODE) { return open_sub(pathL, O_CREAT|O_WRONLY|O_TRUNC, mode); }
    /*! Create Sub-FIFO to \c this with local name \p pathL.
     * \return 0 on success, -1 on error and see \c errno. */
    int mkfifo_sub(const csc& pathL, mode_t mode = PDIR_SUB_MODE);

    /*! Create Sub-Link to \c this with local name \p pathL referencing the full
     *  target path \p pathTF.
     * \return 0 on success, -1 on error and see \c  errno. */
    int mklink_sub(const csc& pathL, const csc& pathFT, mode_t mode = PDIR_SUB_MODE);

    /*! Remove Sub-File/Directory to \c this with local name \p pathL.
     * \return 0 on success, -1 on error and see \c errno. */
    int remove_sub(const csc& pathL, bool dir_flag);
    int rmdir_sub(const csc& pathL) { return remove_sub(pathL, true); }
    int remove_sub(const csc& pathL) { return remove_sub(pathL, false); }

    /*! \em Remove/Delete/Erase @this directory \em tree including all its childs.
     * WARNING: THIS DELETES WHOLE TREE from root \c this!
     * \return 0 on success, -1 on error and see \c errno. */
    virtual int remove();

    /// \}

/* ---------------------------- Group Separator ---------------------------- */

    /*! \em Load Sub-Files and Directories under \c this (that are not already loaded).
     * \param depth is depth from file system root directory
     * \param recurse_flag Recurse if 1, don't otherwise. */
    virtual int load(bool recurse_flag = false, bool cscan_flag = false);

    /*! Unload Directory Tree. */
    virtual void unload();

    /*! \em Relative Load at \c this to the relative or absolute path \p pathR. */
    File* load_to(const csc& pathR);
    File* load_to(const char * pathR) {
        // WARNING: Only works if boost::cref is *NOT* copy constructed in
        // File* relative_load_to(const csc& pathR)
        return load_to(csc(pathR// boost::cref(pathR)
                           ));
    }

    /*! \em Created and \em Relative Load at \c this to the relative or absolute path \p pathR. */
    File* dig_to(const char * pathR) { return load_to(pathR); }

/* ---------------------------- Group Separator ---------------------------- */

    virtual const uchar * get_chash(chash::chashid hid = chash::CHASH_SHA2_256) const;

/* ---------------------------- Group Separator ---------------------------- */

    /*! Count Number of \em Sub-Duplicates common to \c this and \p pB. */
    int countSubDups(const Dir * pB);

    static Dir * lookup_watch(int wd) { auto watch = m_watches.find(wd); return watch->second; }
    static int get_watch_count() { return m_watches.size(); }

/* ---------------------------- Group Separator ---------------------------- */

    virtual std::ostream& show(std::ostream& os) const;

    virtual tdepth_t get_tree_depth() const;
    virtual theight_t get_tree_height() const;

    /*! Get Size of Tree with root \c this.
     * \note Behaviour equivalent to UNIX disk usage command line tool \c du. */
    tsize_t get_tree_csize() const;

    /*!
     * \em Compare \em Tree-Size (\c tsize) of \p pA and \p pB.
     */
    friend int dir_cmp_tree_csize(const Dir * pA, const Dir * pB) {
        return pnw::cmpsgn(pA->get_tree_csize(),
                           pB->get_tree_csize());
    }

    /*! Check whether \c this is the File System \em Root Directory.
     * \return true if root, false otherwise.
     */
    bool is_root() const { return get_pathL().empty(); }
    /*! Check whether \c this \em contains a sub-file/directory named \p name. */
    bool contains(const csc& name) const { return get_pathL() == name; }

    virtual csc path() const {
#if PDIR_CACHED_PATHF
        if (m_pathF.empty()) { m_pathF = path_local(); } return m_pathF;
#else
        return path_local();
#endif
    }
    csc path_local(bool typed_flag = false) const;

protected:
    iterator begin() { return m_subs.begin(); }
    iterator end() { return m_subs.end(); }
    const_iterator begin() const { return m_subs.begin(); }
    const_iterator end() const { return m_subs.end(); }

    /*! Construct new \c Dir at \p dirP having local name \p pathL
     * and \p statp (reuse \c lstat() result from constructer).
     */
    Dir(const csc& pathL, Dir * dirP = nullptr, struct stat * statp = nullptr);
    virtual ~Dir();

    virtual int idle_cb();

#ifdef HAVE_SYS_INOTIFY_H
    static int get_inotify_fd() { return m_inotify_fd; }
    static void inotify_single(const struct inotify_event *event);
    static void inotify_move(const struct inotify_event *eventF,
                             const struct inotify_event *eventT);
    /*! Handle \em Inotify Event addressed to a \em Sub/Child File/Directory of
     * \c this. Sub/Child name is given by \p event.
     * \return 1 if event was processed, 0 otherwise.
     */
    void inotify_sub(const struct inotify_event *event);
    void inotify_create_subs(struct inotify_event * const events[], int events_num);
    void inotify_delete_subs(struct inotify_event * const events[], int events_num);
#endif

    /*! Return \em Sub-File/Directory named \p name.
     * \return sub
     */
    virtual File * lookup_sub(const csc& name);

    int openat_here(const csc& nameS, int flags = O_RDONLY, mode_t mode = PDIR_SUB_MODE) const;

    /*! Return \em Sub-File/Directory named \p name.
     * \param[out] load_flag is set if sub was loaded.
     * \return sub
     */
    virtual File * get_sub(const csc& name, bool dir_flag = false, bool * load_flag = nullptr);
    /*!
     * Create New sub-file/directory to \c this of type \p sub_oT.
     *
     * \param pathP Full Parent Path (to this). If nullptr calculate from this. \todo Move this to member of Dir.
     * \param nameS Local Sub Path.
     * \param sub_oT Type of Sub-Object to create.
     * \return the sub.
     */
    File * new_sub(const csc& pathP, const csc& nameS);

    File* load_sub(const csc& pathP, const csc& nameS, bool dir_flag = false);
    File* load_sub(const csc& pathP, const char * nameS, size_t nameS_N = 0, bool dir_flag = false);

    virtual void set_tree_depth(tdepth_t tdepth) const;
    virtual void set_tree_height(theight_t theight) const;

public:
#ifdef HAVE_SYS_INOTIFY_H
    static int m_inotify_fd;    ///< Inotify File Descriptor.
#endif

private:
    /*! Construct File System Root. */
    Dir();

    void init();

/* ---------------------------- Group Separator ---------------------------- */

    /*! Obselete Tree Digest. */
    void unload_tdig();

    /*! Obselete subs statistics. */
    void unload_subs_stat();

    /*! Update Version Control Directory State. */
    VCS_t update_VCstate(const REL_t* rM, size_t rM_N,
                         const OB_t* oM, size_t oM_N);

    VCS_t update_VCstate() {
        const REL_t rM[] = { REL_CHILD }; const size_t rM_N = ARRAY_LENGTH(rM);
        const OB_t oM[] = { OB_DIR }; const size_t oM_N = ARRAY_LENGTH(oM);
        return Dir::update_VCstate(rM,rM_N, oM,oM_N);
    }

    int update_VCS_state_from_sub_creation();

    /*! Update Internal Tree \em Hash (Checksum) of the Directory (Tree) \c this.
     * \see http://en.wikipedia.org/wiki/Hash_tree
     */
    uint64_t update_chash(chash::chashid hid = chash::CHASH_SHA2_256) const;

    /*! Update Internal Statistics about \p dir. */
    void update_all();

private:
    mutable DIR* m_ds;          ///< Directory Stream.

#if PDIR_CACHED_PATHF
    mutable csc m_pathF;        ///< Cached \em Full Path.
#endif

    // Cached Statistics of Dir-File tree
    mutable tdepth_t  m_tdepth;  ///< Tree Depth.
    mutable theight_t m_theight; ///< Tree Height. File has height 0.
    mutable tsize_t   m_tcsize; ///< Tree Content Size. Sum of Size of all sub-dirs/files.

    // TODO: Move this logic into a new Objecgt pVCDir : public Dir
    mutable VCS_t m_vctype;     ///< Type of \em Version Control (VC) if any.
    mutable VCS_STATUS_t m_vcstate; ///< Version Control State.
    mutable std::unique_ptr<CDigestF> m_tdig;       ///< Tree Content Digest.

#ifdef HAVE_SYS_INOTIFY_H
    int m_watch_fd;             ///< Inotify Watch File Descriptor.
#endif

    /*! \em Sub-Files/Dirs \em Name Hash Table. Hashes from
     * string to pointer. Used by inotify to \c speedup lookups from a
     * working-dir-relative filename to a \c Ob typically a sub-
     * Dir/File.
     */
    Subs m_subs;

private:
    // static stuff
    static Dir* m_root;        ///< File-System Root Directory.
    static Watches m_watches; ///< \em File Watch Hash Table: fd => Dir*.

    /*! Watch \c this. */
    void watch_me();
    /*! Unwatch \c this. */
    void unwatch_me();
    /*! Unwatch \c it. */
    static void unwatch_at(Watches::iterator& position);
};

}
}
