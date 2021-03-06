#define _LARGEFILE64_SOURCE
#define _ATFILE_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <cstring>
#include <time.h>
#include <fcntl.h>
#include <iomanip>
#include "symlink.hpp"
#include "ob.hpp"
#include "dir.hpp"
#include "../pathops.hpp"
#include "../memory_x.hpp"

namespace semnet {
namespace filesystem {

SymLink::Sources SymLink::m_sources;

SymLink::~SymLink()
{
    erase_self_ref();
}

void
SymLink::erase_self_ref()
{
    if (m_dest) {
        auto hit = m_sources.find(m_dest);
        if (hit != m_sources.end()) {
            hit->second.erase(this);
        }
    }
}

void
SymLink::insert_self_ref()
{
    std::cout << __FUNCTION__ << ": " << path() << " => " << m_dest << std::endl;
    if (m_dest) {
        auto hit = m_sources.find(m_dest);
        if (hit != m_sources.end()) { // if other links already points to m_dest
            hit->second.insert(this); // add to them
        } else {                      // otherwise
            SymLink* thisC = this;   // need an lvalue
            m_sources.emplace(m_dest, SymLinks((&thisC),
                                               (&thisC)+1));
        }
    }
}

SymLinks *
SymLink::get_links_to(semnet::filesystem::File * dest)
{
    auto hit = m_sources.find(dest);
    if (hit != m_sources.end()) {
        return &hit->second;
    } else {
        return nullptr;
    }
}

void
SymLink::init(int fd, const struct stat * statp)
{
    //std::cout << __FUNCTION__ << ": " << path() << " => " << m_dest << std::endl;
    semnet::filesystem::File::init(FKIND_undefined_, statp);
    m_fd = fd;
}

ssize_t
SymLink::read_destination(char *destB, char destN) const
{
    ssize_t ssize = -1;
#ifdef __USE_ATFILE
    const auto parent = get_parent();
    if (parent and
        parent->open() >= 0) {
        ssize = readlinkat(parent->get_fd(), get_pathL().c_str(), destB, destN);
    }
#else
    auto srcF = path();     // full source path
    ssize = readlink(srcF.c_str(), destB, destN);
#endif
    return ssize;
}

semnet::filesystem::File*
SymLink::follow(bool all_flag)
{
    if (m_dest) {               // if already defined
        return m_dest;          // just return it
    }

    size_t destN = MAXPATHLEN;   // target name length
    char   destB[MAXPATHLEN+1];  // target name buffer
    auto ssize = read_destination(destB, destN);

    if (ssize >= 1) {
        destN = ssize; destB[destN] = '\0';   // set path string size and terminator.
        csc destS(destB// boost::cref(destB)
                  , destN); // target path string

        if (is_absolute_path(destS)) { // absolute path
            m_dest = semnet::filesystem::File::load_path(destS);
        } else if (is_relative_path(destS)) { // relative path, that contains ..
            if (auto parent = get_parent()) {
                if        (destS.size() == 1 and destS[0] == '.') { // TODO: functionize
                    m_dest = parent;
                } else if (destS.size() == 2 and destS[0] == '.' and destS[1] == '.') { // TODO: functionize
                    if (auto parpar = parent->get_parent()) {
                        parent = parpar;
                    } else {
                        return m_dest; // unresolved parent. TODO: Show in DVis.
                    }
                    m_dest = parent;
                } else {
                    // chop off "../'" and walk up parent
                    const csc beg("../"); const size_t begsz = beg.size();
                    size_t i;                         // offset index
                    for (i = 0;
                         destN-i >= begsz and
                             ::cbuf_begins(destS.data()+i, destN-i,
                                           beg.c_str(), begsz);
                         i += begsz) {
                        if (auto parpar = parent->get_parent()) {
                            parent = parpar;
                        } else {
                            return m_dest; // unresolved parent. TODO: Show in DVis.
                        }
                    }

                    const csc pathP  = parent->path(); // parent path
                    const csc pathGL = path_add(pathP, csc(destS.data()+i, // boost::cref(destS.data()+i),
                                                           destN-i)); // global link
                    m_dest = semnet::filesystem::File::load_path(pathGL);
                    if (not m_dest) {
                        // TODO: Tag and group as missing link and show in DVis.
                        // std::cerr << __FUNCTION__
                        //           << ": source:" << path()
                        //           << " target:" << pathGL << " not found" << std::endl;
                    }
                    //std::cout << "Resolved " << srcF << " to " << pathGL << " at " << pathP << std::endl;
                }
            } else {
                return m_dest;   // unresolved parent. TODO: Show in DVis.
            }
        } else {                                   // local path
            auto pathP  = get_pathP();             // parent path
            auto pathGL = path_add(pathP, destS);  // global link
            m_dest = semnet::filesystem::File::load_path(pathGL);
        }

        if (m_dest) {
            insert_self_ref();
        }
    } else if (ssize == 0) {
        PWARN("ssize == 0\n");
    } else if (ssize == -1) {
        lperror("readlink()");
    }

    return m_dest;
}

const uchar *
SymLink::get_chash(chash::chashid hid) const
{
    size_t destN = MAXPATHLEN;   // target name length
    char   destB[MAXPATHLEN+1];  // target name buffer
    auto ssize = read_destination(destB, destN);
    if (ssize >= 1) {
        destN = ssize; destB[destN] = '\0';   // set path string size and terminator.
    }

    CHashF chash(reinterpret_cast<uint8_t*>(destB), destN); // hash it
    if (const auto dig = m_cdig.get()) {
        chash.final(*dig);
    } else {
        m_cdig = std::make_unique<CDigestF>(chash);
    }
    return const_cast<const CDigestF*>(m_cdig.get())->data();
}

int
SymLink::load(bool recurse_flag, bool cscan_flag)
{
    return (semnet::filesystem::File::load(recurse_flag) + (cscan_flag ? (get_chash() != nullptr) : 0));
}

void
SymLink::unload() const
{
    const_cast<SymLink*>(this)->erase_self_ref(); m_dest = nullptr; // be lazy for now
    m_cdig.reset();
    semnet::filesystem::File::unload();
}

}
}
