#include "fileattr.hpp"
#include "file.hpp"
#include <string>

namespace semnet { namespace filesystem {

std::ostream&
FileAttr::show(std::ostream& os) const
{
    os << " name:\"" << m_name << "\""
       << " value:\"" << m_val << "\"" << std::endl;
    return os;
}

}}
