/*! \file binary_stream.hpp
 * \brief Input and output stream for binary encoding and decoding.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * All streams provide the stream operators for either encoding or decoding of
 * all types having the related encode() or decode() functions defined to it.
 */

#pragma once
#include <fstream>
#include <sstream>
#include "algorithm_x.hpp"
#include "encode.hpp"
#include "decode.hpp"

/// Binary ostringstream.
class bin_oss : public std::ostringstream
{
public:
    /// Encoding (serializing) operator.
    template<class T> friend bin_oss & operator << (bin_oss & os, const T & a) { encode(os, a); return os; }
};
/// Binary istringstream.
class bin_iss : public std::istringstream
{
public:
    bin_iss(const bin_oss & bos) : std::istringstream(bos.str()) {}
    /// Decoding (unserializing) operator.
    template<class T> friend bin_iss & operator >> (bin_iss & is, T & a) { decode(is, a); return is; }
};

/// Binary ofstream.
class bin_ofs : public std::ofstream
{
public:
    bin_ofs(const char * path) : std::ofstream(path) {}
    /// Encoding (serializing) operator.
    template<class T> friend bin_ofs & operator << (bin_ofs & os, const T & a) { encode(os, a); return os; }
};
/// Binary ifstream.
class bin_ifs : public std::ifstream
{
public:
    /// Decoding (unserializing) operator.
    bin_ifs(const char * path) : std::ifstream(path) {}
    template<class T> friend bin_ifs & operator >> (bin_ifs & is, T & a) { decode(is, a); return is; }
};
