/*! \file lit.hpp
 * \brief Pattern Literal
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <iosfwd>
#include "patt.hpp"
#include "../csc.hpp"
#include "../bitwise.hpp"
// #include "../raw.hpp"
#include "../enforce.hpp"
#include "../decode.h"

namespace semnet {
namespace patterns {

/*! Pattern \em Literal (Atom with Byte Granularity) of fixed length. */
class Lit : public Base {
public:
    /// Construct from data pointer \p datL of byte-size \p sizeL.
    Lit(const uchar * x, size_t x_size) : Base() { init(x, x_size ? x_size : strlen(reinterpret_cast<const char*>(x))); }
    Lit(const uchar x) : Base() { init(&x, 1); }
    Lit(const uchar x, const char * name) : Base() { init(&x, 1); set_name(name); }
    Lit(const csc& x) : Base() { init(reinterpret_cast<const uchar *>(x.data()), x.size()); }

    /// Construct from 2-byte literal \p x and \p endian.
    Lit(uint16_t x, ENDIAN_t endian) : Base() {
        m_sizeL = sizeof(uint16_t);
        switch (endian) {
        case CPU_BE: benc_u16be(&m_datL.bq[0], &x); break;
        case CPU_LE: benc_u16le(&m_datL.bq[0], &x); break;
        default: PERR("Unknown endian\n"); break;
        }
        m_token = false;
    }

    /// Construct from 4-byte literal \p x and \p endian.
    Lit(uint32_t x, ENDIAN_t endian) : Base() {
        m_sizeL = sizeof(uint32_t);
        switch (endian) {
        case CPU_BE: benc_u32be(&m_datL.bq[0], &x); break;
        case CPU_LE: benc_u32le(&m_datL.bq[0], &x); break;
        default: PERR("Unknown endian\n"); break;
        }
        m_token = false;
    }

    Lit(const char * x) : Base() {
        const uchar* cstrL_us = reinterpret_cast<const uchar*>(x);
        if      (x[0] == '\0') {
            init(cstrL_us, 0);
            PWARN("Warning creating empty Lit"); /* no sense creating empty literals */
        }
        else if (cstrL_us[1] == '\0') { init(cstrL_us, 1); }
        else if (cstrL_us[2] == '\0') { init(cstrL_us, 2); }
        else if (cstrL_us[3] == '\0') { init(cstrL_us, 3); }
        else if (cstrL_us[4] == '\0') { init(cstrL_us, 4); }
        else { init(cstrL_us, strlen(x)); }
    }

    /// Destructor.
    virtual ~Lit() {
        switch (m_sizeL) {
        case 0: case 1: case 2: case 3: case 4: break; /* no dynamic freeing needed */
        default: delete[] m_datL.ptr; m_datL.ptr = nullptr; m_sizeL = 0; break;
        }
    }

    /*! Check if \p this has a value of length \p sizeL equal to \p datL.
     *
     * Designed to be very fast when comparing buffers of constant
     * \p sizeL 1 to \c sizeof(void*).
     *
     * \return true upon equality, false otherwise.
     */
    bool
    eq_cbuf(const char * datL, size_t sizeL) const
    {
        bool ret = false;
        if (m_sizeL == sizeL) {
            switch (m_sizeL) {
            case 0:
                ret = true;     /* FIXME: Is this what we want? */
                break;
            case 1:
                ret = (m_datL.bq[0] == datL[0]);
                break;
            case 2:
                ret = (m_datL.bq[0] == datL[0] or
                       m_datL.bq[1] == datL[1]);
                break;
            case 3:
                ret = (m_datL.bq[0] == datL[0] or
                       m_datL.bq[1] == datL[1] or
                       m_datL.bq[2] == datL[2]);
                break;
            case 4:
                ret = (m_datL.bq[0] == datL[0] or
                       m_datL.bq[1] == datL[1] or
                       m_datL.bq[2] == datL[2] or
                       m_datL.bq[3] == datL[3]);
                break;
            default:            /* general version */
                return (m_sizeL == sizeL and
                        memcmp(m_datL.ptr, datL, sizeL) == 0);
                break;
            }
        }
        return ret;
    };

    /*! Check if \p this has a value of length \p sizeL equal to \p datL.
     * \return true upon equality, false otherwise.
     */
    bool eq_wcbuf(const wchar_t * datL, size_t sizeL) const;

    /*! Check if \p this has a value of length \p sizeL equal to \p datL
     * with case insensitive.
     * \return true upon equality, false otherwise.
     */
    bool eq_ci_wcbuf(const wchar_t * datL, size_t sizeL) const;

    /*! Get pointer to data of \p this. */
    const uchar* data() const { if (m_sizeL < 5) { return &m_datL.bq[0]; } else { return m_datL.ptr; } }
    uchar* data() { if (m_sizeL < 5) { return &m_datL.bq[0]; } else { return m_datL.ptr; } }

    /*! Get \em Bit of \p this at index \p i. */
    uchar get_bit(bix i = bix(0)) const {
        enforce_gte(bitsize(), i+1);
        auto j = i.get();
        return pnw::get_bit(get_byte(j / 8), j % 8);
    }

    /// TODO: Templatize the following...
    /*! Get \em Byte of \p this at index \p i. */
    uchar get_byte(size_t i = 0) const {
        enforce_gte(bitsize(), 8*(i+1));
        return data()[i];
    }
    uint16_t get_i16ne(size_t i = 0) const {
        enforce_gte(bitsize(), sizeof(uint16_t)*(i+1));
        return bdec_u16ne(&data()[i]);
    }
    uint32_t get_i32ne(size_t i = 0) const {
        enforce_gte(bitsize(), sizeof(uint32_t)*(i+1));
        return bdec_u32ne(&data()[i]);
    }
    uint64_t get_i64ne(size_t i = 0) const {
        enforce_gte(bitsize(), sizeof(uint64_t)*(i+1));
        return bdec_u64ne(&data()[i]);
    }

    bool operator == (uchar a) const { return (is_byte(bitsize()) and
                                               get_byte(0) == a); }

    /*! Get Length of \p this in \em bytes. */
    size_t bytesize() const { return m_sizeL; }

    /*! Get Length of \p this in \em bits. */
    bix bitsize() const { return bix(8*bytesize()); }

    virtual bir sample_range() const { auto sz = bitsize(); return bir(sz, sz); }

    /*! Get \em Content Hash.
     * \return pointer to internal hash result if any, nullptr otherwise.
     */
    virtual const uchar * get_chash(chash::chashid hid = chash::CHASH_SHA2_256) const;

    /*! literal can only have one instance. */
    virtual size_t complexity(bir mults = bir::full()) const { return 1; }

    virtual csc rand(bir ssr = bir::full()) const;

    /// Tokenize/Lexicalize Pattern, that is tag it as \em atomic (in the context of its programming language).
    Lit * tokenize() { m_token = true; return this; }

/* ---------------------------- Group Separator ---------------------------- */

    virtual Base::Skips8& intersect_skips(Skips8& skips) const;

    virtual std::ostream& show(std::ostream& os) const;

protected:
    pHit match_cbuf (const char* const buf, size_t len, pMode pmm = MATCH_PARTIAL) const;
    virtual pHit match_in_local(const char * buf, size_t len,
                                bir roi = bir::full()) const;

protected:
    void init(const uchar * datL, size_t sizeL) {
        m_token = false;
        m_sizeL = sizeL;
        if (sizeL) {            /* if non-empty literal */
            if (sizeL < 5) {    /* if data fits into 4 bytes */
                /* pack the literal into a quad and avoid an extra allocation */
                for (size_t i = 0; i < sizeL; i++) {
                    m_datL.bq[i] = datL[i];
                }
            } else {                    /* if data is larger than 4 bytes */
                m_datL.ptr = new uchar[sizeL]; // heap-allocate
                memcpy(m_datL.ptr, datL, sizeL); // copy it
            }
        } else {
            PERR("Makes no sense defining an empty literal!\n");
            m_datL.ptr = nullptr;
        }
    }
private:
    size_t m_sizeL;               ///< Literal Data Size in bytes.
    union {
        uchar   bq[4];  ///< Byte Quad (8, 16, 24 or 32-bit literal)
        uchar * ptr;    ///< Buffer Literal.
    } m_datL;
    bool m_token:1;
    //PLIT_ENCODING_t m_enc;      ///< Character Encoding.
};

class Seq;

namespace gen {

inline Lit* lit(char x) { return new Lit(x); }
inline Lit* lit(const char * x) { return new Lit(x); }
inline Lit* lit(const char * x, size_t sizeL) { return new Lit(reinterpret_cast<const uchar*>(x), sizeL); }
inline Lit* lit(const uchar * x, size_t sizeL) { return new Lit(x, sizeL); }

inline Lit* lit(char x, csc name) {
    Lit* plit = new Lit(x);
    plit->set_name(name);
    return plit;
}

inline Lit* lit(const char * x, csc name) {
    Lit* plit = new Lit(x);
    plit->set_name(name);
    return plit;
}
inline Lit* lit(const char * x,
                const char * name,
                const char * doc) {
    Lit* plit = new Lit(x);
    if (name) { plit->set_name(name); }
    if (doc) { plit->set_name(doc); }
    return plit;
}
inline Lit* lit_U16be(const uint16_t x) { return new Lit(x, CPU_BE); }
inline Lit* lit_U16le(const uint16_t x) { return new Lit(x, CPU_LE); }
inline Lit* lit_U32be(const uint32_t x) { return new Lit(x, CPU_BE); }
inline Lit* lit_U32le(const uint32_t x) { return new Lit(x, CPU_LE); }

/*! Lookup Literal from \p x. */
Lit* lit_r(const csc& x);

/*! Lookup Literal from \p x. */
inline Lit* lit_r(const char * x) { return lit_r(csc(x)); }

/*! Lookup Literal from \p x. */
inline Lit* lit_r(const char x)   { return lit_r(csc(&x, 1)); }

Seq * lit_full(const char * nameL, size_t sizeL = 0);

/* ---------------------------- Group Separator ---------------------------- */

/*! Create \em Keyword Literal. */
Seq * keyword(char x);
Seq * keyword(const char * x);

/*! Create \em Operator Literal. */
template<class T> inline Lit * oplit(const T& x) {
    auto lit = new Lit(x);
    lit->tokenize();
    return lit;
}

/*! Create \em Operator Literal. */
template<class T, class U> inline Lit * oplit(const T& x,
                                              const U& y) {
    auto lit = new Lit(x, y);
    lit->tokenize();
    return lit;
}

}
}
}
