#include <cstring>

#include <poll.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cfloat>

#include "file.hpp"
#include "regfile.hpp"
#include "preg.hpp"
#include "REL_enum.hpp"
#include "dir.hpp"
#include "aptdir.hpp"
#include "deb.hpp"
#include "patt.hpp"
#include "know_common.hpp"
#include "alt.hpp"
#include "seq.hpp"
#include "lit.hpp"
#include "sit.hpp"
#include "ob_cmp.hpp"
#include "dup.hpp"
#include "any.hpp"

#include "pmatchers.hpp"
#include "../fkind.h"
#include "../pathops.h"
#include "../stdio_x.h"
#include "../magic_x.h"
#include "../timing.h"
#include "../phash.h"
#include "../inotify_utils.h"
#include "../statutils.h"
#include "../rangerand.h"
#include "../chash.hpp"
#include "../ffmpeg_x.hpp"
#include "../qsort_mt.h"
#include "../fdutils.h"
#include "../pathops.hpp"

#ifdef HAVE_SYS_INOTIFY_H
#  include <sys/inotify.h>
#endif

#ifdef HAVE_FREEIMAGE_H
#  include "FreeImage.h"
#endif

#include <iostream>
#include <fstream>
#include <string>

// #include <boost/iostreams/filtering_streambuf.hpp>
// #include <boost/iostreams/copy.hpp>
// #include <boost/iostreams/filter/gzip.hpp>

#include "aptdir.hpp"
#include <algorithm>
#include <iomanip>

namespace semnet {

// ---------------------------- Group Separator ----------------------------

pReg * g_oreg = nullptr;
uint8_t g_oreg_rmagic[PREG_RMAGIC_SIZE];

// ---------------------------- Group Separator ----------------------------

__attribute__ ((constructor))
static void preg_init(void)
{
    g_oreg = new pReg();
}

__attribute__ ((destructor))
static void preg_clear(void)
{
    if (g_oreg) { delete g_oreg; g_oreg = nullptr; }
}

// ---------------------------- Group Separator ----------------------------

void
pReg::init()
{
    statutils_init_uid_gid();

#ifdef HAVE_MAGIC_H
    magic_open_and_load_stubborn(&m_magic_standard_cookie,
                                 MAGIC_PRESERVE_ATIME | MAGIC_CONTINUE);
    magic_open_and_load_stubborn(&m_magic_mime_cookie,
                                 MAGIC_PRESERVE_ATIME | MAGIC_CONTINUE |
                                 MAGIC_MIME);
#endif

#ifdef HAVE_FREEIMAGE_H
    FreeImage_Initialise(0);
#endif

#ifdef HAVE_LIBAVFORMAT_AVFORMAT_H
    av_register_all();
#endif

    for (OB_t oT = OB_FIRST; oT != OB_NUM; oT = static_cast<OB_t>(oT + 1)) {
        m_types_hist[oT] = 0;
    }

    uint8array_rand(g_oreg_rmagic, PREG_RMAGIC_SIZE);
    m_rmagic = g_oreg_rmagic;
}

pReg::~pReg()
{
    eraseAll();
    if (filesystem::File::get_open_count() != 0) { PWARN("Opened Files remain\n"); }
}

// ---------------------------- Group Separator ----------------------------

void
pReg::add_check(Ob * ob)
{
#ifndef NDEBUG
    if (not ob) { PERR("ob is nullptr\n"); }
    if (m_obR.find(ob) != m_obR.end()) { PERR("ob already in m_obR\n"); }
    if (m_obN.find(ob) != m_obN.end()) { PERR("ob already in m_obN\n"); }
#endif
}

void
pReg::add1(Ob * ob)
{
    add_check(ob);
}

void
pReg::addM(Ob * const* obsA, size_t obsA_N)
{
}

// ---------------------------- Group Separator ----------------------------

void
pReg::tagAll()
{
    for (auto& ob : m_obR) { if (ob) { ob->tag(); } }
}

void
pReg::untagAll()
{
    for (auto& ob : m_obR) { if (ob) { ob->untag(); } }
}

filesystem::APTDir * pReg::get_APTRoot()
{
    if (not m_rootAPT) {
        add1(m_rootAPT = new filesystem::APTDir(csc())); // root is always empty string
    }
    return m_rootAPT;
}

// ---------------------------- Group Separator ----------------------------

URI * pReg::load_APT_URI(filesystem::APTDir * dir, const csc& pathF)
{
    URI * bottom = nullptr;          // to return
    const size_t pathF_N = pathF.size();
    if (not is_root_path(pathF)) {
        Ob* subsA[MAXPATHDEPTH];
        size_t subsN = 0;
        // go through all sub directories in pathF and create them if they don't exist
        size_t i = 0;		// path char index
        while (i < pathF_N) {
            while (i < pathF_N and pathF[i] == PATH_SEP) { i++; } // skip leading '/'
            const size_t iB = i; // index to beginnining of sub directory
            while (i < pathF_N and pathF[i] != PATH_SEP) { i++; } // find next '/'
            const size_t iE = i;   // index beyond end of sub directory
            if (iB < iE) {
                const size_t lS = iE - iB; ///< length of sub path
                bool load_flag = false; // indicates that next was loaded
                URI* next = dir->get_sub(csc(pathF.data()+iB, lS), // load-on-demand next
                                          not (i == pathF_N), &load_flag); // all but last is assumed to be directory
                if (next) {
                    if (load_flag) {
                        subsA[subsN++] = next;
                    }
                    if (not (dir = dynamic_cast<filesystem::APTDir*>(next))) { bottom = next; break; } // if bottom-most break
                } else { // if next could not be loaded
                    bottom = nullptr; break; // return nothing
                }
            }
        }
        addM(subsA, subsN);
    }
    return bottom;
}

filesystem::Deb * pReg::load_Deb(const csc& name)
{
    //::enforce(not name.empty());
    // lookup \p name, \p name_N and if it's not loaded load it like APTDir::...sub() does.
    filesystem::Deb * deb = nullptr;          // to return
    return deb;
}

// ---------------------------- Group Separator ----------------------------

void
pReg::load_apt_file_pkg(filesystem::APTDir * apt_root, const std::string& line)
{
    size_t begF = 0;                      // filename begin offset
    size_t endF = line.find_first_of(" \t", 0, 2); // filename end offset (ends with either space or tab
    size_t begP = line.find_last_of(" "); // package begin offset
    size_t endP = line.find_last_of("\n"); // package end offset

    URI * uri = load_APT_URI(apt_root, csc(line.data(), endF-begF));
    filesystem::Deb * deb = load_Deb(csc(line.data() + begP, endP - begP)); // \todo Use package hash table!
    net_connectS(REL_CHILD, uri,
                 REL_PARENT, deb);
}

// size_t string::find(char c, size_type opos);

URI *
pReg::load_apt_tree(filesystem::RegFile * apt_archive)
{
    csc pathF = apt_archive->path();
    std::ifstream is(pathF.c_str(), std::ios_base::in | std::ios_base::binary);

    // boost::iostreams::filtering_istreambuf in;
    // in.push(boost::iostreams::gzip_decompressor());
    // in.push(is);

    filesystem::APTDir * apt_root = get_APTRoot();

    net_connectS(REL_PARENT, apt_archive,  // and connect it
                 REL_CHILD, apt_root);

    std::string line;

    // find header "FILE" WS "LOCATION"
    do {
        getline(is, line);
    } while (not (is.eof() or
               line.compare(0, 4, "FILE"))); // starts with FILE

    // for each FILE and PACKAGE
    while (true) {
        std::getline(is, line);
        if (is.eof()) { break; } // exit if end of file
        else { std::cout << "Got " << line << std::endl; } // print line
        load_apt_file_pkg(apt_root, line);
    }

    return apt_root;
}

// ---------------------------- Group Separator ----------------------------

void pReg::unreg(Ob * ob)
{
    m_tags.remove(this);
    if (ob->get_type() < OB_NUM) {
        m_types_hist[ob->get_type()] -= 1; // adjust type histogram
    }
}

int
pReg::eraseS(Ob * ob)
{
    int ret = 0;
    return ret;
}

void
pReg::eraseAll()
{
}

// ---------------------------- Group Separator ----------------------------

size_t
pReg::erase_obselete()
{
    size_t ret = 0;
    return ret;
}

// ---------------------------- Group Separator ----------------------------

// \see http://jdennis.fedorapeople.org/lwatch/html/InotifyOverview.html
int
pReg::process_events(int timeout_ms)
{
    int ret = 0;
    nfds_t nfds = 0;      // one file descriptor to poll for now
    const int nfds_max = 10;
    struct pollfd ufds[nfds_max];

    // TODO: When too many fd's gets added use epoll instead.

    // prepare poll
#ifdef HAVE_SYS_INOTIFY_H
    ufds[0].fd = filesystem::Dir::get_inotify_fd();  // one to poll
    ufds[0].events = POLLIN;	// we check if we can read from it
    ufds[0].revents = 0;
    nfds++;
#endif

    // do poll
    // TODO: replace with epoll. See: http://banu.com/2011/06/how-to-use-epoll-complete-example/
    int rval = ::poll(&ufds[0], nfds, timeout_ms); // check it. when timeout \c nullptr block indefinitely.

    // handle poll return
    if (rval == 1) {
        if (ufds[0].revents& POLLIN) {

            // determine how man bytes that are pending in queue
            uint qN;
            int io_ret = ::ioctl(ufds[0].fd, FIONREAD, &qN);
            if (io_ret < 0) { perror("ioctl"); return -1; }
            else {
                PNOTE("%u bytes pending in queue\n", qN);
            }

            char buf[qN];
            int len = ::read(ufds[0].fd, buf, qN); // read number of evA
            if (len < 0) {
                if (errno == EINTR) { PNOTE("Need to reissue system call\n"); }
                else                { perror ("read"); }
                ret = -1;
            } else if (len == 0) {
                PWARN("qN=%d to small\n", qN);
            } else {                                               // evA are pending
                const ssize_t esz = sizeof(struct inotify_event);  // event size
                const int nE = len / esz; // maximum number evA possible
                struct inotify_event* ev[nE]; // event grouper array
                /* NOTE: inotify require a special way of looping because events can be of different sizes.
                 * See: http://www.linuxjournal.com/article/8478 for reference. */

                int off = 0;    // event index offset
                int ecnt = 0;   // event counter
                while (off < len) {      // as long as we have ev left
                    struct inotify_event *event = (struct inotify_event *) &buf[off];
                    off += (esz + event->len); // move index to next event
                    ev[ecnt++] = event; // store current event
                }
#ifndef NDEBUG
                if (ecnt > nE) { PERR("To many messages ecnt:%d > nE:%d!\n", ecnt, nE); }
#endif
                ret = ecnt;

                // message loop
                int i = 0;
                while (i < ecnt) { // for each message
                    if (ev[i]->mask bitand IN_MOVED_FROM) {
                        if (i+1 < ecnt and                       // one look-ahead
                            ev[i+1]->mask bitand IN_MOVED_TO and // if IN_MOVED pair
                            ev[i]->cookie == ev[i+1]->cookie) { // and same cookie
                            filesystem::Dir::inotify_move(ev[i], ev[i+1]); i += 2; continue;
                        }
                    }
                    if (i < ecnt and
                        (ev[i]->mask bitand IN_Q_OVERFLOW)) {
                        PWARN("Inotify event queue overflowed!\n"); i += 1; continue;
                    }

                    if (ev[i]->mask bitand IN_CREATE or
                        ev[i]->mask bitand IN_MOVED_TO or
                        ev[i]->mask bitand IN_DELETE or
                        ev[i]->mask bitand IN_MOVED_FROM) {

                        auto j = i+1;
                        while (j < ecnt and
                               ev[i]->mask == ev[j]->mask and // same operation
                               ev[i]->wd   == ev[j]->wd) // same working directory
                        { j++; }

                        auto gn = j - i; // group length
                        if (gn >= 2) {
                            printf("%d-grouped ", gn); inotify_print_event(ev[i]);
                            filesystem::Dir * wdir = filesystem::Dir::lookup_watch(ev[i]->wd); // lookup working dir from its watch fd
                            if (ev[i]->mask bitand IN_CREATE or
                                ev[i]->mask bitand IN_MOVED_TO) {
                                wdir->inotify_create_subs(&ev[i], gn); i += gn;
                            }
                            if (ev[i]->mask bitand IN_DELETE or
                                ev[i]->mask bitand IN_MOVED_FROM) {
                                wdir->inotify_delete_subs(&ev[i], gn); i += gn;
                            }
                        }
                    }

                    if (i < ecnt) {
                        //inotify_print_event(event);
                        filesystem::Dir::inotify_single(ev[i]); i += 1; continue;
                    }
                }
            }
        }
    } else if (rval == 0) {
        // do nothing
    } else if (rval < 0) {
        lperror("poll");
    }
    return ret;
}

void pReg::iter(int timeout_ms)
{
    process_events(timeout_ms);          // 1 ms latency
    erase_obselete();
}

// ---------------------------- Group Separator ----------------------------

std::ostream& pReg::show(std::ostream& os, size_t iB, size_t iE) const
{
    using std::endl;
    os << "- watch_count:" << filesystem::Dir::get_watch_count() << endl;
    os << "- tags.size():" << m_tags.size() << endl;
    os << "- obs_hash.size():" << m_obs_hash.size() << endl;
    os << "- open_cnt:" << filesystem::File::get_open_count() << endl;

    for (OB_t i = OB_FIRST; i != OB_NUM; i = static_cast<OB_t>(i + 1)) {
        const size_t cnt = m_types_hist[i];
        if (cnt) {
            os << "- name:" << OB_getName(i)
               << " count:" << cnt << endl;
        }
    }

    iB = std::min(iB, m_obR.size());
    if (iE == 0) {
        iE = m_obR.size();
    } else {
        iE = std::min(iE, m_obR.size());
    }

    size_t i = 0;
    for (auto& ob : m_obR) {
        os << "i:" << i;
        ob->show(os);
        os << endl;
        if (++i >= iE) { break; }
    }
    return os;
}

void
pReg::sort_merge()
{
    m_obR.insert(m_obN.begin(), m_obN.end());
    m_obN.clear();
}

}
