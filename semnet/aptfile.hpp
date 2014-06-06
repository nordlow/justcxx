/*! \file aptfile.hpp
 * \brief APT File or Directory
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "uri.hpp"

namespace semnet {
namespace filesystem {

/*! APT File or Directory. */
class APTFile : public URI
{
public:
    APTFile(const csc& name) : URI(name) {}

    virtual ~APTFile() {}

    virtual OB_t get_type() const { return OB_APTFILE; }
protected:
private:
};

}
}
