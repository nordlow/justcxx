/*!
 * \file deb.hpp
 * \brief Debian/Ubuntu Package.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-11-22 13:41
 */

#pragma once
#include "uri.hpp"

namespace semnet {
namespace filesystem {

/*!
 * Debian/Ubuntu Package
 * Name is actually a path.
 */
class Deb : public URI
{
public:
    /// Default Constructor.
    Deb(const csc& name) : URI(name) {}
    /// Destructor.
    virtual ~Deb() {}

    virtual OB_t get_type() const { return OB_DEB; }
protected:
private:
};

}
}
