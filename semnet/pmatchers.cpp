#include "pmatchers.hpp"
#include "decode.h"

pHit cbuf_lmatch_CIdent(const char * pbuf, size_t len,
                        bix off_, pMode pmm)
{
    size_t off = to_byte(off_);
    pmret_t ret = 0;
    auto i = off;               /* counter from offset */
    switch (pmm.get_type()) {
    case MATCH_FULL:
        if (char_is_LETTER_CIDENT_BEGIN(pbuf[i])) i++; /* first char must be beginning */
        for (; off < len; i++) {    /* rest chars \em must be rest */
            if (!char_is_LETTER_CIDENT_REST(pbuf[i])) { break; }
        }
        ret = (i == len) ? i-off : 0; /* all or nothing */
        break;
    case MATCH_PREFIX:
        if (char_is_LETTER_CIDENT_BEGIN(pbuf[i])) i++; /* first char must be beginning */
        for (; i < len; i++) {      /* rest chars \em may be rest */
            if (!char_is_LETTER_CIDENT_REST(pbuf[i])) { break; }
        }
        ret = (i > off) ? i-off : 0; /* one or more */
        break;
    case MATCH_SUFFIX:
        PTODO("Search backwards... to off\n");
        break;
    case MATCH_PARTIAL: {
        for (; i < len; i++) {      /* find begin char */
            if (char_is_LETTER_CIDENT_BEGIN(pbuf[i])) { break; }
        }
        auto off0 = i;              /* either at begin char or at end */
        for (i = off0+1; i < len; i++) { /* rest chars \em may be rest */
            if (!char_is_LETTER_CIDENT_REST(pbuf[i])) { break; }
        }
        ret = (off0 < len and (i - off0) >= 1) ? (i-off0) : 0;
        break;
    }
    default:
        PTODO("case MATCH_t:%d\n", pmm.get_type());
        break;
    }

    return pHit(to_bit(off),
                to_bit(ret) ,
                ret >= len ? IDIST_FULL : len - ret);
}

/* ---------------------------- Group Separator ---------------------------- */

pHit cbuf_lmatch_mask8bit(const char * pbuf, size_t len_,
                          bix off, pMode pmm, uchar mask)
{
    const auto len = to_byte(len_);
    const size_t qN = len / 4;    /* quad number */
    const size_t qR = len % 4;    /* quad rest */
    const uint32_t qmask = uint32_rep4(mask);
    auto i = to_byte(off);               /* offset iterator */

    switch (pmm.get_type()) {
    case MATCH_FULL:
        PTODO("MATCH_FULL\n");
        break;
    case MATCH_PREFIX:
        for (size_t q = 0; i < qN; q++, i+=4) { /* for all byte-quads (32-bit blocks) */
            const uint32_t quad = bdec_u32ne(reinterpret_cast<const unsigned char*>(&pbuf[i])); /* Note: no need to care about byte-order here */
            if (quad & ~qmask) { goto done; }
        }
        for (size_t r = 0; i < qR; r++, i++) { /* for all rest-bytes */
            if (pbuf[i] & ~mask) { goto done; }
        }
        break;
    case MATCH_SUFFIX:
        PTODO("Search backwards... to off\n");
        break;
    case MATCH_PARTIAL:
        PTODO("MATCH_PARTIAL\n");
        break;
    default:
        break;
    }

done:                  /* we have reached the end of the match at offset \c i */
    const pmret_t ret = i-off;
    return pHit(8*off, 8*ret, ret >= len ? IDIST_FULL : 8*(len - ret));
}

pHit cbuf_lmatch_LETTER(const char * pbuf, size_t len,
                        bix off, pMode pmm, HLANG_t lang)
{
    switch (lang) {
    case HLANG_EN: return cbuf_lmatch_LETTER_EN(pbuf, len, off, pmm); break;
    case HLANG_SV: return cbuf_lmatch_LETTER_SV(pbuf, len, off, pmm); break;
    default: return cbuf_lmatch_LETTER_EN(pbuf, len, off, pmm); break;
    }
}
