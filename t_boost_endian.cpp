/*! \file t_boost_endian.cpp
 * \brief Test Boost Endian Integers
 * \see http://boost.cowic.de/rc/endian/doc/integers.html
 */

#include <iostream>
#include <cstdio>
#include <boost/endian/types.hpp>

using namespace boost::endian;

namespace
{
// This is an extract from a very widely used GIS file format. I have no idea
// why a designer would mix big and little endians in the same file - but
// this is a real-world format and users wishing to write low level code
// manipulating these files have to deal with the mixed endianness.

struct header
{
    big_int32_t     file_code;
    big_int32_t     file_length;
    little_int32_t  version;
    little_int32_t  shape_type;
};

const char * filename = "t_boost_endian.dat";
}

int main()
{
    static_assert(sizeof( header ) == 16U,
                  "Header of wrong size"); // check requirement

    header h;

    h.file_code   = 0x01020304;
    h.file_length = sizeof( header );
    h.version     = -1;
    h.shape_type  = 0x01020304;

    // Low-level I/O such as POSIX read/write or <cstdio> fread/fwrite is sometimes
    // used for binary file operations when ultimate efficiency is important.
    // Such I/O is often performed in some C++ wrapper class, but to drive home the
    // point that endian integers are often used in fairly low-level code that
    // does bulk I/O operations, <cstdio> fopen/fwrite is used for I/O in this example.

    std::FILE * fi;

    if (not (fi = std::fopen( filename, "wb" )) ) {  // MUST BE BINARY
        std::cout << "could not open " << filename << '\n';
        return 1;
    }
    if ( std::fwrite( &h, sizeof( header ), 1, fi ) != 1 )  {
        std::cout << "write failure for " << filename << '\n';
        return 1;
    }
    std::fclose( fi );
    std::cout << "created file " << filename << '\n';
    return 0;
}
