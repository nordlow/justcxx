#include "lit.hpp"
#include "seq.hpp"
#include "sit.hpp"
#include "preg.hpp"
#include "know_common.hpp"
#include "../bitvec.h"
#include "../memory_x.hpp"
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <ostream>

namespace semnet {
namespace patterns {

bool
Lit::eq_wcbuf(const wchar_t * datL, size_t sizeL) const
{
    return (m_sizeL % sizeof(wchar_t) == 0 and
            m_sizeL / sizeof(wchar_t) == sizeL and
            wcsncmp((wchar_t*)m_datL.ptr, datL, sizeL) == 0);
}

bool
Lit::eq_ci_wcbuf(const wchar_t * datL, size_t sizeL) const
{
    return (m_sizeL % sizeof(wchar_t) == 0 and
            m_sizeL / sizeof(wchar_t) == sizeL and
            wcsncasecmp((wchar_t*)m_datL.ptr, datL, sizeL) == 0);
}

pHit
Lit::match_cbuf(const char* const buf, size_t len, pMode pmm) const
{
    return cbuf_find_LMATCH(buf,
                            len,
                            reinterpret_cast<const char*>(data()),
                            bytesize(),
                            pmm);
}

pHit
Lit::match_in_local(const char* buf, size_t len,
                    bir roi) const
{
    return match_cbuf(buf + to_byte(roi.low()), len);
}

csc
Lit::rand(bir ssr) const
{
    const auto sizeI = std::min(ssr.high(), bitsize());
    const auto bytesI = to_padded_byte(sizeI);
    char bufI[bytesI];
    memcpy(bufI, Lit::data(), bytesI); /* nothing random, just copy */
    return csc(bufI, bytesI);
}

Base::Skips8 &
Lit::intersect_skips(Skips8 & skips) const
{
    int cnt = 0;
    pMode pmm;
    if (bytesize() >= 1) {
        const uchar ch = get_byte(0);
        if (pmm.ci() and              /* case-insensitive */
            ::isalpha(ch)) {
            skips.set(::tolower(ch)); cnt += 1;
            skips.set(::toupper(ch)); cnt += 1;
        } else {
            skips.set(ch); cnt += 1;
        }
        if (pmm.usi()) {             /* space-underscore-insensitive */
            if (ch == '-') { skips.set('_'); cnt += 1; }
            if (ch == '_') { skips.set('-'); cnt += 1; }
        }
    }
    return skips;
}

const uchar *
Lit::get_chash(chash::chashid hid) const
{
    if (hid == chash::CHASH_undefined_) { hid = g_default_hid; }
    CHashF chash(reinterpret_cast<const uint8_t*>(data()), bytesize());
    if (const auto dig = m_ctdig.get()) {
        chash.final(*dig);
    } else {
        m_ctdig = std::make_unique<CDigestF>(chash);
    }
    return const_cast<const CDigestF*>(m_ctdig.get())->data();
}

std::ostream &
Lit::show(std::ostream & os) const
{
    Base::show(os);
    os << " sizeL:" << m_sizeL;
    Base::show_rand(os);
    os << std::endl;
    return os;
}

typedef std::unordered_map<csc, Lit*> RegLits;
static RegLits g_lreg; ///< Maps Strings to \em registered \c pLits.
typedef RegLits::value_type RegLit; ///< Registered Literal.

namespace gen {

Seq * lit_full(const char* nameL, size_t sizeL)
{
    return seq(bob(),
               lit(reinterpret_cast<const uchar*>(nameL), sizeL),
               eob());
}

Lit* lit_r(const csc & x)
{
    Lit * plit = nullptr;
    const auto hit = g_lreg.find(x);
    if (hit == g_lreg.end()) {
        g_lreg.insert(RegLit(x, plit = lit(reinterpret_cast<const uchar*>(x.data()), x.size())));
    } else {
        plit = hit->second;
    }
    return plit;
}

Seq * keyword(const char* x)
{
    auto s = seq(bos(), lit(x), eos());
    s->tokenize();
    return s;
}

Seq * keyword(const char x)
{
    auto s = seq(bos(), lit(x), eos());
    s->tokenize();
    return s;
}

}
}
}
