#include "sit.hpp"
#include "pmatchers.hpp"

namespace semnet { namespace patterns {

/*! Match Beginning-of-Word Pattern */
inline bool cbuf_lmatch_BOW(const char* pbuf, size_t len, bix off, pMode pmm, HLANG_t lang = HLANG_any_)
{
    cbuf_lmatch_check(pbuf, len, off, pmm);
    if (off == bix(0)) {
        return cbuf_lmatch_LETTER(pbuf,len,off, pmm, lang);
    } else {
        return (not cbuf_lmatch_LETTER(pbuf,len, off-8, pmm, lang) and
                cbuf_lmatch_LETTER(pbuf,len,off, pmm, lang));
    }
}

/*! Match End-of-Word Pattern */
inline bool cbuf_lmatch_EOW(const char* pbuf, size_t len, bix off, pMode pmm, HLANG_t lang = HLANG_any_)
{
    cbuf_lmatch_check(pbuf, len, off, pmm);
    if (off == len) {
        return cbuf_lmatch_LETTER(pbuf,len,off, pmm, lang);
    } else {
        return (cbuf_lmatch_LETTER(pbuf,len,off, pmm, lang) and
                not cbuf_lmatch_LETTER(pbuf,len, off+8, pmm, lang));
    }
}

/*! Match Beginning-of-Symbol Pattern */
inline bool cbuf_lmatch_BOS(const char* pbuf, size_t len, bix off, pMode pmm, HLANG_t lang = HLANG_any_)
{
    cbuf_lmatch_check(pbuf, len, off, pmm);
    if (off == bix(0)) {
        return cbuf_lmatch_LETTER_CIDENT_BEGIN(pbuf,len,off, pmm);
    } else {
        return (not cbuf_lmatch_LETTER_CIDENT_BEGIN(pbuf,len,off-8, pmm) and
                cbuf_lmatch_LETTER_CIDENT_BEGIN(pbuf,len,off, pmm));
    }
}

/*! Match End-of-Symbol Pattern */
inline bool cbuf_lmatch_EOS(const char* pbuf, size_t len,
                            bix off, pMode pmm, HLANG_t lang = HLANG_any_)
{
    cbuf_lmatch_check(pbuf, len, off, pmm);
    if (off == len) {
        return cbuf_lmatch_LETTER_CIDENT_BEGIN(pbuf,len,off, pmm);
    } else {
        return (cbuf_lmatch_LETTER_CIDENT_BEGIN(pbuf,len,off, pmm) and
                not cbuf_lmatch_LETTER_CIDENT_BEGIN(pbuf,len,off+8, pmm));
    }
}

/*! Match \em Beginning-Of-Line Pattern */
inline bool cbuf_lmatch_BOL(const char* pbuf, size_t len,
                            bix off_, pMode pmm, HLANG_t lang = HLANG_any_)
{
    const auto off = to_byte(off_);
    if (len >= 2 and
        off >= 2) {             /* CR-LF before cursor */
        return (pbuf[off-2] == ASCII_CR and
                pbuf[off-1] == ASCII_LF);
    } else if (len >= 1 and
               off >= 1) { /* CR or LF before cursor */
        return (pbuf[off-1] == ASCII_CR or
                pbuf[off-1] == ASCII_LF);
    } else {
        return false;
    }
}

/*! Match \em End-Of-Line Pattern */
inline bool cbuf_lmatch_EOL(const char* pbuf, size_t len,
                            bix off_, pMode pmm, HLANG_t lang = HLANG_any_)
{
    const auto off = to_byte(off_);
    if (len >= 2 and
        off < len-1) { /* CR-LF after cursor */
        return (pbuf[off] == ASCII_CR and
                pbuf[off+1] == ASCII_LF);
    } else if (len >= 1 and
               off < len) { /* CR or LF after cursor */
        return (pbuf[off] == ASCII_CR or
                pbuf[off] == ASCII_LF);
    } else {
        return false;
    }
}

pHit
Sit::match_in(const char* buf, size_t len,
              bir roi) const
{
    return match_in_special(buf, len, roi);
}

pHit
Sit::match_in_local(const char * buf, size_t len,
                    bir roi) const
{
    auto off = roi.low();     // offset
    bool hit = false;
    pMode pmm;
    switch (m_rpos) {
    case PRPOS_BOW: hit = cbuf_lmatch_BOW(buf,len, off, pmm, m_lang); break;
    case PRPOS_EOW: hit = cbuf_lmatch_EOW(buf,len, off, pmm, m_lang); break;

    case PRPOS_BOS: hit = cbuf_lmatch_BOS(buf,len, off, pmm, m_lang); break;
    case PRPOS_EOS: hit = cbuf_lmatch_EOS(buf,len, off, pmm, m_lang); break;

    case PRPOS_BOL: hit = cbuf_lmatch_BOL(buf,len, off, pmm, m_lang); break;
    case PRPOS_EOL: hit = cbuf_lmatch_EOL(buf,len, off, pmm, m_lang); break;

    case PRPOS_BOB: { hit = (off == bix(0)); }  break;
    case PRPOS_EOB: { hit = (off == len); }  break;
    default: break;
    }

    return pHit(bir(off, off), hit);
}

virtual std::ostream& Sit::show(std::ostream& os) const {
    Base::show(os);
    os << " rpos:" << PRPOS_getName(m_rpos);
    Base::show_rand(os);
    return os;
}

}}
