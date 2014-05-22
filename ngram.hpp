/*! \file ngram.hpp
 * \brief n-gram.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-10-28 18:24
 *
 * \note Adjacent Elements on a specific Level can be regarded as aliases and close elements as synonmys.
 *
 * \see https://en.wikipedia.org/wiki/N-gram
 * \see http://www.newser.com/story/107766/google-database-tracks-popularity-of-500b-words.html
 *
 * \todo Reuse histogram container from histogram.hpp
 * \todo Can we make use of STL algorithms similar to std::for_each in \c sub_log and \c seq_log?
 * \todo Add Support for N-Grams in Boost.Accumulator.
 */

#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include "memory_x.hpp"
#include "saturate.hpp"
#include "histogram.hpp"

namespace pnw { namespace histogram { namespace ngram {

/*! N-gram Tree of Histograms.
 * \tparam V is Value Type.
 * \tparam C is Count (Bin) Type.
 * \tparam L is Level (Depth) Type.
 */
template<class V, class C = size_t, class L = uint8_t>
class tree {
public:
    typedef d_generic<V, C, std::unique_ptr<tree>> KMap; // maps Value \c V to \c Bin of (Histogram Sub-Tree)
    typedef std::pair<C, std::unique_ptr<tree> > Bin; // Sub-Tree
public:
    // Construct an \p n levels deep N-gram from elements in [\p begin, \p end].
    template<class It> tree(const It begin, const It end, L n, CADJ_t cadj = CADJ_DEFAULT)
        : m_map(cadj) { seq_log(begin, end, n); }
    // Construct an \p n levels deep N-gram from elements in Container \p c.
    template<class Co> tree(const typename std::enable_if<std::is_class<Co>::value, Co>::type& c, L n, CADJ_t cadj = CADJ_DEFAULT)
        : m_map(cadj) { seq_log(c, n); }

    /*! Construct (sub) tree at \p level.
     * \note Must be public because its called from \c std::make_unique.
     */
    tree(L level = 0, CADJ_t cadj = CADJ_DEFAULT) : m_map(cadj), m_level(level) {}

    /// Check if Empty.
    bool empty() const { return m_map.get().empty(); }
    L level() const { return m_level; }
    const KMap& map() const { return m_map; }

protected:
    // Log \p value and return its sub-tree.
    tree& log(const V& value) {
        auto hit = m_map.get().find(value); // find value
        if (hit == m_map.get().end()) { // if value not found
            m_map.get()[value] = Bin(1, std::make_unique<tree>(m_level+1, static_cast<CADJ_t>(m_map.cadj())));
            return *m_map.get()[value].second.get();
        } else {
            hit->second.first = sinc(hit->second.first); // increment bin counter
            return *hit->second.second.get();
        }
    }

    /*! Sub-Log All Elements in [begin end]. */
    template<class It> void sub_log(const It begin, const It end) {
        if (begin != end) {
            auto next = begin+1;
            auto& sub_tree = log(*begin); // log and return its sub tree
            if (next != end) { // if \c next level
                sub_tree.sub_log(next, end); // recurse
            }
        }
    }
    /*! Log All \em Consecutive \p N-Tuples in Sequence from \p begin to \p end at n-gram level \p level.
     * \complexity[time] O((n+1) * (end-begin))
     */
    template<class It> void seq_log(const It begin, const It end, L n) {
        for (auto it = begin; it != end; ++it) {
            sub_log(it, std::min(it + n + 1, end));
        }
    }

    template<class Container> void seq_log(const Container& cont, uint n = 0) { seq_log(begin(cont), end(cont), n); }

private:
    KMap m_map;                 ///< Histogram Bins.
    L m_level;                  ///< N-Gram Level.
};

}}}
