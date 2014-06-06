/*!
 * \file dirtype.hpp
 * \brief Directory Type Specifier.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "filetype.hpp"

namespace semnet {
namespace filesystem {

/*! Directory Type Pattern.
 */
class DirType : public FileType {
public:
    DirType(patterns::Base * nfmt = nullptr,
            patterns::Base * cfmt = nullptr,
            const char * name = nullptr,
            const char * doc = nullptr,
            DFMT_t dfmt = DFMT_any_,
            std::vector<OP_t> ops = std::vector<OP_t>())
        : FileType(nfmt, cfmt, name, doc, dfmt)
    {}

    virtual ~DirType() {}
};

}
}
