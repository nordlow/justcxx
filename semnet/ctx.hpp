/*! \file ctx.hpp
 * \brief Pattern Context.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <ostream>
#include <iosfwd>
#include "spatt.hpp"

namespace semnet {
namespace patterns {

/*! Pattern \em Recognition \em Context Delimiter.
 * \similar \c recog in filedb.el
 * \see \c Sit and \c PRPOS_t
 * \see \c FileType::RECOG_t
 */
typedef enum {
    PCTX_any_,                  ///< Any - Use what is available.

    PCTX_IN_HOST_NAME,          ///< In \em Name of Host (Computer).
    PCTX_IN_DISK_LABEL,         ///< In \em Name of Disk Name (Label).

    PCTX_IN_FILE_PATH,          ///< In \em Path of File.
    PCTX_IN_FILE_NAME,          ///< In \em Name of File.
    PCTX_IN_DIR_NAME,           ///< In \em Name of Directory.

    PCTX_IN_FILE_CONTENTS,      ///< In \em Contents of File.

    PCTX_IN_FILE_NAME_AND_CONTENTS, ///< In \em Name \em and \em Contents of \em File.
    PCTX_IN_FILE_NAME_OR_CONTENTS, ///< In \em Name \em or \em Contents of \em File.

    PCTX_IN_NETWORK_IP_PROTOCOL, ///< In Contents or \em IP-based Network Protocol.
} __attribute__ ((packed)) PCTX_t;

/*! Pattern \em Context. */
class Ctx : public Wrap {
public:
    Ctx(PCTX_t ctx, Base * sub = nullptr) : Wrap(sub), m_ctx(ctx) {}
    virtual ~Ctx() {}

    Ctx * set_ctx(PCTX_t ctx) { m_ctx = ctx; return this; }
    PCTX_t get_ctx() const { return m_ctx; }

    virtual std::ostream& show(std::ostream& os) const;

    virtual pHit match_in(const char* buf, size_t len,
                          bir roi = bir::full()) const;

protected:
    virtual Base* mutating_canonicalize();
private:
    PCTX_t m_ctx:4;             ///< \em Context.
};

namespace gen {

template<class... T> inline Ctx* ctx(T&&... a) { return new Ctx(a...); }

/*! Assert that \p sub lies in context \p ctx.
 * \return related pattern
 */
template<class... Tp> inline Ctx* ctx(PCTX_t ctx, Base* sub)
{
    auto ctxS = dynamic_cast<Ctx*>(sub);
    if (ctxS and
        ctxS->get_ctx() == ctx) { // \c this and its \c sub are same context
        return ctxS;
    } else {
        return new Ctx(ctx, sub);
    }
}
}

}}
