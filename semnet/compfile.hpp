/*! \file compfile.hpp
 * \brief Compressed File.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include "regfile.hpp"
#include "../csc.hpp"

namespace semnet {
namespace filesystem {

/*! Compressed File. */
class CompFile : public RegFile {
public:
    CompFile(int fd) : RegFile(fd) { }
    CompFile(FILE * file) : RegFile(file) { }
    virtual ~CompFile() {}
protected:
private:
};

/*! GZip Compressed File. */
class GZFile : public CompFile {
public:
    GZFile(int fd) : CompFile(fd) { }
    GZFile(FILE * file) : CompFile(file) { }
    virtual ~GZFile() {}
protected:
private:
};

/*! BZip Compressed File. */
class BZFile : public CompFile {
public:
    BZFile(int fd) : CompFile(fd) { }
    BZFile(FILE * file) : CompFile(file) { }
    virtual ~BZFile() {}
protected:
private:
};

/*! XZip Compressed File. */
class XZFile : public CompFile {
public:
    XZFile(int fd) : CompFile(fd) { }
    XZFile(FILE * file) : CompFile(file) { }
    virtual ~XZFile() {}
protected:
private:
};

/*! LZMAip Compressed File. */
class LZMAFile : public CompFile {
public:
    LZMAFile(int fd) : CompFile(fd) { }
    LZMAFile(FILE * file) : CompFile(file) { }
    virtual ~LZMAFile() {}
protected:
private:
};

// CompFile * make_CompFile(int fd);

}}
