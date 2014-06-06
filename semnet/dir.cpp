#define _LARGEFILE64_SOURCE
#define _ATFILE_SOURCE

#ifdef HAVE_SYS_INOTIFY_H
#  include <sys/inotify.h>
#endif
#include "../inotify_utils.h"

#include <cstring>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/dir.h>
#include <fcntl.h>

#include <algorithm>
#include <iomanip>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "patt.hpp"
#include "dir.hpp"
#include "file.hpp"
#include "regfile.hpp"
#include "symlink.hpp"
#include "fileattr.hpp"
#include "preg.hpp"
#include "know_common.hpp"
#include "obmr.hpp"
#include "alt.hpp"
#include "know_dfmts.hpp"

#include "seq.hpp"

#include "../stdio_x.h"
#include "../pathops.h"
#include "../statutils.h"
#include "../str_eq.h"
#include "../memcmpn.h"
#include "../phash.h"
#include "../chash.hpp"
#include "../pathops.hpp"
#include "../saturate.hpp"
#include "../fdutils.h"
#include "../ffmpeg_x.hpp"

#include "../vcs.hpp"
#include "../memory_x.hpp"
#include "../enforce.hpp"

namespace semnet {
namespace filesystem {

int Dir::m_inotify_fd = -1;

__attribute__ ((constructor))
static void dir_init()
{
#ifdef HAVE_SYS_INOTIFY_H
#if 1
    /* \c IN_NONBLOCK: Set the \c O_NONBLOCK file status flag on the new \c open
     * \c file description.  Using this flag saves extra calls to \c fcntl(2) to
     * \c achieve the same result. */
    Dir::m_inotify_fd = inotify_init1(IN_NONBLOCK);
#else
    Dir::m_inotify_fd = inotify_init();
    unblockfd(Dir::m_inotify_fd);
#endif
#endif
}

__attribute__ ((destructor))
static void dir_clear()
{
    if (Dir::m_inotify_fd != -1) {
        close(Dir::m_inotify_fd);
    }
}

Dir::Dir()
    : File(csc(), reinterpret_cast<Dir*>(0), FKIND_DIR, nullptr)
{
    init();
}

Dir::Dir(const csc& name, Dir * dirP , struct stat * statp)
    : File(name, dirP, FKIND_DIR, statp)
{
    init();
}

void Dir::init()
{
    m_ds = nullptr;
    m_tdepth  = TDEPTH_undefined_;
    m_theight = THEIGHT_undefined_;
    m_tcsize  = TSIZE_undefined_;
#ifdef HAVE_SYS_INOTIFY_H
    m_watch_fd = -1;
#endif
    m_vctype = VCS_undefined_;
    m_vcstate = VCS_obselete;
    watch_me();
}

/* ---------------------------- Group Separator ---------------------------- */

int
Dir::open() const
{
    int ret = 0;
    if (not m_ds) {
        close_oldest();
        m_ds = opendir(path().c_str());
        if (m_ds) {
            log_open(); ret = 1;
        } else {
            fprintf(stderr, " "); lperror("open()"); ret = -1;
        }
    }
    return ret;
}

int
Dir::close() const
{
    int ret = 0;
    if (m_ds) {
        log_close();
        if (::closedir(m_ds) == 0) { ret = 1; } else { ret = -1; }
        m_ds = nullptr;
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
Dir::mkdir_sub(const csc& pathL, mode_t mode)
{
#ifdef __USE_ATFILE
    return (open() >= 0) ? ::mkdirat(get_fd(), pathL.c_str(), mode) : -1;
#else
    return ::mkdir(path_add(path(), pathL).c_str(), mode);
#endif
}

int
Dir::mkfifo_sub(const csc& pathL, mode_t mode)
{
#ifdef __USE_ATFILE
    return (open() >= 0) ? ::mkfifoat(get_fd(), pathL.c_str(), mode) : -1;
#else
    return ::mkfifo(path_add(path(), pathL).c_str(), mode);
#endif
}

int
Dir::mklink_sub(const csc& pathL, const csc& pathFT, mode_t mode)
{
#ifdef __USE_ATFILE
    if (is_absolute_path(pathFT)) { // absolute
        PTODO("Handle absolute path.\n"); return -1;
    } else if (is_relative_path(pathFT)) { // relative
        PTODO("Handle relative path.\n"); return -1;
    } else {                    // local
        return (open() >= 0) ? ::linkat(get_fd(), pathFT.c_str(), get_fd(), pathL.c_str(), mode) : -1;
    }
#else
    auto pathF = path();
    return ::link(path_add(pathF, pathFT).c_str(), path_add(pathF, pathL).c_str(), mode);
#endif
}

/* ---------------------------- Group Separator ---------------------------- */

int
Dir::open_sub(const csc& pathL, int flags, mode_t mode)
{
#ifdef __USE_ATFILE
    return (open() >= 0) ? ::openat(get_fd(), pathL.c_str(), flags, mode) : -1;
#else
    return ::open(path_add(path(), pathL).c_str(), flags, mode);
#endif
}

int
Dir::remove_sub(const csc& pathL, bool dir_flag)
{
#ifdef __USE_ATFILE
    return (open() >= 0) ? ::unlinkat(get_fd(), pathL.c_str(), dir_flag ? AT_REMOVEDIR : 0) : -1;
#else
    return dir_flag ?
        ::rmdir(path_add(path(), pathL).c_str()) :
        ::unlink(path_add(path(), pathL).c_str());
#endif
}

/* ---------------------------- Group Separator ---------------------------- */

Dir::~Dir()
{
    unwatch_me();
    unload();
}

int
Dir::remove()
{
    bool direct_flag = false;   // if we disable inotify watch before deleting
    for (auto it : m_subs) {
        File* sub = it.second;
        sub->remove();
    }
    unload();
    int ret = ::rmdir(path().c_str());
    if (direct_flag) {
        if (ret == 0) { delete this; }
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

void
Dir::unload_tdig()
{
    m_tdig.reset();
}

void
Dir::unload_subs_stat()
{
    unload_tdig();
    m_tcsize = TSIZE_undefined_;
    m_theight = THEIGHT_undefined_;
    update_VCstate();
}

/* ---------------------------- Group Separator ---------------------------- */

int
Dir::openat_here(const csc& nameS, int flags, mode_t mode) const
{
    int fd = -1;
#ifdef __USE_ATFILE
    if (open() >= 0) {
        fd = ::openat(get_fd(), nameS.c_str(), flags);
        if (fd < 0) {
            PWARN("parent:%s pathL:%s openat:%s\n", path().c_str(), nameS.c_str(), strerror(errno));
        }
    } else {
        PWARN("open() failed");
    }
#else
    fd = ::open(path_add(path(), nameS).c_str(), flags);
#endif
    return fd;
}

File *
Dir::new_sub(const csc& pathP, const csc& nameS)
{
    if (pathP.empty()) {}

    /* Note: We need stat to find out type. This statbuf is then
     * used in constructors of \c File, \c Dir. */
    struct stat statbuf;

    /* create dir/file from \c lstat()-output */

    using std::cout;
    using std::setw;
    using std::endl;

    const auto pathFS = path_add(pathP, nameS); // full sub path
#ifdef __USE_ATFILE
    int stat_ret = -1;
    if (open() >= 0) {
        stat_ret = ::fstatat(get_fd(), nameS.c_str(), &statbuf, AT_SYMLINK_NOFOLLOW);
        // cout << "fstatat: stat_ret " << stat_ret << " statbuf.st_mode: " << std::hex << statbuf.st_mode << std::dec << endl;
    }
#else
    const auto stat_ret = ::lstat(pathFS.c_str(), &statbuf);
#endif
    if (stat_ret < 0) { // file status (always of file itself if link)
        PTODO("GUI-Query user that %s could not load sub %s. Notify of reason: missing or permissions\n", path().c_str(), nameS.c_str());
    }

    File* sub = nullptr;          // sub-file/directory
    if (stat_ret >= 0) { // file status (always of file itself if link)
        /* \NOTE All devices on a system are either character special files
           or block special files */
        using namespace patterns;
        if      (S_ISREG(statbuf.st_mode))  { sub = new RegFile(openat_here(nameS), nameS, this, DFMT_any_, &statbuf); }
        else if (S_ISDIR(statbuf.st_mode))  { sub = new Dir (nameS, this, &statbuf); }
        else if (S_ISCHR(statbuf.st_mode))  { sub = new File(nameS, this, FKIND_SPECIAL_CHR, &statbuf); }
        else if (S_ISBLK(statbuf.st_mode))  { sub = new File(nameS, this, FKIND_SPECIAL_BLK, &statbuf); }
        else if (S_ISFIFO(statbuf.st_mode)) { sub = new File(nameS, this, FKIND_SPECIAL_FIFO, &statbuf); }
#ifdef S_ISLNK
        // \todo Use \c CreateSymbolicLink if WIN32 is defined.
        else if (S_ISLNK(statbuf.st_mode))  { sub = new SymLink(nameS, this, &statbuf); }
#endif
#ifdef S_ISSOCK
        else if (S_ISSOCK(statbuf.st_mode)) { sub = new File(nameS, this, FKIND_SPECIAL_SOCK, &statbuf); }
#endif
        else {
            PWARN("Dir %s sub %s has unknown type\n", path().c_str(), nameS.c_str());
            sub = new File(nameS, this, FKIND_undefined_, &statbuf);
        }
    }
    return sub;
}

File *
Dir::load_sub(const csc& pathP, const csc& nameS, bool dir_flag)
{
    if (dir_flag) {}
    File* sub = new_sub(pathP, nameS);
    if (sub) {
#ifndef NDEBUG
        auto hit = m_subs.find(nameS);
        if (hit != m_subs.end()) {
            PWARN("%s already contains %s\n", pathP.c_str(), nameS.c_str());
        }
        else { m_subs.emplace(nameS, sub); }
#else
        m_subs.emplace(nameS, sub);
#endif
        unload_subs_stat();
    }
    return sub;
}

File *
Dir::load_sub(const csc& pathP, const char * nameS, size_t nameS_N, bool dir_flag)
{
    return load_sub(pathP, csc(nameS, nameS_N), dir_flag);
}


/* ---------------------------- Group Separator ---------------------------- */

File*
Dir::load_to(const csc& pathR)
{
    File * last = nullptr;   // last defined file
    const size_t pathF_N = pathR.size();
    Dir * dir = this;
    if (is_root_path(pathR)) {
        last = dir;
    } else {
        // Ob* subsA[MAXPATHDEPTH];
        // size_t subsN = 0;
        // go through all sub directories in pathR and create them if they don't exist
        size_t i = 0;    // path char index
        while (i < pathF_N) {
            while (i < pathF_N and pathR[i] == PATH_SEP) { i++; } // skip leading '/'
            const size_t iB = i; // index to beginnining of sub directory

            while (i < pathF_N and pathR[i] != PATH_SEP) { i++; } // find next '/'
            const size_t iE = i;   // index beyond end of sub directory

            if (iB < iE) {
                const size_t lS = iE - iB; // length of sub path
                if (not (lS == 1 and pathR[iB] == '.')) {   // skip self reference
                    bool load_flag = false; // indicates that next was loaded
                    File * next = nullptr;
                    if (lS == 2 and pathR[iB] == '.' and pathR[iB+1] == '.') { // if relative path is ".."
                        auto parent = dir->get_parent();
                        next = parent ? parent : dir; // either up or stay at root
                    } else {
                        next = dir->get_sub(csc(pathR.data() + iB, lS), false, &load_flag);
                    }
                    if (next) {
                        // if (load_flag) { subsA[subsN++] = next; } // if new add it
                        last = next;
                        if (not (dir = dynamic_cast<Dir*>(next))) { // if last-most file
                            break;
                        }
                    } else {        // if next could not be loaded
                        last = nullptr; break; // return nothing
                    }
                }
            }
        }
        //addM(subsA, subsN);
    }
    return last;
}

/* ---------------------------- Group Separator ---------------------------- */

VCS_t
Dir::update_VCstate(const REL_t* rM, size_t rM_N,
                    const OB_t* oM, size_t oM_N)
{
    /* check if any of the sub-directory names is a vc-directory */
    Obs hits = net_find_rMoM(rM,rM_N, oM,oM_N);
    for (size_t i = 0; i < hits.size(); i++) {
        Ob *sub = hits[i];
        auto pathL = sub->get_pathL();
        auto hit = patterns::gp_VCSDirName->match_at(pathL);
        if (hit) { /* if path match VC-DirName Pattern */
            std::vector<VCS_STATUS_t> status_list;
            vcs_status(status_list, pathL.c_str());
            break;
        }
    }
    return m_vctype;
}

int
Dir::update_VCS_state_from_sub_creation()
{
    auto hit = patterns::gp_VCSDirName->match_name_of(this);
    if (hit) {
        std::vector<VCS_STATUS_t> status_list;
        vcs_status(status_list, get_pathL().c_str());
    }
    return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

tdepth_t
Dir::get_tree_depth() const
{
    if (m_tdepth == TDEPTH_undefined_) {
        // TODO: Use like \c get_tree_depth().
        m_tdepth = 0;
    }
    return m_tdepth;              // use cache
}

theight_t
Dir::get_tree_height() const
{
    if (m_theight == THEIGHT_undefined_) {
        m_theight = m_subs.size() >= 1 ? 1 : THEIGHT_MIN;
        for (auto it : m_subs) {
            if (auto sub = dynamic_cast<Dir*>(it.second)) { // only directories
                m_theight = std::max(m_theight,
                                     sub->get_tree_height());
            }
        }
    }
    return m_theight;           // use cache
}

tsize_t
Dir::get_tree_csize() const
{
    if (m_tcsize == TSIZE_undefined_) {
        m_tcsize = get_size();
        for (auto it : m_subs) {
            File * fileS = it.second;
            if (auto dirS = dynamic_cast<Dir*>(fileS)) {
                m_tcsize += dirS->get_tree_csize();
            } else {
                m_tcsize += fileS->get_size();
            }
        }
    }
    return m_tcsize;            // use cache
}

/* ---------------------------- Group Separator ---------------------------- */

void
Dir::set_tree_depth(tdepth_t tdepth) const
{
    if (m_tdepth == TDEPTH_undefined_) {
        if (tdepth != TDEPTH_undefined_) { // undefined/obselete => defined
            m_tdepth = tdepth;
        }
    } else {
        if (tdepth == TDEPTH_undefined_) { // defined => undefined/obselete
            for (auto it : m_subs) {
                if (auto sub = dynamic_cast<Dir*>(it.second)) { // only directories
                    sub->set_tree_depth(TDEPTH_undefined_);
                }
            }
        }
        m_tdepth = TDEPTH_undefined_;
    }
}

void
Dir::set_tree_height(theight_t theight) const
{
    if (m_theight == THEIGHT_undefined_) {
        if (theight != THEIGHT_undefined_) { // undefined/obselete => defined
            m_theight = theight;
        }
    } else {
        if (theight == THEIGHT_undefined_) { // defined => undefined/obselete
            auto parent = const_cast<Dir*>(get_parent());
            if (parent) {
                parent->set_tree_height(THEIGHT_undefined_);
            }
        }
        m_theight = THEIGHT_undefined_;
    }
}

/* ---------------------------- Group Separator ---------------------------- */

uint64_t
Dir::update_chash(chash::chashid hid) const
{
    int ret = 0;
    if (hid == chash::CHASH_undefined_) { hid = g_default_hid; }
    CHashF chash;

    using std::cout;
    /* distribute all hits build up tree digest */
    for (auto it : m_subs) {
        File* sub = it.second;
        const uchar * sub_cdig = sub->get_chash(hid);
        if (sub_cdig) {         /* if sub has content hash */
            chash.update(sub_cdig, chashid_get_digest_size(hid)); // update chash using sub-chash
        } else {
            const csc pathF = sub->path();
            PNOTE("path:"); cout << pathF;
            PTODO("sub had no chash. Use content instead?\n");
        }
    }

    if (const auto tdig = m_tdig.get()) {
        chash.final(*tdig);
    } else {
        m_tdig = std::make_unique<CDigestF>(chash);
    }

    return ret;			/* and return it */
}

const uchar*
Dir::get_chash(chash::chashid hid) const
{
    const uchar * ret = nullptr;
    if (hid == chash::CHASH_undefined_) { hid = g_default_hid; }
    switch (hid) {
    case chash::CHASH_SHA2_256: {
        Dir::update_chash(); /* \todo Update only \em after load of subs */
        ret = const_cast<const CDigestF*>(m_tdig.get())->data();
        break;
    }
    default: PTODO("Cannot handle case for hid:%d\n", hid); break;
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
Dir::countSubDups(const Dir * pB)
{
    int ret = 0;

    const REL_t rM[] = { REL_CHILD };
    const size_t rM_N = ARRAY_LENGTH(rM);

    const OB_t oM[] = { OB_FILE_, OB_FILE_ };
    const size_t oM_N = ARRAY_LENGTH(oM);

    Obs hitsA = this->net_find_rMoM(rM,rM_N, oM,oM_N);
    Obs hitsB = pB->net_find_rMoM(rM,rM_N, oM,oM_N);

    PTODO("Sort hitsA and hitsB on type and then checksum to find out number of sub clones.\n");

    for (size_t i = 0; i < hitsA.size(); i++) {
        /* Ob * subA = hitsA[i]; */
    }

    for (size_t i = 0; i < hitsB.size(); i++) {
        /* Ob * subB = hitsB[i]; */
    }

    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

Dir* Dir::m_root = nullptr;

Dir*
Dir::root()
{
    if (not m_root) { m_root = new Dir(); }
    return m_root;
}

Dir*
Dir::home(const csc * user)
{
    if (user) {
        PTODO("Handle user %s\n", user->c_str());
    }
    File* homeF = File::load_path(csc("/home/per"));
    Dir* homeD = dynamic_cast<Dir*>(homeF);
#ifndef NDEBUG
    if (not homeD) { PERR("Failed to load HOME!"); }
#endif
    return homeD;
}

Dir::Watches Dir::m_watches(4096);

/* ---------------------------- Group Separator ---------------------------- */

int
Dir::load(bool recurse_flag, bool cscan_flag)
{
    int ret = 0;
    const auto pathF = path(); /* path to this Global Parent (Directory) */
    if (not pathF.empty()) {
        const char * dir = (pathF.empty()) ? PATH_ROOT_CSTR : pathF.c_str();
        /* PWARN("scanning %s\n", dir); */
        struct dirent ** dent = nullptr;	/* points to array of directory entry pointers */
        int dent_N = scandir(dir, &dent, 0, alphasort); /* system scan directory */
        if (dent_N < 0) {
            switch (errno) {
            case EACCES: /* FIXME: Shake object? */ break;
            default: lperror("scandir()"); break;
            }
        } else if (dent_N <= 2) {
            set_tree_height(0); /* no subs => zero theight */
        } else {                      // dent_t > 2
            const int subs_N = dent_N-2;
            m_subs.rehash(subs_N); // prepare hashed storage

            //const char* keys[subs_N];      // sub keys (names). Time-Complexity: O(1).
            //size_t klens[subs_N];          // sub key length
            File * subs[subs_N];  // sub files. Time-Complexity: O(1).

            int iN = 0;         // new object counter
            int iO = 0;         // old object counter
            for (int i = 0; i < dent_N; i++) {
                const char * name = dent[i]->d_name; /* path to local child */
                const size_t name_N = _D_EXACT_NAMLEN(dent[i]); // on Linux this macro expand to (d)->d_namlen
                if (not nstr_is_DorDD_path(name, name_N)) {
                    const csc key(name, name_N);
                    File * sub = lookup_sub(key);
                    if (sub) {
                        subs[subs_N - ++iO] = sub; // previously loaded at the end
                    } else {
                        sub = new_sub(pathF, key);
                        if (sub) {            // if load was successful
                            //keys[iN] = name;
                            //klens[iN] = name_N;
                            m_subs.emplace(key, sub);
                            subs[iN] = sub; // newly loaded at beginning
                            iN++;
                        }
                    }
                }
                free(dent[i]);
            }

            // TODO: Vectorize for sorted vector container
            //for (int i = 0; i < iN; i++) {
            //PNOTE("key: %.*s\n", klens[i], keys[i]);
            //m_subs.emplace(csc(keys[i], klens[i]), subs[i]);
            //}

            if (iN) { unload_subs_stat(); }

            if (recurse_flag) {
                // load newly created sub-directories
                for (int i = 0; i < iN; i++) {
                    subs[i]->load(recurse_flag, cscan_flag);
                    if (auto subdir = dynamic_cast<Dir*>(subs[i])) {
                        subdir->update_all();
                    }
                }

                // load previously existing sub-directories
                for (int i = subs_N-iO; i < subs_N; i++) {
                    subs[i]->load(recurse_flag, cscan_flag);
                    if (auto subdir = dynamic_cast<Dir*>(subs[i])) {
                        subdir->update_all();
                    }
                }

                // set_tree_height(std::max(parr_get_tree_height_max(subs, iN),
                //                          parr_get_tree_height_max(subs+subs_N-iO, iO)) + 1);

                get_tree_csize(); // update content size
                update_all();
            }

            ret = iN;
        }
        free(dent);
    } else {
        PWARN("pathF not read\n");
    }
    return ret;
}

void Dir::unload()
{
    for (auto it : m_subs) {
        File* sub = it.second;
        sub->unload();
        delete sub;
    }
    m_subs.clear();
    File::unload();
}

/* ---------------------------- Group Separator ---------------------------- */

int Dir::idle_cb()
{
    //m_subs.rehash_coarse();
    return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

void
Dir::watch_me()
{
    // Obtaining Directory Change Notifications Windows (compare with inotify): http://msdn.microsoft.com/en-us/library/aa365261%28VS.85%29.aspx
#ifdef HAVE_SYS_INOTIFY_H
    if (m_watch_fd == -1) {
        if (is_readable()) {
            const auto pathF = path();

            // The following further bits can be specified in mask when calling \c inotify_add_watch(2):
            // IN_EXCL_UNLINK (since Linux 2.6.36)
            //                   By default, when watching events on the children of a
            //                   directory, events are generated for children even after
            //                   they have been unlinked from the directory.  This can
            //                   result in large numbers of uninteresting events for some
            //                   applications (e.g., if watching /tmp, in which many
            //                   applications create temporary files whose names are
            //                   immediately unlinked).  Specifying IN_EXCL_UNLINK
            //                   changes the default behavior, so that events are not
            //                   generated for children after they have been unlinked
            //                   from the watched directory.
            // IN_MASK_ADD       Add (OR) events to watch mask for this pathname if it
            //                   already exists (instead of replacing mask).
            // IN_ONESHOT        Monitor pathname for one event, then remove from watch
            //                   list.
            // IN_ONLYDIR (since Linux 2.6.15)
            //                   Only watch pathname if it is a directory.
            // const uint32_t mask = (IN_CLOSE_WRITE | IN_MOVE | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF);
            const uint32_t mask = (IN_CREATE | IN_MOVE | IN_MOVE_SELF | IN_DELETE | IN_DELETE_SELF |
                                   IN_DONT_FOLLOW // don't' dereference symbolic links (since Linux 2.6.15)
                                   );

            m_watch_fd = inotify_add_watch(get_inotify_fd(), pathF.c_str(), mask);
            if (m_watch_fd < 0) {
                std::cerr << "Inotify watch on pathF " << pathF << " failed\n";
            }
            else {
                m_watches[m_watch_fd] = this;
                // if (hit != this) { // if no insert took place
                //     PWARN("Dir::watch_fd already present in watch_hash for Dir %s\n", pathF.c_str());
                // }
            }
        }
    }
#endif
}

void
Dir::unwatch_me()
{
    if (m_watch_fd != -1) {
        inotify_rm_watch(get_inotify_fd(), m_watch_fd); // unregister it
        auto num = m_watches.erase(m_watch_fd);
#ifndef NDEBUG
        if (not num) { PERR("Could not erase m_watch_fd!\n"); }
#endif
        m_watch_fd = -1;
    }
}

void
Dir::unwatch_at(Watches::iterator& position)
{
    const int fd = position->first;
    inotify_rm_watch(get_inotify_fd(), fd); // unregister it from inotify
    m_watches.erase(position); // no standardized yet
    auto dir = position->second;
#ifndef NDEBUG
    if (dir->m_watch_fd == -1) { PERR("Dir fd already -1\n"); }
#endif
    dir->m_watch_fd = -1;       // tag locally in \c Dir
}

/* ---------------------------- Group Separator ---------------------------- */

void
Dir::update_all()
{
    if (auto parent = get_parent()) {
        set_tree_depth(parent->get_tree_depth() + 1);
    }
    update_VCstate();
    update_chash();
}

/* ---------------------------- Group Separator ---------------------------- */

File * Dir::lookup_sub(const csc& name)
{
    auto hit = m_subs.find(name);
    if (hit != m_subs.end()) {
        return hit->second;
    } else {
        return nullptr;
    }
    //return m_subs[name];
}

File * Dir::get_sub(const csc& name, bool dir_flag, bool * load_flag)
{
    File * file = lookup_sub(name);
    if (not file) {
        file = load_sub(path(), name, dir_flag);
        if (load_flag) { *load_flag = true; }
    }
    return file;
}

/* ---------------------------- Group Separator ---------------------------- */

#ifdef HAVE_SYS_INOTIFY_H
void
Dir::inotify_single(const struct inotify_event *event)
{
    // NOTE: http://linux.die.net/man/7/inotify especially:
    // The name field is only present when an event is returned for a file
    // inside a watched directory; it identifies the file pathname relative to
    // the watched directory. This pathname is null-terminated, and may include
    // further null bytes to align subsequent reads to a suitable address
    // boundary.

    // The len field counts all of the bytes in name, including the null bytes;
    // the length of each inotify_event structure is thus
    // sizeof(inotify_event)+len.

    auto watch = m_watches.find(event->wd); // lookup working dir from its watch fd
    if (watch != m_watches.end()) {
        auto dir = watch->second;
#ifndef NDEBUG
        if (dir->m_watch_fd != event->wd) { // if \p event is "adressed to" to \c dir
            PERR("inotify event->wd != dir->m_watch_fd!!!\n");
        }
#endif
        if (event->mask bitand IN_DELETE_SELF) {
            unwatch_at(watch);
            if (auto parent = dir->get_parent()) {
                parent->m_subs.erase(dir->get_pathL());
                parent->unload_subs_stat();
            } else {            // probably never happens
                delete dir;
                if (m_root == dir) { m_root = nullptr; }
            }
        } else if (event->mask bitand IN_MOVE_SELF) {
            if (auto parent = dir->get_parent()) {
                parent->m_subs.erase(dir->get_pathL());
                parent->unload_subs_stat();
            }
        } else {
            dir->inotify_sub(event);
        }
    } else {
        PERR("watch_hash.find() returned nullptr\n");
    }
}

// ----------------------------------------------------------------------------

void
Dir::inotify_move(const struct inotify_event *eventF,
                  const struct inotify_event *eventT)
{
    // PTODO("Moved %d:%s to %d:%s\n", eventF->wd,eventF->name, eventT->wd,eventT->name);
    auto watchF = m_watches.find(eventF->wd);
    auto watchT = m_watches.find(eventT->wd);
    //bool dirF_flag = (eventF->mask bitand IN_ISDIR);
    bool dirT_flag = (eventT->mask bitand IN_ISDIR);
    if (watchF != m_watches.end() and
        watchT != m_watches.end()) {
        Dir * parentF = watchF->second;
        Dir * parentT = watchT->second;
        File * sub = parentF->lookup_sub(eventF->name);
        if (sub) {
            // internal sub
            sub->m_name.assign(eventT->name);
            sub->m_parent = parentT; // update parent

            // from
            parentF->m_subs.erase(eventF->name);
            parentF->unload_subs_stat();

            // to
            //parentT->m_subs.insert(csc(eventT->name), sub);
            parentT->m_subs[csc(eventT->name)] = sub;
        } else {
            parentT->get_sub(csc(eventT->name), dirT_flag, nullptr); // TODO: Do we need to trigger here?
        }
        parentT->unload_subs_stat(); // always obselete to directory watch
    }
}

// ----------------------------------------------------------------------------

void
Dir::inotify_create_subs(struct inotify_event * const events[], int events_num)
{
    std::pair<csc, File*> subs[events_num];    // whole pair is needed
    for (int i = 0; i < events_num; i++) {
        const csc nameS(events[i]->name); // \c event subject (sub) name
        subs[i] = std::make_pair(nameS, new_sub(path(), nameS));
    }
    m_subs.insert(subs, subs+events_num); // TODO: Replace with emplace?
    unload_subs_stat();
}

void
Dir::inotify_delete_subs(struct inotify_event * const events[], int events_num)
{
    for (int i = 0; i < events_num; i++) {
        auto hit = m_subs.find(events[i]->name);
        if (hit != m_subs.end()) {
            m_subs.erase(hit);
            delete hit->second;
        }
    }
    unload_subs_stat();
}

void
Dir::inotify_sub(const struct inotify_event * event)
{
    /* The name field is only present when an event is returned for a file
     * inside a watched directory; it identifies the file pathname relative to
     * the watched directory. This pathname is null-terminated, and may include
     * further null bytes to align subsequent reads to a suitable address
     * boundary.  */

    const csc nameS(event->name);                        // \c event subject (sub) name
    const bool dir_flag = (event->mask bitand IN_ISDIR); // subject of \c event is a directory

    if (event->mask bitand IN_CREATE or   // sub was create at parent
        event->mask bitand IN_MOVED_TO) { // sub moved to parent
        load_sub(path(), nameS, dir_flag);
    }
    else if (event->mask bitand IN_MOVED_FROM or
             event->mask bitand IN_DELETE) {
        auto hit = m_subs.find(nameS);
        if (hit != m_subs.end()) {
            m_subs.erase(hit);
            delete hit->second;
            unload_subs_stat();
        }
    }
    else if (event->mask bitand IN_MODIFY) { // sub content changed
        File * old_sub = get_sub(nameS, dir_flag, nullptr);
        const auto old_size = old_sub->get_size();
        File * sub = load_sub(path(), nameS, dir_flag);
        sub->unload();
        unload_tdig();
        if (old_sub) {
            m_tcsize += sub->get_size() - old_size;
        } else {
            m_tcsize = TSIZE_undefined_;
        }
        // g_mbox->post_aop(vaop_newS_WOBBLE(this, 0, 255, ATRANS_SINSHAKE, ARELAY_RESTART, 0, g_touch_tdur, OP_NONE));
    }
    else if (event->mask bitand IN_ATTRIB) { // sub attribute changed
        File * sub = lookup_sub(nameS);
        if (sub) { sub->unload_attrs(); }
    }
    else if (event->mask bitand IN_MOVE_SELF) { // self was moved
    }
    else if (event->mask bitand IN_DELETE_SELF) { // self was deleted
    }
    else if (event->mask bitand IN_CLOSE_WRITE) { // sub was closed
    }
    else if (event->mask bitand IN_CLOSE_NOWRITE) { // sub was closed
    }
    else if (event->mask bitand IN_CLOSE) { // sub was closed
    }
    else if (event->mask bitand IN_ACCESS) { // sub was accessed
        // sub->ext_accessed_cb();
        // TODO: vFile: IN_ACCESS Animation: Define and use VANIM_IOSPIRAL.\n
        // auto ext = outer_box();
        // auto cenS = ext.cen();
        // auto dim = ext.dim();
        // float minside = float_min(dim.x, dim.y);
        // vec2f cenD; vec2f_sums(&cenS, 0.1 * minside, &cenD);
        // g_mbox->post_aop(vaop_newS_CEN(this, &cenS, &cenD,
        //                                ATRANS_COSBUMP, ARELAY_RESTART,
        //                                0, g_touch_tdur, OP_NONE));
    }
    // watch was removed explicitly (by inotify_rm_watch(2)) or automatically (file was deleted, or file system was unmounted).
    else if (event->mask bitand IN_IGNORED) {
    }

    m_vcstate = VCS_obselete;
}

#endif

/* ---------------------------- Group Separator ---------------------------- */

std::ostream& Dir::show(std::ostream& os) const
{
    File::show(os);
    os << " theight:" << get_tree_height()
       << " tcsize:" << get_tree_csize();
    os << " tdig:";
    const uchar * tdig = get_chash();
    if (tdig) {
        chararray_fprint_hex(stdout, tdig, m_tdig.get()->size());
    } else {
        os << "none";
    }
    return os;
}

csc
Dir::path_local(bool typed_flag) const
{
    const size_t cap = MAXPATHLEN; // capacity: maxlength of path
    char pathF[cap+1];              // capacity plus nullptr
    size_t off = cap;              // offset initially at end
    pathF[off] = '\0';              // last nil-terminator

    auto pathL = get_pathL();
    off -= pathL.size(); memcpy(pathF+off, pathL.data(), pathL.size());

    /* scan in direction given by \p prel
     * if prel is REL_PARENT => pathF should grow backwards in memory */
    if (const auto * curr = get_parent()) {
        while (curr) {
            off -= 1; pathF[off] = PATH_SEP;
            auto curr_pathL = curr->get_pathL();
            off -= curr_pathL.size(); memcpy(pathF+off, curr_pathL.data(), curr_pathL.size());
            curr = curr->get_parent(); // recurse upwards
        }
    } else {
        off -= 1; pathF[off] = PATH_SEP;
    }

    if (typed_flag) {           // if we want typed pathF
        size_t tprefix_len = 0;
        const char * tprefix = get_typename(&tprefix_len);
        if (tprefix) {
            off -= 1; pathF[off] = ':'; // type prefix separator
            off -= tprefix_len; memcpy(pathF+off, tprefix, tprefix_len);
        }
    }

    return csc(pathF+off, cap-off);
}

}
}
