/*!
 * \file regfile.hpp
 * \brief Regular File.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <boost/bimap.hpp>
#include <memory>
#include <fstream>
#include <vector>

#include "file.hpp"
#include "../fkind.h"
#include "../dfmt.h"
#include "PMAGIC_enum.h"
#include "../substr_match.h"

#include "../chash.hpp"
#include "../histogram.hpp"
#include "OP_enum.hpp"

#ifdef HAVE_OPENCV_CV_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

// namespace pnw { namespace histogram {
// template<class V, class C = size_t, class H = typename std::vector<C> > class dense;
// }}

class CDigest256;

namespace semnet { namespace filesystem {

class Dir;
class RegFile;
class FileType;

/*! Separator C-string for specifying file \em scope of tags, literals etc.
 * Proably ':' or '/'.
 */
#define FILE_SCOPE_OP_CSTR (":")

/*! \em Regular File, that is a file that have \em content. */
class RegFile: public File {
    friend class SymLink;
    friend class Dir;
public:
    typedef pnw::histogram::dense<uint8_t> CHist8;

    typedef boost::bimap<RegFile*, FileType*> TypeHits; // Maps Regular Files to their Corresponding Types
    typedef TypeHits::value_type TypeHit;
    virtual OB_t get_type() const { return OB_REALFILE; };
protected:
    RegFile(int fd, struct stat * statp = nullptr) : File() { init(fd, DFMT_any_, statp); }
    RegFile(FILE * file) : File() { init(fileno(file)); }
    RegFile(const csc& pathL, class Dir * dirP = nullptr,
            DFMT_t dfmt = DFMT_any_, struct stat * statp = nullptr)
        : File(pathL, dirP) {
        init(-1, dfmt, statp);
    }
    RegFile(int fd, const csc& pathL, class Dir * dirP = nullptr,
            DFMT_t dfmt = DFMT_any_, struct stat * statp = nullptr)
        : File(pathL, dirP) {
        init(fd, dfmt, statp);
    }

    virtual ~RegFile() { unload(); }

public:
    virtual int open() const { return File::open_fd(m_fd); }
    virtual int close() const { return File::close_fd(m_fd); }
    //! Get File \em Descriptor.
    virtual int get_fd() const { if (m_fd < 0) { open(); } return m_fd; }

/* ---------------------------- Group Separator ---------------------------- */

    /*! Process value \p a at byte offset \p off8. */
    void process_byte(const uchar* a, off_t off8);

    /*! Process value \p a at quad offset \p qoff. */
    void process_quad(const uchar* a, off_t qoff);

    /*! Process value \p a at oct offset \p ooff. */
    void process_oct(const uchar* a, off_t ooff);

    /*! Process value \p a at hex offset \p qoff. */
    void process_hex(const uchar* a, off_t qoff);

    /*! Process the block at \p bbuf of size \p bsize. */
    void process_block(const uchar* bbuf, uint bsize);

/* ---------------------------- Group Separator ---------------------------- */

    /*! Rescan and Cache Content \em Statistics.
     *
     * \p blksize Blocksize (Scanning Granularity).
     * Note: Set to 0 if default underlying preferred blocksize (from
     * struct stat member st_blksize).
     */
    virtual int load(bool recurse_flag = false, bool cscan_flag = false);
    virtual void unload() const;

    /*! Scan Contents and Calculate Statistics. */
    int cscan();
    /*! Detect File Types. */
    int load_types() const;
    /*! Get \em Possible Operations. */
    virtual std::vector<OP_t> get_ops() const;

    /*! Read Image. \todo Move to imgfile.hpp
     * \param flags can be either
     * - \c CV_LOAD_IMAGE_GRAYSCALE = 0,
     * - \c CV_LOAD_IMAGE_COLOR = 1,
     * - \c CV_LOAD_IMAGE_ANYDEPTH = 2,
     * - \c CV_LOAD_IMAGE_ANYCOLOR = 4,
     * - \c CV_LOAD_IMAGE_UNCHANGED = -1 (overrides them all).
     */
#ifdef HAVE_OPENCV_CV_H
    // \see http://opencv.itseez.com/modules/highgui/doc/reading_and_writing_images_and_video.html#Mat%20imread%28const%20string&%20filename,%20int%20flags%29
    cv::Mat imread(int flags = 1) const { return cv::imread(path().c_str(), flags); }
    cv::Mat imread_Gray() const { return imread(0); }
    cv::Mat imread_RGB() const { return imread(1); }
    cv::Mat imread_Raw() const { return imread(-1); }
#endif

    /*! Cache Attribute. */
    void cache_attr(const csc& name, const void* value, size_t size, int flags);

/* ---------------------------- Group Separator ---------------------------- */

    /*! \em Partial (Random Access) \em Read Block from \p prealfile at \p
     * offset of length \p count.
     */
    ssize_t pread(void *buf, size_t count, off_t offset);

    /*! \em Partial (Random Access) \em Write Block to \p prealfile at \p offset
     *  of length \p count.
     */
    ssize_t pwrite(const void *buf, size_t count, off_t offset);

/* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Get \em Content Hash.
     * \return pointer to internal hash result if any, nullptr otherwise.
     */
    virtual const uchar * get_chash(chash::chashid hid = chash::CHASH_SHA2_256) const;

    /*!
     * Get \em Dynamic \em Content Hash. Chooses a Suitable Hash
     * Function into \p hid_ret based on the size of encoded \p pA.
     *
     * \return pointer to internal hash result if any, nullptr otherwise.
     */
    const uchar * get_chash_dynamic(chash::chashid * hid_ret, uint32_t * size_ret) const;

/* ---------------------------- Group Separator ---------------------------- */

    virtual int cmp_content(const RegFile * pB) const;

    /*! Check for Equality of \em Content \em Hash.     */
    virtual bool eq_chash(const RegFile * pB, chash::chashid hid = chash::CHASH_SHA2_256) const;

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Check for Content Equality. */
    virtual bool eq_content(const RegFile * pB) const {
        // Note: Simple to default to calling cmp, but sub-optimal in most cases.
        return (eq_chash(pB) and cmp_content(pB) != 0);
    }

/* ---------------------------- Group Separator ---------------------------- */

    virtual int idle_cb();

/* ---------------------------- Group Separator ---------------------------- */

    virtual std::ostream& show(std::ostream& os) const;
    virtual tdepth_t get_tree_depth() const;
    virtual theight_t get_tree_height() const { return 0; }

protected:
    void assert_hist8();
    void reset_hist8();

    /*! Increment Histogram Bucket. */
    void inc_hist8 (uchar x);

private:
    void init(int fd, DFMT_t dfmt = DFMT_any_, const struct stat * statp = nullptr);

private:
    mutable int      m_fd;      ///< File Descriptor. TODO: Remove and replace with \c m_fs.

    //mutable std::unique_ptr<std::fstream> m_fs; ///< File Stream. As pointer to Minimize Memory Usage.
    mutable std::unique_ptr<CDigestF> m_cdig __attribute__ ((aligned(16))); ///< \em Content Hash \em Digest.
    mutable std::unique_ptr<CHist8>  m_chist8 __attribute__ ((aligned(16)));  ///< \em 8-bit (Byte) Content \em Histogram.

    static TypeHits g_hits;     ///< Pattern Match TypeHits.
};

}
}
