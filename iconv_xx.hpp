/*! \file iconv_xx.hpp
 * \brief C++ Wrapper Interface to <iconv.h>
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <iconv.h>
#include <string>
#include <iostream>

/// Convert \p in in the format \p in_code to \p out.
inline size_t
iconv_convert_to_wchar(std::string& in, const char * in_code,
                       std::string<wchar_t>& out)
{
    const iconv_t id = iconv_open("WCHAR_T", in_code); // open conversion
    if (id == (iconv_t)-1) { std::cerr << "iconv_open()" << std::endl; }
    out.resize(in.size());  // assure that \p out is large enough
    const size_t numC = iconv(id, x);
    iconv_close(id);        // close conversion
}

/*! C++ Wrapper Interface Class for <iconv.h> */
class IConv {
public:
    /// Default constructor of conversion from \p from_code to \p to_code.
    IConv(const char *from_code, const char *to_code) {
        iconv_t id = iconv_open(tocode, in_code);
    }
    /// Destructor.
    ~IConv() { iconv_close(id); }
protected:
    iconv_t m_id;
private:
};
