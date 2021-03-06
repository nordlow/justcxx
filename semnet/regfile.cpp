/* #define _XOPEN_SOURCE 500 */
/* #include <unistd.h> */

#include <cstring>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "regfile.hpp"
#include "dir.hpp"
#include "patt.hpp"
#include "alt.hpp"
#include "fileattr.hpp"
#include "know_dfmts.hpp"
#include "filetype.hpp"

#include "../pathops.h"
#include "../stdio_x.h"
#include "../statutils.h"
#include "../memcmpn.h"
#include "../workpool.hpp"
#include "../ffmpeg_x.hpp"
#include "../histogram.hpp"

#ifdef HAVE_MAGIC_H
#  include <magic.h>
#endif

#ifdef HAVE_OPENCV_CV_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#endif

#include "../chash.hpp"
#include <ostream>
#include <iostream>
#include <algorithm>
#include "../memory_x.hpp"
#include "../opencv_x.hpp"
#include "../show.hpp"

namespace semnet {
namespace filesystem {

void RegFile::init(int fd, DFMT_t dfmt, const struct stat * statp)
{
    File::init(FKIND_undefined_, statp);
    m_fd = fd;
}

/* ---------------------------- Group Separator ---------------------------- */

// \todo Use SSE operations PEXTRW and PINSRW to optimized these histogram
// calculations.  See: http://www.tommesani.com/SSEPrimer.html for details.
//
// Table look-up operations, such as the ones found in histogram-related
// algorithms, have always been critical to the performance of digital
// imaging. In such cases, each of the computed R, G, and B values is used as an
// index into its respective color look-up table. Such operations have been
// difficult to implement in MMX technology due to the fact that the computed
// RGBA values would be residing in an MMX register, which cannot be used
// directly for addressing. Extracting each of them into the appropriate
// registers for addressing, fetching the contents from the table, and inserting
// them back into MMX registers was cumbersome and harmful to performance. SSE
// includes a pair of instructions (PEXTRW and PINSRW) that help enhance the
// performance of such algorithms.

void RegFile::assert_hist8()
{
    if (not m_chist8.get()) { m_chist8 = std::make_unique<CHist8>(); }
}

void RegFile::reset_hist8()
{
    if (m_chist8.get()) { m_chist8.get()->reset(); }
}

void RegFile::inc_hist8(uchar x)
{
    assert_hist8();
    m_chist8.get()->add(x);           /* increment histogram bucket */
}

void
RegFile::process_byte(const uchar* a, off_t off8)
{
    inc_hist8(a[off8]);
}

void
RegFile::process_quad(const uchar* a, off_t qoff)
{
    inc_hist8(a[qoff*4+0]);
    inc_hist8(a[qoff*4+1]);
    inc_hist8(a[qoff*4+2]);
    inc_hist8(a[qoff*4+3]);
}

void
RegFile::process_oct(const uchar* a, off_t ooff)
{
    inc_hist8(a[ooff*8+0]);
    inc_hist8(a[ooff*8+1]);
    inc_hist8(a[ooff*8+2]);
    inc_hist8(a[ooff*8+3]);
    inc_hist8(a[ooff*8+4]);
    inc_hist8(a[ooff*8+5]);
    inc_hist8(a[ooff*8+6]);
    inc_hist8(a[ooff*8+7]);
}

void
RegFile::process_hex(const uchar* a, off_t qoff)
{
    inc_hist8(a[qoff*16+0]);
    inc_hist8(a[qoff*16+1]);
    inc_hist8(a[qoff*16+2]);
    inc_hist8(a[qoff*16+3]);
    inc_hist8(a[qoff*16+4]);
    inc_hist8(a[qoff*16+5]);
    inc_hist8(a[qoff*16+6]);
    inc_hist8(a[qoff*16+7]);
    inc_hist8(a[qoff*16+8]);
    inc_hist8(a[qoff*16+9]);
    inc_hist8(a[qoff*16+10]);
    inc_hist8(a[qoff*16+11]);
    inc_hist8(a[qoff*16+12]);
    inc_hist8(a[qoff*16+13]);
    inc_hist8(a[qoff*16+14]);
    inc_hist8(a[qoff*16+15]);
}

void
RegFile::process_block(const uchar * bbuf, uint bsize)
{
    uint h = 0;                 // hex counter
    while (h < bsize % 16) {    // for each whole hex
        process_hex(&bbuf[h], 1);
        h++;
    }

    uint b = h * 16;            // byte counter
    while (b < bsize) {         // for each remaining byte
        process_byte(&bbuf[b], 1);
        b++;
    }
}

// void chash_print(chashid hid, const uchar * hdig)
// {
//     PNOTE("Got ");
//     printf("%s=", chash_get_hash_name_static(hid));
//     chararray_fprint_hex(stdout, hdig, chashid_get_digest_size(hid));
//     endline();
// }

/*!
 * xattr namespace for Hash Digests (using chash).
 */
const char g_chash_xns[] = "user.semnetDB.chash.";

/*! Build a complete xattr name from namespace \p xns and chash name of \p hid
 * and return it. */
csc
xns_build_chashid_name(chash::chashid hid, const csc& xns)
{
    return xns.c_str() + csc(boost::cref(chashid_get_name_static(hid)));
}

/* ---------------------------- Group Separator ---------------------------- */

int
RegFile::load(bool recurse_flag, bool cscan_flag)
{
    auto ret = (File::load(recurse_flag) + (cscan_flag ? cscan() + load_types() : 0));
    return ret;
}

void
RegFile::unload() const
{
    m_fkind = FKIND_undefined_; // directly tag as \em undefined
    m_cdig.reset();
    m_chist8.reset();
    File::unload();
}

int
RegFile::cscan()
{
    std::cout << "Scanning " << path() << std::endl;
    int ret = 0;

    bool show_flag = false;

    const csc p_xns(boost::cref(g_chash_xns)); // container \c g_chash_xns

    auto hid = chash::CHASH_undefined_;
    if (hid == chash::CHASH_undefined_) { hid = g_default_hid; }
    const size_t hsize = chashid_get_digest_size(hid);

    bool needF = true; /* flags whether file is big enough to require hash */

    const csc xan = xns_build_chashid_name(hid, p_xns); /* full xattr name into \c xan */

    bool loadF = false;           /* flags that we managed to load hash from file */
    if (m_update_attr_flag) {
        uchar cdig_data[hsize];
        loadF = (get_attr(xan.c_str(), cdig_data, hsize) == (int)hsize);
        if (loadF and
            hsize == SHA1_DIGEST_SIZE) {
            if (not m_cdig.get()) {
                m_cdig = std::make_unique<CDigestF>(static_cast<const uchar*>(cdig_data));
            }
        }
    }
    if (loadF) {		/* if already exist */
        if (show_flag) {
            PNOTE("Loaded Content Digest (CDigest) Extended Attribute (xattr) ");
            PNOTE("from file \""); std::cout << m_name << "\"\n";
        }
        if (remove_attr_if_too_small(xan)) { needF = false; }
    }

    const bool rehashF = (needF and not loadF); /* determine rehashing state */

    uint64_t fcsize = 0;         // file content size
    if (stat_is_readable(m_stat.get())) { fcsize = m_stat.get()->st_size; }

    /* determine blocksize */
    size_t blksize = 0;
    if (not blksize) {
        blksize = m_stat->st_blksize; /* use default blocksize */
    }
    // \todo Ask matchers for preferred block size and increase blksize if needed:
    // blksize = std::max(blksize, blksize_pi);

    const size_t bnum = fcsize / blksize; /* number of blocks */
    const size_t restsize = fcsize % blksize; /* block rest size */

    reset_hist8();

    open();                       /* open it */

    CHashF chash;

    /* iterate over file blocks and process them */
    uchar bbuf[blksize] __attribute__ ((aligned(16))); /* block buffer */
    for (size_t i = 0; i < bnum; i++) {      /* process whole blocks */
        lseek(m_fd, i*blksize, SEEK_SET); /* go current block */
        ssize_t rsize = ::read(m_fd, (char*)(bbuf), blksize);
        if (rsize == -1) { lperror("read()"); }
        else {
            process_block(bbuf, rsize); /* general processing */
            if (rehashF) { chash.update(bbuf, rsize); } /* increment hash */
        }
    }
    if (restsize >= 1) {             /* process last (rest) block */
        lseek(m_fd, bnum*blksize, SEEK_SET); /* go current block */
        ssize_t rsize = ::read(m_fd, (char*)bbuf, restsize);
        if (rsize == -1) { lperror("read()"); }
        else {
            process_block(bbuf, rsize); /* general processing */
            if (rehashF) { chash.update(bbuf, rsize); } /* increment hash */
        }
    }

    /* set chash file xattrs */
    if (rehashF) {
        if (const auto dig = m_cdig.get()) {
            chash.final(*dig);
        } else {
            m_cdig = std::make_unique<CDigestF>(chash);
        }

        cache_attr(xan, const_cast<const CDigestF*>(m_cdig.get())->data(), hsize, 0); // cache it
    }

    /* if (m_cdig.get()) { chash_print(CHASH_SHA2_256, m_cdig.get()->data()); } */

    ret = 1;
    return ret;
}

RegFile::TypeHits RegFile::g_hits;

int RegFile::load_types() const
{
    int cnt = 0;

    auto c_this = const_cast<RegFile*>(this);
    auto ftype = FileType::of(c_this); // determine file type of \p this
    if (ftype) {
        g_hits.insert(TypeHit(c_this, ftype));
        cnt++;
    }

    // Create Audio-File Type Super File Type.
    // \todo: Move this trigger FFMPEG-code analysis into a special \c FileType or \c Super class \c AVPatt.
    if (ftype) {
#ifdef HAVE_LIBAVFORMAT_AVFORMAT_H
        if (true or
            DFMT_is_MEDIA(ftype->get_dfmt())) {
            AVFormatContext *ctx = nullptr;
            const int av_ret = avformat_open_input(&ctx, path().c_str(), nullptr, nullptr); // obseletes \c av_open_input_file
            if (av_ret == 0) {
                avformat_show(ctx);
                av_close_input_file(ctx);
            } else {   /* file is not an audio or video file supported by FFMPEG. */
                std::cout << "File " << path() << " is not supported by FFMPEG" << std::endl;
            }
        }
#endif
#ifdef HAVE_OPENCV_CV_H
        if (DFMT_is_IMAGE(ftype->get_dfmt())) {
            auto img = imread_Raw();      // read file

            cv::Mat mask;               // mask. \todo What to choose for mask?

            // \see http://opencv.willowgarage.com/documentation/cpp/feature_detection.html

            // SIFT
            {
                cv::SIFT sift(1.0);         // SIFT: \todo What to choose for magnification?
                std::vector<cv::KeyPoint> kp; // keypoints
                cv::Mat desc;                 // ndescriptors
                sift(img, mask, kp, desc);
                // \todo memoize: (cv::SIFT, params, img, mask) => (kp, desc)
            }

            // SURF
            {
                auto hessianThreshold = 1.0;
                cv::SURF surf(hessianThreshold);
                std::vector<cv::KeyPoint> kp; // keypoints
                std::vector<float> desc;    // descriptors
                surf(img, mask, kp, desc); // \todo memoize: (cv::SURF, params, img, mask) => (kp, desc)
            }

            // FAST
            {
                std::vector<cv::KeyPoint> kp; // keypoints
                int threshold = 64; // threshold on difference between intensity of the central pixel and pixels on a circle around this pixel
                bool nonmaxSupression=true; // non-maximum supression is applied to detected corners (keypoints)
                cv::FAST(img, kp, threshold, nonmaxSupression); // \todo memoize: (cv::SURF, params, img, mask) => (kp, desc)
            }

            // FREAK
#if CV_VERSION_AT_LEAST(2,4,2)
            {
                std::vector<cv::KeyPoint> kp; // keypoints
                bool orientationNormalized=true;
                bool scaleNormalized=true;
                float patternScale=22.0f;
                int nOctaves=4;
                cv::FREAK(orientationNormalized, scaleNormalized, patternScale, nOctaves, kp) freak;
                freak(img, mask, kp, desc); // \todo memoize: (cv::SURF, params, img, mask) => (kp, desc)
                //auto pairs = FREAK::selectPairs(const vector<Mat>& images, vector<vector<KeyPoint>>& keypoints, const double corrThresh=0.7, bool verbose=true);
            }
#endif
}

#endif
    }
    return cnt;
}

std::vector<OP_t> RegFile::get_ops() const {
    std::vector<OP_t> ops;
    const auto parent = get_parent();
    if (parent and
        parent->is_writable()) {
        ops.push_back(OP_CLONE);
        ops.push_back(OP_DELETE);
    }
    if (is_writable()) {
        ops.push_back(OP_RENAME);
        ops.push_back(OP_REPLACE);
    }
    const auto hit = g_hits.left.find(const_cast<RegFile*>(this));
    if (hit != g_hits.left.end()) {
        hit->second->get_ops();
    }
    return ops;
}

/* ---------------------------- Group Separator ---------------------------- */

void RegFile::cache_attr(const csc& name, const void* value, size_t size, int flags)
{
    if (is_writable()) {
        set_attr(name.c_str(), value, size, flags);
    } else {
        auto pathL = get_pathL();
        PTODO("Write attribute named %s of read-only %s (%s) some place else\n",
              name.c_str(), path().c_str(), pathL.c_str());
    }
}

/* ---------------------------- Group Separator ---------------------------- */

ssize_t RegFile::pread(void *buf, size_t count, off_t offset)
{
    ssize_t ret = 0;
    if (open() >= 0) {
#ifdef HAVE_PREAD
        ret = ::pread(m_fd, buf, count, offset);
#else
        // off_t off =
        lseek(m_fd, offset, SEEK_SET);
        ret = ::read(m_fd, buf, count);
#endif
    }
    return ret;
}

ssize_t RegFile::pwrite(const void *buf, size_t count, off_t offset)
{
    ssize_t ret = 0;
    if (open() >= 0) {
#ifdef HAVE_PWRITE
        ret = ::pwrite(m_fd, buf, count, offset);
#else
        // off_t off =
        ::lseek(m_fd, offset, SEEK_SET);
        ret = ::write(m_fd, buf, count);
#endif
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

tdepth_t
RegFile::get_tree_depth() const
{
    tdepth_t tdepth = TDEPTH_undefined_;
    Ob * obP = net_first(REL_PARENT, OB_DIR);
    if (obP) {
        Dir * dirP = dynamic_cast<Dir*>(obP);
        tdepth = dirP->get_tree_depth() + 1; // Note: parent depth + 1
    }
    return tdepth;              // use cache
}

/* ---------------------------- Group Separator ---------------------------- */

int RegFile::idle_cb()
{
    return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

int
RegFile::cmp_content(const RegFile * pB) const
{
    int ret = 0;

    this->open();
    pB->open();

    // choose block size maximum as read granularity
    const size_t blksize = std::max(this->get_blksize(),
                                    pB->get_blksize());

    lseek(m_fd, 0, SEEK_SET); /* go to start in A */
    lseek(pB->m_fd, 0, SEEK_SET); /* go to start in B */

    char bbufA[blksize];		/* block buffer of A */
    char bbufB[blksize];		/* block buffer of B */

    ssize_t sizeA = 0;
    ssize_t sizeB = 0;

    while ((sizeA = ::read(m_fd, bbufA, blksize)) >= 1 and
           (sizeB = ::read(pB->m_fd, bbufB, blksize)) >= 1) { /* while next block */
        if        (sizeA < sizeB) {
            int sgn = memcmp(bbufA, bbufB, sizeA);
            if      (sgn < 0) { ret = sgn; }
            else if (sgn > 0) { ret = sgn; }
            else              { ret = -1; }
            break;
        } else if (sizeA > sizeB) {
            int sgn = memcmp(bbufA, bbufB, sizeB);
            if      (sgn < 0) { ret = sgn; }
            else if (sgn > 0) { ret = sgn; }
            else              { ret = +1; }
            break;
        } else {
            int sgn = memcmp(bbufA, bbufB, sizeA);
            if      (sgn < 0) { ret = sgn; break; }
            else if (sgn > 0) { ret = sgn; break; }
            else {  }			/* NOTE: still equal so continue loop */
        }
    }

    /* PTEST("Real comparison between %s and %s: ret:%d\n", */
    /*          this->get_pathL(), */
    /*          pB->get_pathL(), */
    /*          ret); */

    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

const uchar *
RegFile::get_chash(chash::chashid hid) const
{
    if (not m_cdig.get()) { const_cast<RegFile*>(this)->load(); }
    return const_cast<const CDigestF*>(m_cdig.get())->data();
}

const uchar*
RegFile::get_chash_dynamic(chash::chashid * hid_ret, uint32_t * size_ret) const
{
    size_t csize = 0; //get_tree_csize();
    if (csize) {
        *hid_ret = chash::chashid_from_size(csize);
        if (size_ret) { *size_ret = chash::chashid_get_digest_size(*hid_ret); } // if size requested set it
        return get_chash(*hid_ret);
    } else {
        return nullptr;
    }
}

/* ---------------------------- Group Separator ---------------------------- */

bool
RegFile::eq_chash(const RegFile * pB, chash::chashid hid) const
{
    bool ret = false;
    if (hid == chash::CHASH_undefined_) { hid = g_default_hid; }
    const uchar *tdigA = get_chash(hid);
    const uchar *tdigB = pB->get_chash(hid);
    if (tdigA and tdigB) {
        const uint32_t hsize = chash::chashid_get_digest_size(hid);
        ret = (memcmpn(tdigA, tdigB, hsize) != 0);
    } else { PERR("tdigA or tdigB undefined!\n"); }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

std::ostream &
RegFile::show(std::ostream& os) const
{
    File::show(os);
    os << " pathL:\"" << m_name << '\"'
       << " tdepth:" << get_tree_depth();

    const_cast<RegFile*>(this)->load();
    os << " ops: ";
    for (auto e : get_ops()) {
        os << e << ",";
    }

    os << " fmts: ";
    Obs fmts = net_find_rToT(REL_PARENT, OB_PATT_);
    for (uint i = 0; i < fmts.size(); i++) {
        os_spaces(os, get_tree_depth() + 1);
        if (auto ppatt = dynamic_cast<patterns::Base*>(fmts[i])) {
            os << ppatt->get_pathL() << std::endl;
        }
    }

    os << " cdig:";
    if (m_cdig.get()) {
        const uint32_t hsize = chashid_get_digest_size(g_default_hid);
        chararray_fprint_hex(stdout, const_cast<const CDigestF*>(m_cdig.get())->data(), hsize);
    } else {
        os << "none";
    }

    if (auto dfmt = dynamic_cast<patterns::Base*>(net_first(REL_PARENT, OB_PATT_))) {
        if (DFMT_is_IMAGE(dfmt->get_dfmt())) {
            if (open() >= 0) {
                //freeimage_fd_DFMT_printMetaData(stdout, m_fd, dfmt->get_dfmt());
            }
        }
    }
    return os;
}

}
}
