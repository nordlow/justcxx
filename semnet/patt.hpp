/*! \file patt.hpp
 * \brief semnetDB Pattern Specifier.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "ob.hpp"
#include <wchar.h>
#include "../cbufn.h"
#include "../encode.h"
#include "pmatch.hpp"
#include "../endian_utils.h"
#include "../fkind.h"
#include "../dfmt.h"
#include "../udunits.hpp"
#include <iosfwd>
#include <unordered_set>
#include <bitset>
// #include <boost/dynamic_bitset.hpp>
#include <memory>
#include <algorithm>

#define HAVE_GHT_HASH_TABLE_H (1) /* Always present now that we include libghthash in this directory. */
#include "../libghthash/src/ght_hash_table.h" /* Note: Use local variant. */

/* ---------------------------- Group Separator ---------------------------- */

namespace semnet {

namespace filesystem {
class File;
class RegFile;
}

namespace patterns {

/*!
 * Pattern (Hit) Data \em Interpretation (Meaning (Semantics), Format, Decoding).
 *
 * Note: Highly \em Hit \em Context \em Sensitive and therefore
 * contained of \c Hit.
 *
 * Use for \em efficient encoding and decoding of hits in (Intellisens)
 * preparsed file metadata.
 *
 * \see @related Closely related to \c PRPOS_t.
 */
typedef enum {
    PINTR_any_,               ///< Any Meaning.

    /* \name Tag/Token Type. */
    /// \{
    PINTR_ID,                   ///< Identifier

    PINTR_CLASS_,               ///< Class
    PINTR_CLASS_DECL,           ///< Class Declaration
    PINTR_CLASS_DEF,            ///< Class Definition
    PINTR_CLASS_INST,           ///< Class Instance

    PINTR_OP_,                  ///< Operator
    PINTR_OP_ARITHMETIC,        ///< Arithmetic Operator
    PINTR_OP_LOGICAL,           ///< Logical Operator
    PINTR_OP_BITWISE,           ///< Bitwise Operator

    PINTR_VAR_,                 ///< Variable
    PINTR_VAR_DECL,             ///< Variable Declaration
    PINTR_VAR_DEF,              ///< Variable Definition
    PINTR_VAR_REF,              ///< Variable Reference

    PINTR_LIT_,                 ///< Literal
    PINTR_LIT_NUM,              ///< Number Literal
    PINTR_LIT_STR,              ///< String Literal

    PINTR_FUN_,
    PINTR_FUN_DECL,             ///< Function Declaration
    PINTR_FUN_DEF,              ///< Function Definition
    PINTR_FUN_CALL,             ///< Function Call
    /// \}

    // Higher Level Real World Eigenname.
    PINTR_ENAME_CONTINENT,
    PINTR_ENAME_LAND,
    PINTR_ENAME_TOWN,
    PINTR_ENAME_HUMAN,
    PINTR_ENAME_ANIMAL,
    PINTR_ENAME_DOG,
    PINTR_ENAME_HORSE,

    PINTR_LANG_SV_WORD,
    PINTR_LANG_EN_WORD,
    PINTR_LANG_EN_BR_WORD,
    PINTR_LANG_EN_AM_WORD,

}  __attribute__ ((packed)) PINTR_t;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Pattern Instance Uniformity Type.
 */
typedef enum {
    PUNIFORMITY_NONE = 0,         ///< None.
    PUNIFORMITY_SIZE = 1,         ///< All of equal \em length.
    PUNIFORMITY_CONTENTS = 2,     ///< All of equal \em contents.
} __attribute__ ((packed)) PUNIFORMITY_t;

/* ---------------------------- Group Separator ---------------------------- */

class Hit;
class Super;

/*! \em Byte Pattern Specifier/Description.
 *
 * Specifies a \em raw \em logical data pattern in a \em finite
 * \em consecutive array of bytes (uchar*).
 */
class Base
{
    friend class Rep;
    friend class Seq;
    friend class Alt;
    friend class Ctx;
    friend class Wrap;
    friend class Multi;
public:
    typedef std::unordered_set<Super*> Supers; ///< Super Patterns.
    typedef std::bitset<256> Skips8; ///< Skips for 8-bit Values
    typedef std::bitset<65536> Skips16; ///< Skips for 16-bit values
    // TODO: Generalize to n-bits and instead use:
    //typedef boost::dynamic_bitset<size_t> SkipsN; ///< Skips for N-Bit Values

    Base();
    Base(const char* ame);
    Base(const csc& name) : Base(name.c_str()) {}
    Base(pHit (*cbuf_lmatchP)(const char* pbuf, size_t len, bix off, pMode pmm));

private:
    void init();

public:
    /// Copy Constructor.
    //Base(const Base& in) { return *this; }

    /// Destructor.
    virtual ~Base();

    /*! Try to Match \c this \p rep number of times in \p buf of
     *  length \p len.
     * \return Matching Count
     */
    pHit fixed_rep_lmatch_in(size_t rep,
                             const char* buf, size_t len,
                             bir roi = bir::full()) const;

    /*! Set Explicit Matcher Function to \p cbuf_lmatchP.     */
    Base * set_pmatch(pHit (*match)(const char* pbuf, size_t len,
                                    bix off, pMode pmm)) {
        m_cbuf_lmatchP = match; return this;
    }

    Base * set_dfmt(DFMT_t dfmt) { m_dfmt = dfmt; return this; }
    DFMT_t get_dfmt() const { return m_dfmt; }

    /*! Return true if \c this pattern is \em cyclic. */
    bool is_cyclic() const { return false; }

    /*! \em AutoDetect Suitable \em Matcher Function and set it to @c
     *  cbuf_lmatchP.     */
    virtual void auto_set_detected_pmatch() { };

    Base * set_name(csc name) { m_nameP.assign(name); return this; }
    Base * set_name(const char* name) { m_nameP.assign(name); return this; }
    Base * set_doc(const char* doc) { m_docP.assign(doc); return this; }

    friend int countSubDups(const Base * pA, const Base * pB);

    virtual void prepare() const;
    virtual void unprepare() const;

public:
    /*! Load \em Begin Lookup-table of 8-bit (byte) values (\c m_skips8)
     * for use in \em Search and \em Skip.

     * \return >= 1 if a new lookup-table was generated, 0 if already present.
     * This is currently only used \em internally by \c Base.
     */
    Skips8& load_skips8() const;
    void unload_skips8() const;

    /*! Get \em Skip-Table and or the result into \p skips.
     * For explanation see doc on member \c m_skips8.
     * \return number of bits that were set in \p skips.
     */
    virtual Base::Skips8& intersect_skips(Skips8& skips) const = 0;

public:
    /*! Get Range of Possible Length (in Bits) \em Instances. */
    virtual bir sample_range() const = 0;

    /// Check if \c this has \em fixed \em size.
    bool has_fixed_size() const { return sample_range().empty(); }

    /*! Get \em Instance \em Complexity, that is the number of different
     *  instance patterns that \c this can match.
     * \param[out] multV Instance Complexity Range
     */
    virtual size_t complexity(bir mults = bir::full()) const = 0;

    /*! Get \em Constant (Fixed Size and Constant Value) State. */
    bool is_constant() const { return complexity() == 1; }

    /*! Check if \c is a \em constant (fixed size and constant value) and in
     * that case return the constant value as a string, otherwise return an empty string.
    */
    csc constant() const { return is_constant() ? rand() : ""; }

    /*! Get a \em Random \em Instance of \c this limiting its size to \p ssr.
     * \param[in] ssr Sample Size Range
     * \return random instance.
     */
    virtual csc rand(bir ssr = bir::full()) const = 0;
public:
    /*! Generate \em All \em Possible \em Instances limiting its size to \p range.
     *
     * \param[in] numI Size of Instance
     * \param[out] ibufA_ret upon return contains a pointer to an array of pattern instances (of type char*)
     * \param[out] inum_ret  upon return contains a pointer to an array of pattern lengths (of type size_t)
     * \return number of generated instances, that is the array length of *ibufA_ret and *inum_ret.
     */
    //virtual bix all_samples(bix numI, char*** ibufA_ret, size_t** inum_ret) const { return 0; }

protected:
    /*! Match \c this with \p buf inside range \p roi.
     * Note: Virtual matcher that must be defined in sub-classes of \c Base.
     * \return Hit
     */
    virtual pHit match_in_local(const char* buf, size_t len,
                                bir roi = bir::full()) const = 0;

    /*! mutating_canonicalize (Standardize) \C This And return result (possibly replacing \c this). */
    virtual Base * mutating_canonicalize() { return this; }

public:
    /*! Match \c this with \em name of \p file.
     * \return Hit.
     */
    pHit match_name_of(const filesystem::File * file) const;

    /*! Match \c this with \em path of \p file.
     * \return Hit.
     */
    pHit match_path_of(const filesystem::File * file) const;

    /*! Match \c this with \em contents of file named \p pathF.
     * \return Hit
     */
    pHit match_contents_of(int fd,
                            off_t fsize = std::numeric_limits<off_t>::max(),
                            bir roi = bir::full()) const;

    /*! Match \c this with \em contents of \p file.
     * \return Hit
     */
    pHit match_contents_of(const filesystem::File * file,
                            bir roi = bir::full()) const;

public:
    /*! Match \c this in \p buf at offset \p off.
     * Note: Uses \em Specialized \em Optimized \em Matchers and @em Caches if possible.
     * \return Hit
     */
    pHit match_at(const csc& buf,
                  bix off = bix::min()) const {
        return match_in(buf.data(), buf.size(),
                        bir(off,
                            off+static_cast<bix>(8*buf.size())));
    }

    /*! Match \c this in \p buf at offset \p off.
     * \return Hit
     */
    pHit match_at(const char* buf, size_t len,
                  bix off = bix::min()) const {
        return match_in(buf,len,
                        bir(off, static_cast<bix>(0)));
    }

    /*! Match \c this in \p buf of length \p len inside range \p roi. */
    virtual pHit match_in(const char* buf, size_t len,
                          bir roi = bir::full()) const;
    /*! Match \c this in \p buf inside range \p roi. */
    pHit match_in(const csc& buf,
                  bir roi = bir::full()) const {
        return match_in(buf.data(), buf.size(), roi); }

    pHit match_in_special(const char* buf, size_t len,
                          bir roi = bir::full()) const;

public:
    /*! \name Use these to optimize sets of filters for example in Web Browser Adblocking.
     * Note: Requires \em canonicalization (standardization) of pattern (trees).
     */
    /* @{ */

    /*! Check if \c this is \em generalization of \p a. */
    bool generalizes(Base * a) const;

    /*! Check if \c this is \em specialization of \p a. */
    bool specializes(Base * a) const;

    /* @} */

    virtual const csc& get_pathL() const { return m_nameP; }

public:
    /*! Run Self-Checks on \c this.
     * Creates \p num instances of itself and matches itself on them.
     * \param[in] show flags for printing results
     * \param[in] range Sample Size Range
     * \return true upon successfull checks, false otherwise.
     */
    bool self_check(int num, bir roi, bool show = false) const;

    /*! Print Random Sample (Instance) of \c this to \p os. */
    void show_rand(std::ostream& os) const;

    /*! Print \c this to \p os.     */
    virtual std::ostream& show(std::ostream& os) const;

protected:
    /*! Add Super Pattern \p super (having \c this as sub). */
    bool add_super(Super * super) { return m_supers.insert(super).second; }

    /*! Remove Super Pattern \p super (having \c this as sub). */
    size_t remove_super(Super * super) {
        const size_t ret = m_supers.erase(super);
        if (m_supers.empty()) { // if not more super that references \c this
            delete this;        // forget it
        }
        return ret;
    }

    /// \em Move \p this supers to \c target.
    void move_supers_to(Base * target);

    /// \em Mutate \p this with \p target.
    Base * mutate_to(Base * target) { move_supers_to(target); delete this; return target; }

protected:
    /*! \name Specific Optimized Match and Transform (Expand/Reduce). */
    /* @{ */
    /**!
     * Explicit (Manually Written) Pattern \em Matcher function.
     * \param[in] pbuf Pattern \em Buffer.
     * \param[in] off Offset into \p pbuf.
     * \return Distance Measure: 0 upon exact match, >= 0 otherwise,
     *         where an
     *         - unmatched byte adds 2
     *         - misplaced byte adds 1
     */
    pHit (*m_cbuf_lmatchP)(const char* pbuf, size_t len,
                           bix off, pMode pmm);
    /* @} */

    mutable Supers m_supers; ///< Super Patterns, combining \c this with other patterns.

    mutable std::unique_ptr<CDigestF> m_ctdig; ///< Content Tree (Recursive) Digest (cache). nullptr means not defined.

private:
    csc m_nameP;                ///< \em Name if any.
    csc m_langP;                ///< \em Language if any.
    csc m_docP;                 ///< \em Documentation if any.

    DFMT_t   m_dfmt;            ///< Data \em Format.
    ud::Unit m_unit;            ///< Data \em Unit, defaulting to \c nullptr if \em dimensionless.

    /*! Begin \em Skip Lookup-Table when searching for 8-bit values (Bytes).
     *
     * Example: If bit at position \c i is set, we can skip all occurrencies of
     * \c i in source \c s when matching \c s with this pattern.
     *
     * This enables \em O(1) Time Complexity when \em skipping a set
     * of bytes (alternatives) that \c this pattern doesn't start with.
     *
     * Example: To quickly find next beginning of the \c Alt of \c Lit's:
     * \c int, \c short, \c float. In this case \c m_skips8 is of size \em 256
     * and all values except at indexes 'i', 's', 'f' should be zero.
     */
    mutable std::unique_ptr<Skips8> m_skips8;
};

namespace operations {
/*! Get case-folded/mixed \p a. */
extern Base* mixcase(Base* a);
/*! Get up-cased \p a. */
extern Base* upcase(Base* a);
/*! Get down-cased \p a. */
extern Base* dowcase(Base* a);
}
}
}
