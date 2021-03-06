/*! \file pmatch.hpp
 * \brief Base of Pattern Matching (Scanning/Searching) Logic.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \keywords pattern,matching,scanning,searching,finding,context
 */

#pragma once
#include <algorithm>
#include "../utils.h"
#include "../MATCH_enum.h"
#include "../csc.hpp"
#include "../saturate.hpp"
#include "../bix.hpp"
#include "../range.hpp"

/*! Pattern Matching Index Distance (Error) in \em Bits.
 *
 * Can be either:
 * - Number of \em bytes that \em didn't match.
 * - Other Distance Measure: 0 upon full match, >= 0 otherwise, where
 *   an
 *   - unmatched byte adds 16 (bits)
 *   - misplaced byte adds 8 (bits)
 */
typedef uint64_t idist;

// TODO: Support std::numeric_limits.
#define IDIST_NO_MATCH (UINT64_MAX)
#define IDIST_FULL (0)

/*! Pattern Matching Return (Status)
 *
 * Number of Sub-Patterns that were matched. A sub-pattern can be
 * either a
 * - Bit
 * - Byte
 * - Literal (Atom/Terminal)
 * - Sub-Sequences
 */
typedef int32_t pmret_t;

// TODO: Support std::numeric_limits.
#define PMRET_undefined_ (0)
#define PMRET_NONE (INT32_MAX)

/*! Pattern Match \em Hit. */
class pHit : public bir {
public:
    pHit()
        : bir(bix::min(), bix::min()), m_dist(IDIST_FULL) {}
    pHit(bix off, bix len, idist dist = IDIST_FULL)
        : bir(off, off+len), m_dist(dist) {}
    pHit(bir roi, idist dist = IDIST_FULL)
        : bir(roi), m_dist(dist) {}
    pHit(bir roi, bool hit)
        : bir(roi), m_dist(hit ? IDIST_FULL : IDIST_FULL) {}

    /// Get \em Range.
    // bir& range() { return m_range; }
    // bir range() const { return m_range; }

    /// Get \em Offset in Bits.
    const bix get_offset() const { return low(); }
    /// Get \em Length in Bits.
    const bix bitlength() const { return bitsize(); }
    /// Get \em Distance (Error). Used in \c MATCH_FUZZY.
    const idist get_dist() const { return m_dist; }

    /// Set \em Offset to \p off.
    pHit& set_offset(bix off) { low() = off; return *this; }
    /// Set \em Length to \p len.
    pHit& set_length(bix len) { high() = low() + len; return *this; }
    /// Set \em Distance to \p dist.
    pHit& set_dist(idist dist) { m_dist = dist; return *this; }
    /// Set All Values.
    pHit& change(bix off, bix len, idist dist) {
        set_low(off);
        set_high(off + len);
        m_dist = dist;
        return *this;
    }

    /// Reset.
    pHit& zero() { return change(bix::min(), bix::min(), IDIST_FULL); }

    /// Undefine.
    pHit& undefine() { return change(bix::inf(), bix::min(), IDIST_NO_MATCH); }
    /// Check if \em defined.
    bool defined() const { return (bir::defined() and
                                   m_dist == IDIST_NO_MATCH); }

    /// Append \p hit.
    void app(const pHit& hit) {
        high() += hit.bitsize();
        m_dist = sadd(m_dist, hit.m_dist); /* add to distance sum */
    }

    /// Return true if \c this hit is \em full.
    bool full() const { return (m_dist == 0); }
    /// Defaul to \em full hit.
    operator bool() const { return full(); }

    friend pHit append(const pHit& a, const pHit& b) {
        return pHit(a.get_offset(),
                    a.bitlength() +
                    b.get_offset() + b.bitlength(),
                    a.get_dist() + b.get_dist());
    }

    // Print to \p os.
    std::ostream& show(std::ostream& os) const
    {
        os << "off:" << low() << ' '
           << "len:" << bitsize() << ' '
           << "dist:" << m_dist;
        return os;
    }

private:
    idist m_dist;       ///< \em Distance (Error). Used in \c MATCH_FUZZY.
};

/// Construct Hit att \p off of length one byte if \p hit is set, otherwise none.
inline pHit pHit1(bix off, bool hit)
{
    return pHit(off,
                hit ? off+static_cast<bix>(8) : off,
                IDIST_NO_MATCH + (hit ? 0 : 8));
}

/*! Pattern Matching \em Mode. */
class pMode {
public:
    pMode(MATCH_t mT = MATCH_PARTIAL,
          bool l_ci = false, bool l_usi = false, bool l_syni = false, bool l_woi = false)
        : m_mT(mT), m_ci(l_ci), m_usi(l_usi), m_syni(l_syni), m_woi(l_woi) {}

    // case_relax()
    // underscore_space_relax()
    // synonym_relax()
    // word_order_relax()

    /// Get \em Case Insensitivity State
    bool ci() const { return (m_ci); }
    /// Get \em Underscore-Space Insensitivity State
    bool usi() const { return (m_usi); }
    /// Get \em Synonyms Insensitivity State
    bool syni() const { return (m_syni); }
    /// Get Word \em Order Insensitivity State
    bool woi() const { return (m_woi); }
    /// Get \em Precise State.
    bool precise() const { return not (ci() or usi() or syni() or woi()); }

    MATCH_t get_type() const { return m_mT; }

    /*! Toggle \em Case Sensitivity. */
    bool toggle_case() { return (m_ci = not m_ci); }

    /*! Cycle Backward to \em Previous Genre. */
    void cycle_previous() { if (m_mT == MATCH_first) { m_mT = MATCH_last; } else { m_mT--; } }

    /*! Cycle Forward to \em Next Genre. */
    void cycle_next() { if (m_mT == MATCH_last) { m_mT = MATCH_first; } else { m_mT++; } }

    const char * name() const;
private:
    MATCH_t m_mT:3;            ///< Literal Matching \em Genre Type:
    bool     m_ci:1;            ///< \em Case Insensitive Flag:
    bool     m_usi:1; ///< \em Underscore-Space Insensitive Flag: Underscore and Space are treated as equal.
    bool     m_syni:1; ///< \em Synonyms Insensitive Flag: Synonyms are treated as equal.
    /*!
     * \em Word \em Order Insensitive Flag - Word order does not matter.
     *
     * Example: When searching for \c create_file() and \c file_create()
     * will match.
     */
    bool m_woi:1;
};

/*! Search Mode flag - Prefix (0), Substring (1) or Fuzzy (2). */
extern pMode g_search_pmc;	///< Start with substring match.

/*! Search Hit Flag.
 * 1 if a new hit was found, 0 if current hit found again, -1 if no hit found.
 */
extern bool g_search_hit_flag;

/*! Offset of Recent Hit.
 * >= 0 if defined, -1 otherwise.
 */
extern int g_search_hit_off;

/*! Length of Recent Hit.
 * >= 0 if defined, -1 otherwise.
 */
extern int g_search_hit_len;

#if 0
/*! File Byte Offset \em Iterator. */
typedef struct {
    bix       posF;             ///< File (Iteration) Position.
    bix       sizeF;            ///< Size of file.
    uint8_t * bufF;             ///< Buffer of file.
} pmIter;
inline void pmiter_init_empty(pmIter * pmiter) {
    pmiter->posF = 0;
    pmiter->sizeF = 0;
    pmiter->bufF = nullptr;
}
inline void pmiter_set(pmIter * pmiter, bix posF, bix sizeF) {
    pmiter->posF = posF;
    pmiter->sizeF = sizeF;
    pmiter->bufF = nullptr;
}
inline void pmiter_clear(pmIter * pmiter) {
    pmiter->posF = 0;
    pmiter->sizeF = 0;
    free(pmiter->bufF); pmiter->bufF = nullptr;
}
#endif

/*!
 * Search for \p key of length \p keyL in the char array \p dat of
 * length \p datL.
 *
 * \param[in] key Pattern to search for.
 * \param[in] keyL Length of Pattern to search for.
 * \param[in] pmm Kind of Pattern Matching.
 * \return number of matching characters.
 */
pHit
cbuf_find_LMATCH(const char * dat, size_t datL,
                 const char * key, size_t keyL,
                 pMode pmm = MATCH_PARTIAL);

pHit
cstr_find_LMATCH(const char * dat,
                 const char * key, size_t keyL,
                 pMode pmm = MATCH_PARTIAL);

pHit
csc_find_LMATCH(const csc& dat, const csc& key,
                pMode pmm = MATCH_PARTIAL);
