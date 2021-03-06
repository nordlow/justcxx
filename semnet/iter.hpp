/*!
 * \file iter.hpp
 * \brief Iterator.
 */

#pragma once
#include "PMAGIC_enum.h"
#include "patt.hpp"

//#include <unordered_set>
//#include <nedtries/nedtrie.h>

/* ========================================================================= */

namespace semnet { namespace patterns {

class PattHits;

/*!
 * SemNet Traversal (Visiting Order) Type Code.
 */
typedef enum {
    PTRAV_BREADTH_FIRST_,
    PTRAV_BREADTH_FIRST_FORWARD,   ///< \em Breadth-First Forward
    PTRAV_BREADTH_FIRST_REVERSE,   ///< \em Breadth-First Reverse

    PTRAV_DEPTH_FIRST_,
    PTRAV_DEPTH_FIRST_FORWARD,  ///< \em Depth-First Forward
    PTRAV_DEPTH_FIRST_REVERSE, ///< \em Depth-First Reverse
} __attribute__ ((packed)) PTRAV_t;
/*!
 * Merge with this.
 */
typedef enum {
    PSCAN_ORDER_FILETYPE,         ///< Scan for \em all patterns of \em same \em type first.
    PSCAN_ORDER_FILESIZE,         ///< Scan for patterns in \em Small / \em Large files first.
    PSCAN_ORDER_MAGICSIZE,        ///< Scan for \em Simple / \em Complex patterns first.
} PSCAN_ORDER_t;

#if 0
/*! Filtering Iterator.
 *
 * Use to scan \c RegFile. This results in a set of \c PattHits
 * related to as \c CHILDs. Use the \c PARENT - \c Base for each \c PattHit to find its type.
 */
class Iter: public ObMr {
public:
    virtual OB_t get_type() const { return OB_SCAN; };

    /// New Scan for \c Ob of type \p soT.
    Iter(Ob * begin,
         REL_t rM[], size_t rM_N,
         OB_t soT = OB_any_,
         PTRAV_t itT = PTRAV_BREADTH_FIRST_FORWARD,
         Base * patt = nullptr,
         size_t depth = std::numeric_limits<size_t>::max())
        : ObMr(), m_begin(begin), m_curr(begin), m_rM_N(rM_N), m_soT(soT), m_patt(patt), m_depth(depth) {
        m_rM = new REL_t[rM_N];
        std::copy(rM, rM + rM_N, m_rM);
    }

    virtual ~Iter() {
        if (m_rM) { delete[] m_rM; }
    }

    Iter * clone(size_t depth = 0) { return new Iter(m_begin, m_rM, m_rM_N, m_soT, m_itT, m_patt); }

    /*!
     * \em Perform Scan and return resulting set of hits.
     * \p overlap Hits may Overlap if \c true
     * \return set of hits
     */
    Obs perform(bool overlap = false);

    OB_t get_target_type() const { return m_soT; }
    Ob* get_current() const { return m_curr; }

    virtual std::ostream& show(std::ostream& os) const {
        return os << " soT:" << OB_getName(m_soT) << std::endl;
    }

    Obs next() {
        // TODO: Update \c m_curr, depth.
        return Obs();
    }

protected:
private:
    Ob*           m_begin;      ///< Begin Object.
    Ob*           m_curr;       ///< Current Object.

    REL_t*        m_rM;         ///< Relations to iterate over.
    size_t        m_rM_N;       ///< Number of relations.

    OB_t          m_soT;        ///< Relation types to iterate through.
    PTRAV_t       m_itT;        ///< Iteration Traversal type.
    pMode         m_pmc;        ///< Pattern Matching Mode.
    //std::unordered_set<Ob*> m_visited;    ///< Visited (Tagged) Objects. Enables iterating \em non-directed (\em cyclic) \c Ob - \c Rel - graphs.
    //nedtries::trie_map<Ob*> m_visited; ///< Visited (Tagged) Objects. Enables iterating \em non-directed (\em cyclic) \c Ob - \c Rel - graphs.
    Base*        m_patt;       ///< Pattern.

    size_t        m_depth;      ///< Scanning Depth.
};

namespace gen {

/*! Create New Scan for \c Ob of type \p soT, optionally matching pattern @p
 *  ppattA. */
inline Iter* iter(Ob * begin,
                  REL_t rM[], size_t rM_N,
                  OB_t soT = OB_any_,
                  PTRAV_t itT = PTRAV_BREADTH_FIRST_FORWARD,
                  Base * patt = nullptr,
                  size_t depth = std::numeric_limits<size_t>::max())
{
    Iter * piter = new Iter(begin, rM, rM_N, soT, itT, patt, depth); g_oreg->add1(piter);
    return piter;
}

/*! Create New Scan for \c Ob of type \p soT, optionally matching pattern @p
 *  ppattA. */
inline Iter* iter(Ob * begin,
                  REL_t rM1,
                  OB_t soT = OB_any_,
                  PTRAV_t itT = PTRAV_BREADTH_FIRST_FORWARD,
                  Base * patt = nullptr,
                  size_t depth = std::numeric_limits<size_t>::max())
{
    Iter * piter = new Iter(begin, &rM1, 1, soT, itT, patt, depth); g_oreg->add1(piter);
    return piter;
}

}

#endif

}
}
