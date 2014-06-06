/*!
 * \file dup.hpp
 * \brief Object Duplicate.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#if 0
#include "obmr.hpp"
#include "rels.hpp"
#include "../fkind.h"
#include "../substr_match.h"

/* ========================================================================= */

namespace semnet {

/*! Duplicate Type. */
typedef enum {
    PDUP_CONTENT,                 ///< Content Duplicate.
    PDUP_NAME,                    ///< Name Duplicate.
} __attribute__ ((packed)) PDUP_t;

/*!
 * semnetDB Duplicate.
 */
class Dup: public ObMr {
public:
    virtual OB_t get_type() const { return OB_DUP; };

    Dup(PDUP_t dupT) : ObMr() { m_dupT = dupT; }

    virtual std::ostream& show(std::ostream& os) const {
        return os << " type:" << ((m_dupT == PDUP_NAME) ? "NAME" : "CONTENT");
    }

protected:
private:
    PDUP_t m_dupT;              ///< Duplicate Type.
};

namespace gen
{
inline Dup* dup(PDUP_t dupT)
{
    Dup * pdup = new Dup(dupT);
    g_oreg->add1(pdup);
    return pdup;
}
}

/* ---------------------------- Group Separator ---------------------------- */

void parr_update_name_duplicates(Ob ** obs, size_t obs_N, Refs& new_dups);
void parr_update_content_duplicates(Ob ** obs, size_t obs_N, Refs& new_dups);

}

#endif
