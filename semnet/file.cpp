/* #define _XOPEN_SOURCE 500 */
#define _LARGEFILE64_SOURCE
#define _ATFILE_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.hpp"
#include "dir.hpp"
#include "patt.hpp"
#include "fileattr.hpp"
#include "regfile.hpp"
#include "rels.hpp"
#include "preg.hpp"
#include "ob.hpp"

#include "symlink.hpp"

#include "../encode.hpp"
#include "../decode.hpp"
#include "../pathops.h"
#include "../stdio_x.h"
#include "../statutils.h"
#include "../memcmpn.h"
#include "../ffmpeg_x.hpp"
#include "../memory_x.hpp"
#include "../pathops.hpp"

#include <vector>
#include <ostream>
// #include <iomanip>
// #include <iostream>
#include <boost/circular_buffer.hpp>

/* ---------------------------- Group Separator ---------------------------- */

namespace semnet { namespace filesystem {

static size_t m_opens_max = 128;  ///< Maximum number of open files allowed.
typedef boost::circular_buffer<File*> OpenRing;
static OpenRing m_open_ring(m_opens_max); // Opened files.

void File::close_oldest()
{
    if (m_open_ring.size() == m_opens_max) { // if too many open files
        File * oldest = m_open_ring.front();
        oldest->close();
        // std::cout << "closed " << oldest->path() << " ring size is now " << m_open_ring.size() << std::endl;
    }
}

/* ---------------------------- Group Separator ---------------------------- */

void File::init(FKIND_t fkind,
                const struct stat * statp)
{
    m_fkind = fkind;
    m_update_attr_flag = true;
    m_name_obs_flag = true;
    m_magic_obs_flag = true;
    if (statp) {                  // if stat is defined
        m_stat = std::make_unique<struct stat>(*statp); // copy already defined stat
    } else {
        m_stat = nullptr;
    }
    m_attrs = nullptr;
}

/* ---------------------------- Group Separator ---------------------------- */

// TODO: Disable watch, rename and active watches.
int
File::rename(const char * newpath, RENAME_t flags)
{
    int ret = -1;
#ifdef __USE_ATFILE
    const auto parent = get_parent();
    if (parent and
        parent->open() >= 0) {
        // TODO: Handle case where \c newdirfd != \c olddirfd in \c renameat():)
        if (is_absolute_path(newpath)) { // absolute newpath
            PTODO("Handle absolute newpath!\n");
        } else if (is_relative_path(newpath)) { // relative newpath
            PTODO("Handle relative newpath!\n");
        } else {                // local newname
            ret = ::renameat(parent->get_fd(), get_pathL().c_str(),
                             parent->get_fd(), newpath);
        }
    } else {
        ret = ::rename(path().c_str(), newpath);
    }
#else
    ret = ::rename(path().c_str(), newpath);
#endif
    return ret;
}

// TODO: Disable watch, rename and active watches.
int
File::remove()
{
    return ::unlink(path().c_str());
}

/* ---------------------------- Group Separator ---------------------------- */

int
File::copy_into(const char * newpath, COPY_t flags)
{
    return 0;
}

int
File::copy_into(Dir * dir, COPY_t flags)
{
    return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

#ifdef HAVE_SYS_XATTR_H
#  include <sys/xattr.h>
#endif

int
File::set_attr(const char* name, const void* value, size_t size, int flags)
{
#ifdef HAVE_SYS_XATTR_H
    if (open() >= 0) {
        auto ret = fsetxattr(get_fd(), name, value, size, flags);
        if (ret == -1) {
            switch (errno) {
            case EACCES: ret = -1; break;
            case ENOENT: ret = -1; break;
            case ENOTDIR: ret = -1; break; // NOTE: Link is unresolved so just be silent here
            default:
                std::cout << "pathL:" << get_pathL() << " name::" << std::endl << name << std::endl;
                lperror("fsetxattr()");
                break;
            }
        }
        return ret;
    }
#endif
    return -1;
}

int
File::set_attr_csc(const char * name, const csc& value, int flags)
{
    return set_attr(name, value.data(), value.size(), flags); // value doesn't have to be null terminated'
}

int
File::has_attr(const char * name) const
{
    int ret = 0;
#ifdef HAVE_SYS_XATTR_H
    if (open() >= 0) {
        void * value = nullptr; size_t size = 0;
        /* Get the attribute NAME of the file descriptor FD to VALUE (which is SIZE
           bytes long).  Return 0 on success, -1 for errors.  */
        ret = fgetxattr(get_fd(), name, value, size);
        if (ret == -1) {
            switch (errno) {
            case ENODATA: ret = 0; break; /* no attribute present (yet) */
            case ERANGE: PTODO("size to small\n"); break; /* size of buffer is to small */
            case ENOTSUP: PWARN("ENOTSUP: Not supported or disabled\n"); break;
            default:
                std::cout << "pathL:" << get_pathL() << " name:" << name << ":" << std::endl;
                lperror("fgetxattr()");
                break;
            }
        } else {			/* attribute present */
            ret = 1;
        }
    }
#endif
    return ret;
}

int
File::get_attr(const char * name, void * value, size_t size) const
{
    int ret = -1;
#ifdef HAVE_SYS_XATTR_H
    /* Get the attribute NAME of the file descriptor FD to VALUE (which is SIZE
       bytes long).  Return 0 on success, -1 for errors.  */
    if (open() >= 0) {
        ret = fgetxattr(get_fd(), name, value, size);
        if (ret == -1) {
            switch (errno) {
            case ENODATA: break; /* no attribute present (yet) */
            case ERANGE: break; /* size of buffer is to small */
            case ENOTSUP: PWARN("ENOTSUP: Not supported or disabled\n"); break;
            default: {
                char buf[64]; strerror_r(errno, buf, sizeof(buf));
                std::cout << "pathL:" << get_pathL() << " name:" << name << ": fgetxattr(): " << buf << std::endl;
                break;
            }
            }
        }
    }
#endif
    return ret;
}

const size_t g_xattr_initial_size = 4096;
const size_t g_xattr_max_size     = (1 << 20);

int
File::get_attr_csc(const char * name, csc& value_ret) const
{
    int ret = -1;

    size_t size = g_xattr_initial_size;
    char * value = reinterpret_cast<char*>(malloc(size));
    int xret = 0;
    while ((xret = get_attr(name, value, size)) == -1 and
           errno == ERANGE) {
        if (size * 2 > g_xattr_max_size) {
            PWARN("size:%zd > g_xattr_max_size:%zd\n", size, g_xattr_max_size);
            break;
        } else {
            if (size == 0) { size = 1; } else { size *= 2; }
            value = reinterpret_cast<char*>(realloc(value, size));
        }
    }

    if (xret >= 1) {
        value = reinterpret_cast<char*>(realloc(value, xret));
        value_ret = csc(value, xret); // TODO: Init string with value without copying it.
        free(value);
        ret = xret;
    } else {
        free(value);
    }

    return ret;
}

int
File::remove_attr(const csc& xan)
{
    int ret = 0;
#ifdef HAVE_SYS_XATTR_H
    if (open() >= 0) {
        ret = fremovexattr(get_fd(), xan.c_str());
        if (ret < 0) {
            lperror("fremovexattr()");
            /* \todo Handle specific error codes. */
        }
    }
#endif
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
File::update_stat() const
{
    int ret = -1;
    if (m_stat.get()) {
        ret = 1;
    } else {
        m_stat = std::make_unique<struct stat>(); // allocate new \c stat
        if (auto parent = get_parent()) {
#ifdef __USE_ATFILE
            if (parent->open() >= 0) {
                ret = fstatat(parent->get_fd(), get_pathL().c_str(), m_stat.get(), AT_SYMLINK_NOFOLLOW);
            }
#else
            ret = ::lstat(path().c_str(), m_stat.get());
#endif
        } else {
            ret = ::lstat("/", m_stat.get());
        }
        if (ret < 0) { lperror("stat()"); } /* \todo Handle specific error codes. */
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

/* Timestamp. */
const char g_timestamp_xns[] = "user.SemNet.xattr_timestamp";

File *
File::set_attr_timestamp_time(time_t t)
{
    uint64_t ts = t;            // always 64-bit
    uchar bbuf[sizeof(ts)];     // byte buffer
    benc_u64be(bbuf, &ts);
    set_attr(g_timestamp_xns, bbuf,sizeof(bbuf), 0);
    return this;
}

uint64_t
File::get_attr_timestamp() const
{
    uint64_t ret = 0;
    uchar bbuf[sizeof(ret)];    // byte buffer
    if (get_attr(g_timestamp_xns, bbuf,sizeof(bbuf)) >= 1) {
        ret = bdec_u64be(bbuf);
    }
    return ret;
}

void
File::update_attr_timestamp_time(time_t t)
{
    bool rescan_flag = false;		/* flags for update */
    rescan_flag = (has_attr(g_timestamp_xns) == 0);
    if (rescan_flag) { set_attr_timestamp_time(t); }
}

int
File::has_uptodate_attr_timestamp() const
{
    int ret = 0;
    uint64_t xattr_mtime = get_attr_timestamp(); /* xattr modification time */
    uint64_t real_mtime = 0;      /* real modification time */
    if (xattr_mtime != 0 and	/* if timestamp found and */
        xattr_mtime > (real_mtime = get_mtime())) { /* if xattrs are newer than modification timestamp */
        ret = 1;
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

#ifdef HAVE_MAGIC_H
#  include <magic.h>
#endif

#include "../chash.hpp"

/*!
 * xattr namespace for magic strings.
 */
const char g_magic_standard_xns[] = "user.semnetDB.magic";

/// MIME-type from libmagic
const char g_magic_mime_xns[] = "user.SemNet.magic.MIME";

/// Fileformat (Container and Codec) from FFMPEG.
const char g_ffmpeg_avformat_xns[] = "user.semnetDB.ffmpeg.avformat";

int
File::rescan_magic_single(PMAGIC_t pmagic, const char *xns, csc * magic_ret)
{
    int ret = 0;
#ifdef HAVE_MAGIC_H

    bool show_flag = false;
    auto pathGF = path();

    auto& os = std::cout;

    /* try load magic from xattr */
    int loadF = 0;  /* flags that we managed to loaded magic from file */
    csc mag;
    if (m_update_attr_flag) {
        int rval = get_attr_csc(xns, mag);
        if (rval >= 0) {             /* if we could read xattr */
            if (show_flag) {
                PNOTE("Loaded xattr-magic \""); os << mag << '"';
                PNOTE("from file \""); os << pathGF << "\"\n";
            }
            loadF = 1;
        }
    }

    /* try scan for magic */
    int scanF = 0;  /* flags that we managed to scan magic from file */
    if (not loadF) {
        const char * mstr = nullptr;
        if        (pmagic == PMAGIC_STANDARD) {
            mstr = magic_file(g_oreg->m_magic_standard_cookie, pathGF.c_str());
        } else if (pmagic == PMAGIC_MIME) {
            mstr = magic_file(g_oreg->m_magic_mime_cookie, pathGF.c_str());
        } else {
            mstr = magic_file(g_oreg->m_magic_standard_cookie, pathGF.c_str());
            PWARN("Cannot handle pmagic:%d. Defaulting to PMAGIC_STANDARD\n", pmagic);
        }
        if (mstr) {
            if (show_flag) {
                mag.assign(mstr); scanF = 1;
                PNOTE("Scanned magic \""); os << mag << '\"';
                PNOTE("from file \""); os << pathGF << "\"\n";
            }
        }
    }

    if (loadF or scanF) {                                /* if got magic somehow */
        if (not m_update_attr_flag) { /* if magic not loaded from disk */
            const bool wr_flag = is_writable(); /* and if writable */
            if (wr_flag) { set_attr_csc(xns, mag, 0); /* write xattr to disk */ }
        }
        ret = 1;
    } else {
        /* NOTE: This is not an error. */
    }

    if (magic_ret) { magic_ret->assign(mag); }
#endif
    return ret;
}

int
File::update_magic(PMAGIC_t pmagic)
{
    int ret = 0;
    if (m_magic_obs_flag) {
        switch (pmagic) {
        case PMAGIC_STANDARD:
            ret += rescan_magic_single(PMAGIC_STANDARD, g_magic_standard_xns, nullptr);
            break;
        case PMAGIC_MIME:
            ret += rescan_magic_single(PMAGIC_MIME, g_magic_mime_xns, nullptr);
            break;
        case PMAGIC_ALL:
            ret += rescan_magic_single(PMAGIC_STANDARD, g_magic_standard_xns, nullptr);
            ret += rescan_magic_single(PMAGIC_MIME, g_magic_mime_xns, nullptr);
            break;
        }
        m_magic_obs_flag = false;
    }
    return ret;
}

int File::sync(bool data_only_flag)
{
    int ret = 0;
    if (open() >= 0) {
        if (data_only_flag) {
            /* Note: reduce disk activity for applications that do not
               require all metadata to be synchronized with the disk. */
            ret = fdatasync(get_fd());
        } else {
            ret = fsync(get_fd());
        }
        if (ret == -1) {
            switch (errno) {
            default:
                PWARN("pathL:"); std::cout << m_name; lperror(" fsync()");
                break;
            }
        }
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
File::remove_attr_if_too_small(const csc& xan)
{
    int ret = 0;
    if (m_stat or update_stat() >= 0) {
        if (m_stat->st_size >= 0 and
            (size_t)m_stat->st_size < xan.size()) { /* if file size is smaller than hash block size */
            /* no use in using checksums so remove it instead */
            if (remove_attr(xan) == 0) {
                PTEST("Removed attribute %.*s in file %s",
                      (int)xan.size(), xan.c_str(), path().c_str());
                endline();
                ret = 1;
            }
        }
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

const File::Attrs &
File::get_attrs(bool rescan_flag)
{
    if (m_attrs) {
        if (rescan_flag) {
            m_attrs->clear();
        }
    } else {
        m_attrs = new Attrs();
    }
    if (rescan_flag) {
        /* \todo Fix problem with \c read_all_attr_names() and @c
           get_attr_xstr() and activate this function. */
#ifdef HAVE_SYS_XATTR_H
        if (open() >= 0) {
            size_t size = g_xattr_initial_size;
            char * list = static_cast<char*>(malloc(size)); // string containing all attributes
            /* find number of xattr's */
            ssize_t xret = 0;
            while ((xret = flistxattr(get_fd(), list, size)) == -1 and
                   errno == ERANGE) {
                if (size * 2 > g_xattr_max_size) {
                    PWARN("size:%zd > g_xattr_max_size:%zd", size, g_xattr_max_size);
                    break;
                } else {
                    if (size == 0) { size = 1; } else { size *= 2; }
                    list = reinterpret_cast<char*>(realloc(list, size));
                }
            }
            if (xret >= 1) {                                           // if read ok
                //const size_t xnum = std::count(list, list+size, '\0');  // count number of NULLs
                size_t iB = 0;          // name begin index
                for (size_t i = iB = 0; i < (size_t)xret; i++) {
                    if (i > iB and list[i] == '\0') {
                        csc name(&list[iB], i-iB); // note: reference string
                        csc value; get_attr_csc(name.c_str(), value);
                        m_attrs->push_back(FileAttr(name, value));
                        iB = i;     /* reset begin counter */
                    }
                }
            }
            free(list);
        }
#endif
    }
    return *m_attrs;
}

void
File::unload_attrs()
{
    if (m_attrs) { delete m_attrs; m_attrs = nullptr; }
}

/* ---------------------------- Group Separator ---------------------------- */

int
File::update_FKIND(bool force_flag)
{
    int ret = 0;
    if (force_flag or
        m_fkind == FKIND_undefined_) {
        m_fkind = FKIND_from_stat_mode(get_mode());
        ret = 1;
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int File::update()
{
    update_stat();
    /* we need current time \em before we rescan and update magic and
       hash xattr */
    const time_t tcurr = time(nullptr);
    if (m_update_attr_flag) {    /* if we want to try to read from xattr */
        /* check if xattrs are still uptodate */
        const int tstamp_ok = (has_uptodate_attr_timestamp() == 1);
        if (not tstamp_ok) {           /* if they aren't */
            m_update_attr_flag = false;      /* skip reading xattr's */
        }
    }
    const int ret = load(false, true);
    m_update_attr_flag = false;
    set_attr_timestamp_time(tcurr);
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int
File::load(bool recurse_flag, bool cscan_flag)
{
    if (recurse_flag) {}
    if (cscan_flag) {}
    get_attrs();
    return (update_magic(PMAGIC_ALL) + update_FKIND());
}

File*
File::load_path(const csc& pathF)
{
    return Dir::root()->load_to(pathF);
}

File*
File::load_path(const char * pathF)
{
    return Dir::root()->load_to(pathF);
}

/* ---------------------------- Group Separator ---------------------------- */

csc
File::path() const {
    return path_add(get_parent()->path(), get_pathL(), false);
}

int
File::open_fd(int& fd, int flags, mode_t mode) const
{
    if (fd < 0) {
        close_oldest();
        if (auto parent = get_parent()) {
            fd = parent->openat_here(get_pathL(), flags, mode);
        } else {
            fd = ::open("/", flags);
        }
        if (fd >= 0) { log_open(); }
    }
    return fd;
}

int
File::close_fd(int& fd) const
{
    int ret = 0;
    if (fd != -1) {
        log_close();
        if (::close(fd) == 0) { ret = 1; } else { ret = -1; }
        fd = -1;
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

void
File::log_open() const
{
    m_open_ring.push_back(const_cast<File*>(this));
}

void
File::log_close() const
{
    auto hit = find(m_open_ring.begin(),
                    m_open_ring.end(),
                    this);
    if (hit != m_open_ring.end()) {
        m_open_ring.erase(hit);
    } else {
        PERR("this was not found in m_open_ring");
    }
}

/* ---------------------------- Group Separator ---------------------------- */

int
File::done() const
{
    close();
    return 0;
}

void
File::unload() const
{
    m_name_obs_flag = true;
    m_magic_obs_flag = true;
    m_stat.release();
}

File::~File()
{
    close();
}

/* ---------------------------- Group Separator ---------------------------- */

std::ostream &
File::show(std::ostream& os) const
{
    Ob::show(os);
    return os << " name:\"" << m_name << '\"'
              << " tdepth:" << get_tree_depth()
              << " fkind:" << FKIND_getName(get_FKIND());
}

/* ---------------------------- Group Separator ---------------------------- */

bool File::lock() { return false; }
bool File::unlock() { return false; }

ssize_t
File::read(size_t len, char * dat, off64_t offset)
{
    size_t ret = 0;
    if (open() >= 0) {
        if (::lseek64(get_fd(), offset, SEEK_SET) != (off64_t) -1) {
            ret = ::read(get_fd(), dat, len);
        }
    }
    return ret;
}

size_t
File::get_open_count()
{
    return m_open_ring.size();
}

}}
