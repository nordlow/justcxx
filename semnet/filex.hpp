/*! \file filex.hpp
 * \brief File Extension Pattern Generators.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "ctx.hpp"
#include "lit.hpp"
#include "alt.hpp"
#include "seq.hpp"
#include "sit.hpp"
#include "know_common.hpp"

namespace semnet { namespace filesystem { namespace gen {

using namespace patterns;
using namespace patterns::gen;

/*! Create Matcher for files named \p litA och length \p sizeL at \c PRPOS_BOB. */
inline Ctx * atBOB(const char * litA[], const size_t sizeA[], size_t num, PCTX_t pctx = PCTX_any_) {
    return ctx(pctx,
               seq(alt(litA, sizeA, num),
                   bob()));
}
/*! Create Matcher for files named \p litA och length \p sizeL at \c PRPOS_EOB. */
inline Ctx * atEOB(const char * litA[], const size_t sizeA[], size_t num, PCTX_t pctx = PCTX_any_) {
    return ctx(pctx,
               seq(alt(litA, sizeA, num),
                   eob()));
}

/*! Create Matcher for files with \p num names \p litA at \c PRPOS_BOB. */
inline Ctx * atBOB(const char * litA[], size_t num, PCTX_t pctx = PCTX_any_) {
    return ctx(pctx,
               seq(alt(litA, num),
                   bob()));
}
/*! Create Matcher for files with \p num names \p litA at \c PRPOS_EOB. */
inline Ctx * atEOB(const char * litA[], size_t num, PCTX_t pctx = PCTX_any_) {
    return ctx(pctx,
               seq(alt(litA, num),
                   eob()));
}

/*! Create Matcher for File Names \em Beginning with alternatives \p subs. */
template<class... T> inline Ctx * bofn(T*... subs) {
    const char* a[sizeof...(subs)] = { subs... };
    return atBOB(a, sizeof...(subs), PCTX_IN_FILE_NAME);
}
/*! Create Matcher for File Names \em Ending with alternatives \p subs. */
template<class... T> inline Ctx * eofn(T*... subs) {
    const char* a[sizeof...(subs)] = { subs... };
    return atEOB(a, sizeof...(subs), PCTX_IN_FILE_NAME);
}
/*! Create Matcher for File Names with \em Extension with alternatives \p subs. */
template<class... T> inline Ctx * fnx(T*... subs) {
    const size_t num = sizeof...(subs);
    const char* a[sizeof...(subs)] = { subs... };
    return ctx(PCTX_IN_FILE_NAME,
               seq(lit("."),
                   alt(a, num),
                   eob()));
}

}}}
