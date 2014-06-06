#include "aptdir.hpp"
#include "aptfile.hpp"

namespace semnet { namespace filesystem {

URI *
APTDir::new_sub(const csc& name, bool dir_flag)
{
    if (URI * sub = dir_flag ?
        reinterpret_cast<URI*>(new APTDir(name)) :
        reinterpret_cast<URI*>(new APTFile(name)))
    {
        net_connectS(REL_PARENT, this,
                     REL_CHILD, sub);
        if (m_subs_hash.insert(sub, name.size(), name.data()) == -1) {
            PERR("name already present in m_subs_hash\n");
        }
        return sub;
    } else { PERR("sub could not be allocated\n"); return nullptr; }
}

URI * APTDir::lookup_sub(const csc& name)
{
    return reinterpret_cast<URI*>(m_subs_hash.get(name.size(), name.data()));
}

URI * APTDir::get_sub(const csc& name, bool dir_flag, bool * load_flag)
{
    if (URI * file = lookup_sub(name)) {
        return file;
    } else {
        if (load_flag) { *load_flag = true; }
        return new_sub(name, dir_flag);
    }
}

}}
