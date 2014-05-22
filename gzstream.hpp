// ============================================================================
// gzstream, C++ iostream classes wrapping the zlib compression library.
//
// Standard streambuf implementation following Nicolai Josuttis, "The
// Standard C++ Library".
// ============================================================================

#pragma once

#ifndef GZSTREAM_H
#define GZSTREAM_H 1

#include <iostream>
#include <fstream>
#include <zlib.h>

// ----------------------------------------------------------------------------
// Internal classes to implement gzstream. See header file for user classes.
// ----------------------------------------------------------------------------

class gzstreambuf : public std::streambuf
{
public:
    gzstreambuf() : m_opened (false) {
        setp (m_buffer, m_buffer + (bufferSize - 1));
        setg (m_buffer + 4,		// beginning of putback area
              m_buffer + 4,		// read position
              m_buffer + 4);	// end position
        // ASSERT: both input & output capabilities will not be used together
    }
    ~gzstreambuf() { close(); }

    bool is_open() const { return m_opened; }
    gzstreambuf* open(const char *name, int open_mode, unsigned buffer_size = 8192);
    gzstreambuf* open(int fd, int open_mode, unsigned buffer_size = 8192);
    gzstreambuf* close();

    // Set the internal buffer size. The default buffer size is 8192 bytes. This
    // function must be called after gzopen() or gzdopen(), and before any other
    // calls that read or write the file. The buffer memory allocation is always
    // deferred to the first read or write. Two buffers are allocated, either
    // both of the specified size when writing, or one of the specified size and
    // the other twice that size when reading. A larger buffer size of, for
    // example, 64K or 128K bytes will noticeably increase the speed of
    // decompression (reading).
    bool set_buffer_size(unsigned size = 8192) {
        // return (is_open() and
        // (gzbuffer((m_file, size)) == 0));
        return false;
    }

    virtual int overflow (int c = EOF);
    virtual int underflow ();
    virtual int sync ();
private:
    static const int bufferSize = 47 + 256;	// size of data buff
    // totals 512 bytes under g++ for igzstream at the end.

    gzFile m_file;              // file handle for compressed file
    char m_buffer[bufferSize];	// data buffer
    int m_mode;                 // I/O mode
    bool m_opened:1;            // open/close state of stream

    int flush_buffer();
    bool prepare_open(int open_mode, char fmode[10]);
};

class gzstreambase : public virtual std::ios
{
public:
    gzstreambase () { init (&buf); }
    gzstreambase (const char *name, int mode) { init (&buf); open (name, mode); }
    gzstreambase (int fd, int mode) { init (&buf); open (fd, mode); }
    ~gzstreambase () { buf.close (); }

    void open (const char *name, int mode);
    void open (int fd, int mode);
    void close ();

    gzstreambuf *rdbuf () { return &buf; }
protected:
    gzstreambuf buf;
};

// ----------------------------------------------------------------------------
// User classes. Use igzstream and ogzstream analogously to ifstream and
// ofstream respectively. They read and write files based on the gz*
// function interface of the zlib. Files are compatible with gzip compression.
// ----------------------------------------------------------------------------

class igzstream: public gzstreambase, public std::istream
{
public:
    igzstream() :std::istream (&buf) { }
    igzstream (const char *name, int open_mode = std::ios::in)
        :gzstreambase (name, open_mode), std::istream (&buf)
    {}

    gzstreambuf *rdbuf () { return gzstreambase::rdbuf (); }

    void open (const char *name, int open_mode = std::ios::in) { gzstreambase::open (name, open_mode); }
};

class ogzstream: public gzstreambase, public std::ostream
{
public:
    ogzstream () :std::ostream (&buf) {}
    ogzstream (const char *name, int mode = std::ios::out)
        :gzstreambase (name, mode), std::ostream (&buf) {}

    gzstreambuf *rdbuf () { return gzstreambase::rdbuf (); }

    void open (const char *name, int open_mode = std::ios::out) { gzstreambase::open (name, open_mode); }
};

#endif
