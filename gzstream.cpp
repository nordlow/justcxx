#include "gzstream.hpp"
#include <iostream>
#include <string.h>		// for memcpy

// --------------------------------------

bool
gzstreambuf::prepare_open (int open_mode, char fmode[10])
{
    if (is_open ()) {
        return false;
    }
    m_mode = open_mode;
    // no append nor read/write mode
    if ((m_mode bitand std::ios::ate) or
        (m_mode bitand std::ios::app) or
        ((m_mode bitand std::ios::in) and
         (m_mode bitand std::ios::out))) {
        return false;
    }
    char *p = fmode;
    if (m_mode bitand std::ios::in)
        *p++ = 'r';
    else if (m_mode bitand std::ios::out)
        *p++ = 'w';
    *p++ = 'b';
    *p = '\0';
    return true;
}

gzstreambuf*
gzstreambuf::open (const char *name, int open_mode, unsigned buffer_size)
{
    char fmode[10];
    if (not prepare_open(open_mode, fmode)) {
        return nullptr;
    }
    m_file = gzopen (name, fmode);
    if (m_file) {
        m_opened = true;
        // if (buffer_size != 8192) { gzbuffer(m_file, buffer_size); }
    } else {
        return nullptr;
    }
    return this;
}

gzstreambuf*
gzstreambuf::open (int fd, int open_mode, unsigned buffer_size)
{
    char fmode[10];
    if (not prepare_open(open_mode, fmode)) {
        return nullptr;
    }
    m_file = gzdopen(fd, fmode);
    if (m_file) {
        m_opened = true;
        //if (buffer_size != 8192) { gzbuffer((file, size)); }
    } else {
        return nullptr;
    }
    return this;
}

gzstreambuf*
gzstreambuf::close ()
{
    if (is_open ()) {
        sync ();
        m_opened = false;
        if (gzclose (m_file) == Z_OK)
            return this;
    }
    return (gzstreambuf *) nullptr;
}

int
gzstreambuf::underflow ()
{				// used for input buffer only
    if (gptr () and (gptr () < egptr ()))
        return *reinterpret_cast < unsigned char *>(gptr ());

    if (not (m_mode bitand std::ios::in) or not m_opened)
        return EOF;
    // Josuttis' implementation of inbuf
    int n_putback = gptr () - eback ();
    if (n_putback > 4)
        n_putback = 4;
    memcpy (m_buffer + (4 - n_putback), gptr () - n_putback, n_putback);

    int num = gzread (m_file, m_buffer + 4, bufferSize - 4);
    if (num <= 0)		// ERROR or EOF
        return EOF;

    // reset buffer pointers
    setg (m_buffer + (4 - n_putback), // beginning of putback area
          m_buffer + 4,		// read position
          m_buffer + 4 + num);	// end of buffer

    // return next character
    return *reinterpret_cast < unsigned char *>(gptr ());
}

int
gzstreambuf::flush_buffer ()
{
    // Separate the writing of the buffer from overflow() and
    // sync() operation.
    int w = pptr () - pbase ();
    if (gzwrite (m_file, pbase (), w) != w)
        return EOF;
    pbump (-w);
    return w;
}

int
gzstreambuf::overflow (int c)
{				// used for output buffer only
    if (not (m_mode bitand std::ios::out) or not m_opened)
        return EOF;
    if (c != EOF) {
        *pptr () = c;
        pbump (1);
    }
    if (flush_buffer () == EOF)
        return EOF;
    return c;
}

int
gzstreambuf::sync ()
{
    // Changed to use flush_buffer() instead of overflow( EOF)
    // which caused improper behavior with std::endl and flush(),
    // bug reported by Vincent Ricard.
    if (pptr () and pptr () > pbase ()) {
        if (flush_buffer () == EOF)
            return -1;
    }
    return 0;
}

// --------------------------------------

void gzstreambase::open (const char *name, int open_mode)
{
    if (not buf.open (name, open_mode)) {
        clear (rdstate () bitor std::ios::badbit);
    }
}

void gzstreambase::open (int fd, int open_mode)
{
    if (not buf.open (fd, open_mode)) {
        clear (rdstate () bitor std::ios::badbit);
    }
}

void gzstreambase::close ()
{
    if (buf.is_open ()) {
        if (not buf.close ()) {
            clear (rdstate () bitor std::ios::badbit);
        }
    }
}
