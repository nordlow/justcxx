/*! \file pmatchers.hpp
 * \brief Matches patterns in char arrays of specified length.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "ASCII.h"
#include "ISO8859.h"
#include "pmatch.hpp"
#include "cmatchers.h"
#include "HLANG_enum.hpp"
#include "stdio_x.h"
#include "../enforce.hpp"

inline void cbuf_lmatch_check(const char* pbuf, size_t len, bix off, pMode pmm) { enforce_lte(off, len); }

/*! Match Letter \em English Lowercase */
inline pHit cbuf_lmatch_LETTER_EN_LOWER(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_LETTER_EN_LOWER(pbuf[to_byte(off)]));
}
/*! Match Letter \em English Uppercase */
inline pHit cbuf_lmatch_LETTER_EN_UPPER(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_LETTER_EN_UPPER(pbuf[to_byte(off)]));
}
/*! Match Letter \em English Uppercase or Lowercase */
inline pHit cbuf_lmatch_LETTER_EN(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_LETTER_EN(pbuf[to_byte(off)]));
}
/*! Match Letter \em Swedish Letter. */
inline pHit cbuf_lmatch_LETTER_SV(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_LETTER_ISO8859_SV(pbuf[to_byte(off)]));
}
/*! Match Letter C Identifier Begin Letter */
inline pHit cbuf_lmatch_LETTER_CIDENT_BEGIN(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_LETTER_CIDENT_BEGIN(pbuf[to_byte(off)]));
}

/*! Match Letter Swedish-Only */
inline pHit cbuf_lmatch_LETTER_ISO8859_SVonly(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_LETTER_ISO8859_SVonly(pbuf[to_byte(off)]));
}
inline pHit cbuf_lmatch_LETTER_a_to_f(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_LETTER_a_to_f(pbuf[to_byte(off)]));
}
inline pHit cbuf_lmatch_LETTER_A_to_F(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_LETTER_A_to_F(pbuf[to_byte(off)]));
}

/*! Match Letter English Lowercase */
inline pHit cbuf_lmatch_LETTER_HEX(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_LETTER_HEX(pbuf[to_byte(off)]));
}

/*! Match English Vowel - aeiouy */
inline pHit cbuf_lmatch_VOWEL_EN(const char* pbuf, size_t len, bix off, pMode pmm)
{
    return pHit1(off, char_is_VOWEL_EN(pbuf[to_byte(off)]));
}
/*! Match Vowel Swedish-Only - åäö */
inline pHit cbuf_lmatch_VOWEL_SVonly(const char* pbuf, size_t len, bix off, pMode pmm)
{
    return pHit1(off, char_is_VOWEL_SVonly(pbuf[to_byte(off)]));
}
/*! Match Vowel Swedish - aeiouy, ĺäö */
inline pHit cbuf_lmatch_VOWEL_SV(const char* pbuf, size_t len, bix off, pMode pmm)
{
    return pHit1(off,
                 cbuf_lmatch_VOWEL_EN(pbuf,len,off, pmm) or
                 cbuf_lmatch_VOWEL_SVonly(pbuf,len,off, pmm));
}

/*! Match Consonant English - bcdfghjklmnpqrstvwxz */
inline pHit cbuf_lmatch_CONSONANT_EN(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off,
                 (cbuf_lmatch_LETTER_EN(pbuf,len,off, pmm) and
                  not cbuf_lmatch_VOWEL_EN(pbuf,len,off, pmm)));
}
/*! Match Consonant Swedish - bcdfghjklmnpqrstvwxz */
inline pHit cbuf_lmatch_CONSONANT_SV(const char* pbuf, size_t len, bix off, pMode pmm) {
    return cbuf_lmatch_CONSONANT_EN(pbuf,len,off, pmm);
}

/*! Match Digit Binary. */
inline pHit cbuf_lmatch_DIGIT_BIN(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_DIGIT_BIN(pbuf[to_byte(off)]));
}
/*! Match Digit Octal */
inline pHit cbuf_lmatch_DIGIT_OCT(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_DIGIT_OCT(pbuf[to_byte(off)]));
}
/*! Match Digit Decimal */
inline pHit cbuf_lmatch_DIGIT_DEC(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_DIGIT_DEC(pbuf[to_byte(off)]));
}
/*! Match Digit HexaDecimal */
inline pHit cbuf_lmatch_DIGIT_HEX(const char* pbuf, size_t len, bix off, pMode pmm) {
    return pHit1(off, char_is_DIGIT_HEX(pbuf[to_byte(off)]));
}

/*! Match Letter in Language \p lang */
pHit cbuf_lmatch_LETTER(const char* pbuf, size_t len, bix off, pMode pmm, HLANG_t lang);
/*! Match Letter C Identifier */
pHit cbuf_lmatch_CIdent(const char* pbuf, size_t len, bix off, pMode pmm);

/*! Match elements that have all the bits common with \p mask. */
pHit cbuf_lmatch_mask8bit(const char* pbuf, size_t len, bix off, pMode pmm, uchar mask);

inline pHit cbuf_lmatch_8bit_0to3(const char* pbuf, size_t len, bix off, pMode pmm) {
    return cbuf_lmatch_mask8bit(pbuf, len, off, pmm, 0x3);
}
inline pHit cbuf_lmatch_8bit_0to7(const char* pbuf, size_t len, bix off, pMode pmm) {
    return cbuf_lmatch_mask8bit(pbuf, len, off, pmm, 0x7);
}
inline pHit cbuf_lmatch_8bit_0to15(const char* pbuf, size_t len, bix off, pMode pmm) {
    return cbuf_lmatch_mask8bit(pbuf, len, off, pmm, 0xf);
}
inline pHit cbuf_lmatch_8bit_0to31(const char* pbuf, size_t len, bix off, pMode pmm) {
    return cbuf_lmatch_mask8bit(pbuf, len, off, pmm, 0x1f);
}
inline pHit cbuf_lmatch_8bit_0to63(const char* pbuf, size_t len, bix off, pMode pmm) {
    return cbuf_lmatch_mask8bit(pbuf, len, off, pmm, 0x3f);
}
inline pHit cbuf_lmatch_8bit_0to127(const char* pbuf, size_t len, bix off, pMode pmm) {
    return cbuf_lmatch_mask8bit(pbuf, len, off, pmm, 0x7f);
}
