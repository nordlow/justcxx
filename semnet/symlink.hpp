/*!
 * \file symlink.hpp
 * \brief Symbolic Link File.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include "file.hpp"
#include "../chash.hpp"

namespace semnet {
namespace filesystem {

class SymLink;
typedef std::unordered_set<SymLink*> SymLinks;

/*!
 * \em Symbolic Link File.
 */
class SymLink: public File {
    friend class RegFile;
    friend class Dir;
public:
    typedef std::unordered_map<File*,SymLinks> Sources;
    virtual OB_t get_type() const { return OB_SYMLINKFILE; };

protected:
    /*! New Symbolic Link from file descriptor \p fd.     */
    SymLink(int fd) : File(), m_dest(nullptr) { init(fd); }

    /*! New Symbolic Link from FILE-stream \p file.     */
    SymLink(FILE * file) : File(), m_dest(nullptr) { init(fileno(file)); }

    /*! New Symbolic Link from path \p pathL with flags \p flags. */
    SymLink(const csc& pathL, class Dir * parent,
             struct stat * statp = nullptr)
        : File(pathL, parent), m_dest(nullptr) {
        init(-1, statp);
    }

    virtual ~SymLink();

public:
    virtual int open() const { return File::open_fd(m_fd); }
    virtual int close() const { return File::close_fd(m_fd); }
    //! Get File \em Descriptor.
    virtual int get_fd() const { if (m_fd < 0) { open(); } return m_fd; }

    /*! Follow (Chase) Link.
     *
     * \param all_flag is \c true if \em all levels should followed (chase).
     * \return link target if exists, \c nullptr if \c this if \em unresolved
     * link, or \c this if no link.
     */
    File* follow(bool all_flag = false);
    File* chase() { return follow(true); }

    /*! \em Read (\em Resolve) Symbolic Link Destination (Target) and put result
     * in \p destB containing \p destN reserved bytes. */
    ssize_t read_destination(char *destB, char destN) const;

    /*!
     * Get \em Content Hash.
     * \return pointer to internal hash result if any, nullptr otherwise.
     */
    virtual const uchar * get_chash(chash::chashid hid = chash::CHASH_SHA2_256) const;

    virtual int load(bool recurse_flag = false, bool cscan_flag = false);
    virtual void unload() const;

    // Follow Link when idle.
    virtual int idle_cb() { return follow() != nullptr; }

public:
    /*! Lookup \c set of SymLink that targets \p dest.
     * \return \c SymLinks* if defined, 0 otherwise.
     */
    static SymLinks* get_links_to(File * dest);
private:
    void erase_self_ref();
    void insert_self_ref();
private:
    void init(int fd, const struct stat * statp = nullptr);
    mutable int        m_fd;    ///< File Descriptor.
    mutable File*      m_dest;  ///< Link Target.
    mutable std::unique_ptr<CDigestF> m_cdig __attribute__ ((aligned(16)));  ///< \em Content Hash \em Digest.
    static Sources     m_sources; ///< Reverse Map from Link to Source Files.
};

}
}
