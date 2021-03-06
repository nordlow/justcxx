#include <wchar.h>
#include <sys/mman.h>

#include "patt.hpp"
#include "rels.hpp"
#include "../udunits.hpp"
#include "know_dfmts.hpp"
#include "regfile.hpp"
#include "lit.hpp"
#include "alt.hpp"
#include "seq.hpp"
#include "../mean.h"
#include "../stdio_x.h"
#include "pmatchers.hpp"
#include "../sadd.h"
#include "../smul.h"
#include "../rangerand.h"
#include "../memory_x.hpp"
#include "../algorithm_x.hpp"

#include <ostream>
#include <iomanip>
#include <iostream>
#include <set>
#include <algorithm>

namespace semnet {
namespace patterns {

Base::Base() : m_cbuf_lmatchP(nullptr) {
    init();
}

Base::Base(const char * name) : m_cbuf_lmatchP(nullptr),
                                m_nameP(name) {
    init();
}

Base::Base(pHit (*cbuf_lmatchP)(const char * pbuf, size_t len,
                                bix off, pMode pmm))
    : m_cbuf_lmatchP(cbuf_lmatchP)
{
    init();
}

Base::~Base() {
    unprepare();
}
void Base::init() {
    m_dfmt = DFMT_any_;
}

int countSubDups(const Base * pA, const Base * pB)
{
    int ret = 0;
    PTODO("Sort subsA and subsB on type and then checksum to find out number of sub clones.\n");
    // for (auto& it : subsA) {
    // }
    // for (auto& it : subsB) {
    // }
    return ret;
}

pHit Base::fixed_rep_lmatch_in(size_t rep,
                               const char* buf, size_t len,
                               bir roi) const {
    pHit tot(roi.low(), false);                  // total repeat hit
    for (size_t r = 0; r < rep; r++) { /* for each \em required repetition */
        if (roi.low() >= to_bit(len)) { break; }  /* if no more data break */
        if (auto hit = match_in(buf,len, roi)) {
            roi += hit.bitlength(); /* advance roi */
            tot.include(hit); /* include in total match */
        } else {
            break;
        }
    }
    return tot;
}

Base::Skips8& Base::load_skips8() const {
    if (not m_skips8) {
        (m_skips8 = std::make_unique<Skips8>())->set(); // default to all skips
    }
    return *m_skips8;
}
void Base::unload_skips8() const
{
    m_skips8.reset();
}

void Base::prepare() const {
}
void Base::unprepare() const {
    m_ctdig.reset();
    unload_skips8();
}

pHit Base::match_name_of(const semnet::filesystem::File * file) const {
    return match_at(file->get_pathL());
}
pHit Base::match_path_of(const semnet::filesystem::File * file) const {
    return match_at(file->path());
}

pHit
Base::match_contents_of(int fd, off_t fsize, bir roi) const
{
    if (fsize == std::numeric_limits<off_t>::max()) { // if size not yet read
        fsize = fd_get_fsize(fd);                     // read size
    }
    auto off = roi.low();
    auto s_bisz = sample_range().high(); // greedy
    const bix f_bisz = std::min<bix>(std::min<bix>(off + s_bisz,
                                                  roi.bitsize()),
                                    to_bit(fsize)); // file bitsize
    const size_t f_bysz = to_byte(f_bisz); // file bytesize
    void* dat = nullptr; // file contents data pointer
#ifdef HAVE_MMAP
    const bix p_bisz = to_bit(sysconf(_SC_PAGE_SIZE)); // page bit size
    const bix offQ = off / p_bisz;            // offset in number of pages
    const bix offR = off % p_bisz;            // in-page bit offset
    // \see http://stackoverflow.com/questions/9519648/mmap-fun-map-shared-vs-map-private
    dat = ::mmap(nullptr,          // void *addr
                 f_bysz,            // size_t length
                 PROT_READ | PROT_WRITE, // int prot
                 MAP_FILE | MAP_PRIVATE,       // int flag
                 fd,
                 to_byte(offQ) * to_byte(p_bisz) // off_t offset must be a multiple of the page size as returned by sysconf(_SC_PAGE_SIZE)
                 );
    if (dat == MAP_FAILED) { perror("mmap"); PERR("mmap()\n"); }
    auto hit = match_in(reinterpret_cast<char*>(dat), f_bysz, bir(offR, bir::highest()));
    ::munmap(dat, f_bysz);
    return hit;
#else
#  error "TODO: Use file->pread() into dat when no mmap is available."
    return match_at(reinterpret_cast<char*>(dat), f_bysz, off);
#endif
}

pHit
Base::match_contents_of(const filesystem::File * file, bir roi) const
{
    return match_contents_of(file->get_fd(), file->get_size(), roi);
}

pHit
Base::match_in(const char* buf, size_t len,
               bir roi) const
{
    pMode pmm;
    if (m_cbuf_lmatchP) {       // if specific (optimized) matcher defined
        return m_cbuf_lmatchP(buf, len, roi.low(), pmm); // use it
    }

    load_skips8();       // load skip table
    // performed optimized "skip"-search

    if (pmm.get_type() == MATCH_PARTIAL) {
        // find hit beginning
        auto high = (roi.high() == range<bix>::highest()) ? len : to_byte(roi.high());
        for (auto i = to_byte(roi.low()) ; i != high; i++) { // TODO: for (auto i : roi) {
            uint8_t bb = (reinterpret_cast<const uint8_t*>(buf))[i]; // make unsigned
            if (not (*m_skips8.get())[bb]) { /* if search hit */
                roi.low() = to_bit(i);
                break;
            }
        }
        if (roi.empty()) {
            return roi;
        }
    }

    return match_in_local(buf,len, roi); // if no match use \em generic
}

pHit
Base::match_in_special(const char* buf, size_t len,
                       bir roi) const
{
    pMode pmm;
    if (m_cbuf_lmatchP) {       // if specific (optimized) matcher defined
        return m_cbuf_lmatchP(buf, len, roi.low(), pmm); // use it
    } else {
        return match_in_local(buf,len, roi); // if no match use \em generic
    }
}

/* ---------------------------- Group Separator ---------------------------- */

void
Base::move_supers_to(Base * target)
{
    std::for_each(begin(m_supers), end(m_supers),
                  [=] (Super * super) {
                      target->add_super(super); // uplink
                  });
}

/* ---------------------------- Group Separator ---------------------------- */

bool Base::self_check(int num, bir roi, bool show_) const
{
    auto& os = std::cout;

    const range<size_t> mults(1, 10);        // variants size range to print

    /* generate instances */
    auto multV = std::min(complexity(to_bit(mults)),
                          mults.high()); // truncate
    if (show_) { os << " instances[" << multV << "]:["; }
    for (decltype(multV) i = 0; i < multV; i++) {
        const csc bufI = rand(roi);
        const size_t lenI = bufI.size();
        if (lenI >= 1) {
            auto hit = match_in(bufI, bir::full());
            if (not hit) {
                PWARN("%s failed to match its own instance ", get_pathL().c_str());
                chararray_print_humanely(reinterpret_cast<uchar*>(const_cast<char*>(bufI.data())), lenI); endline();
                hit = match_in(bufI, bir::full());
                return false;
            }
        }
        if (show_) { os << "[ "; }
        chararray_print_humanely(reinterpret_cast<uchar*>(const_cast<char*>(bufI.data())), lenI);
        if (show_) { os << "],"; }
    }
    if (show_) { os << "]"; }
    return true;
}

void
Base::show_rand(std::ostream& os) const
{
    self_check(1, sample_range(), true);
}

std::ostream &
Base::show(std::ostream& os) const
{
    if (not m_nameP.empty()) { os << " name:\"" << m_nameP << '\"'; } // name
    if (not m_langP.empty()) { os << " lang:\"" << m_langP << '\"'; } // language

    auto range = sample_range();
    range.high() = std::min<bix>(range.high(), bix(256)); /* truncate instance max */
    if (range.high() <
        range.low()) {
        PERR("range.high() (256) < range.low()\n");
    }
    os << " range:" << range;

    return os;
}

}
}
