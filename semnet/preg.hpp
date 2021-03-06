/*!
 * \file preg.hpp
 * \brief SemNet Engine Registry.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "pmatch.hpp"
#include "../fkind.h"
#include "../dfmt.h"
#include "../chash.hpp"
#include "../csc.hpp"
#include "../qsort_mt.h"

#include "OB_enum.hpp"
#include "HISTLOG.hpp"
#include "rels.hpp"
#include "patt.hpp"

#ifdef HAVE_SYS_INOTIFY_H
#include <sys/inotify.h>
#endif

#ifdef HAVE_MAGIC_H
#include <magic.h>
#endif

#include "../xhash.hpp"

#include <unordered_set>

namespace semnet {

#define PREG_RMAGIC_SIZE (64)   ///< Size of Random Magic in bytes.
/*! Hash Number (Magic) for the \em whole SemNet Registry. */
extern uint8_t g_oreg_rmagic[PREG_RMAGIC_SIZE];

class Ob;

class Refs;
class Unit;

namespace filesystem {
class File;
class Dir;
class RegFile;
class APTFile;
class APTDir;
class Deb;
}

class URI;

namespace patterns {
class Lit;
class Alt;
}

class Iter;

/* ========================================================================= */

/*! Object Registry. */
class pReg {
    friend class Ob;
    friend class filesystem::File;
    friend class filesystem::RegFile;
    friend class filesystem::Dir;
    // friend class Refs;
public:

    /*! \em Create (Allocate and Initialize) a Registry and return it as a
     *     pointer.
     * \param num initial room for Ob pointer arrays.
     */
    pReg() : m_obR(), m_obN(), m_rootAPT(nullptr), m_tags(128), m_default_hid(chash::CHASH_SHA1_160)
    { init(); }

    ~pReg();

/* ---------------------------- Group Separator ---------------------------- */

    /*! \em Add \p ob to Registry.     */
    void add1(Ob * ob);

    /*! \em Add \p obA and \p obB to Registry.     */
    void add2(Ob * obA, Ob * obB);

    /*! \em Add \p obs_N number of \c Ob at \p obs to Registry.     */
    void addM(Ob *const* obs, size_t obs_N);

/* ---------------------------- Group Separator ---------------------------- */

    //! Load whole \p pathF and return bottom-most file.
    URI * load_APT_URI(filesystem::APTDir * dir, const csc& pathF);

    //! Load Debian \em Package named \p name of length name_N.
    filesystem::Deb * load_Deb(const csc& name);

    /*! Load APT information from \p apt_file
     * \return APT file tree root
     */
    URI * load_apt_tree(filesystem::RegFile * apt_archive);

    void load_apt_file_pkg(filesystem::APTDir * apt_root, const std::string& line);

    /*! Load APT file at \p pathF relative to root directory \p root.     */
    filesystem::APTFile * load_aptfile(const filesystem::APTDir* root, const csc& pathF);

public:
    filesystem::APTDir* get_APTRoot();

public:
    /*! Iterate the Registry at \p preg.     */
    void iter(int timeout_ms = 0);

    /*! Print Registry \em Range at \p preg.
     */
    std::ostream& show(std::ostream& os, size_t iB = 0, size_t iE = 0) const;

    const uint8_t *get_rmagic() const { return m_rmagic; }

/* ========================================================================= */

    size_t ob_count() const { return m_obR.size(); }

/* ========================================================================= */

    /*! Remove all objects of type \p oT present in registry \p preg.     */
    void deleteAllOfType(OB_t oT);

/* ---------------------------- Group Separator ---------------------------- */

    /*! Search in Registry \p preg for object matching \c g_currStr.
     * \param[in] next_flag nth search for nth hit. meaning 1 prev/next hit and 0 for
     *        current hit.
     * \return > 0 if a new non-zero hit was found and selected,
     *         -1 if nothing matched, 0 if existing selection matched search.
     */
    int search(GOREL_t gorel, GODIR_t godir, bool next_flag);

    /*! Find \em old \em altered objects and move them to the end of \p preg
     * (after new ones).
     */
    void move_altered_to_new();

    /*! Sort New Part \p this->m_obR.nthD [NEW => END[ and then merge with old
     * part this->m_obR.nthD [0 => NEW[.
     */
    void sort_merge();
protected:
    void add_check(Ob * ob);

    /*! Unregister \p ob.     */
    void unreg(Ob * ob);

    /*! Remove Ob \p ob.     */
    int eraseS(Ob * ob);

    /// Remove all \em Obseleted Objects.
    size_t erase_obselete();
    void eraseAll();

protected:
    void init();

    void tagAll();
    void untagAll();

protected:
    /*! Process External System Events.
     * \param timeout_ms \em timeout in \em milliseconds if >= 0, or infinite if negative < 0.
     */
    int process_events( int timeout_ms = 0);

public:
    std::unordered_set<Ob*> m_obR;  ///< \em Registered Objects.
    std::unordered_set<Ob*> m_obN;  ///< \em New (\em Unprocessed) or Un-Modified Objects.
protected:
    filesystem::Dir* m_rootFS;             ///< File System Tree Root Object.
    filesystem::APTDir* m_rootAPT; ///< APT Tree Root Object. \todo Decouple this from \c Reg. There may be lots more of these kinds of hub objects.

#ifdef HAVE_MAGIC_H
    magic_t m_magic_standard_cookie; ///< Magic Cookie (handle).
    magic_t m_magic_mime_cookie;     ///< Magic Cookie (handle).
#endif

public:
    /*! \em Tag Hash Table: Contains all objects that have been tagged. */
    xHash m_tags;

    /*! \em Obseletion Hash Table: Contains all objects that have been
     *      obseleted and should be deleted, when there is idles
     *      time. */
    xHash m_obs_hash;

    xHash m_lock_hash;          ///< \em Locked.

protected:
    chash::chashid m_default_hid;      ///< Content Hash Id Type.
    size_t m_types_hist[OB_NUM]; ///< Histogram of types in Registry.
    const uint8_t *m_rmagic;  ///< Registry Magic of length \c PREG_RMAGIC_SIZE.
};

/* ---------------------------- Group Separator ---------------------------- */

/*! Global Pointer \em Singleton Registry \em Instance, assuming \em one \c pReg
 *  instance per process for now. */
extern pReg * g_oreg;

}
