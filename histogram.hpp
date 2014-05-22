/*! \file histogram.hpp
 * \brief Calculations of Histograms (Mid-Value).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \todo Add iostream functor inspired by
 * http://www.dreamincode.net/code/snippet2626.htm with an internal state that
 * keeps track of max bin to assert in advance that bin overflows will
 * not occur given the size of \p x.
 *
 * \see http://stackoverflow.com/questions/8836388/generic-stl-compatible-histogram-in-c11
 * \todo Simplify when static_if becomes avaiable.
 * \todo Specialize to Sliding and Saturated Histogram
 * \todo Support Boost.Accumulator.
 *
 * \todo Hybrid using Virtual Classes Dense/Sparse U 8/16/32/64 used in
 * \todo Hybrid Version
 */

#pragma once
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <vector>
#include <array>
#include "type_traits_x.hpp"
#include "cc_features.h"
#include "saturate.hpp"
#include "algorithm_x.hpp"
#include "range.hpp"

namespace pnw { namespace histogram {

/*! Histogram Count Adjustment Type. */
typedef enum {
    CADJ_SATURATE,          ///< Saturate.
    CADJ_SLIDE,             ///< Sliding Normalization.
} CADJ_t;                   ///< Normalization.
static const CADJ_t CADJ_DEFAULT = CADJ_SLIDE;

#define SEPCH ("#")

/*! Prepare Size of Dense Histogram.
 * \todo Use m_map std::array<T,std::numeric_limits<V>::max()+1>
 *       for dense variant of unsigned integral types uint8_t, uint16_t
 */
template<class V, class H>
inline void prepare_dense(typename std::enable_if<std::is_integral<V>::value, H>::type & c) {
    c.resize(std::numeric_limits<V>::max()-
             std::numeric_limits<V>::min() + 1); // pre-allocate
}

/*! \name Recalculate Bin Count Limits. */
/// \{
template<class V, class C = size_t>
inline range<C> recalc_limits(const std::unordered_map<V,C>& h) {
    typedef typename std::unordered_map<V,C>::value_type P;
    auto l = std::minmax_element(begin(h), end(h), [](const P& a, const P& b) { return (a.second < b.second) ; });
    return range<C>(l.first->second,
                    l.second->second);
}
template<class C = size_t>
inline range<C> recalc_limits(const std::vector<C> & h) {
    auto l = std::minmax_element(begin(h), end(h));
    return range<C>(*l.first,
                    *l.second);
}
template<std::size_t N, class C = size_t>
inline range<C> recalc_limits(const std::array<C,N> & h) {
    auto l = std::minmax_element(begin(h), end(h));
    return range<C>(*l.first,
                    *l.second);
}

template<class V, class C = size_t, class D = size_t>
inline range<C> recalc_limits(const std::unordered_map<V,std::pair<C,D>>& h) {
    typedef typename std::unordered_map<V,C>::value_type P;
    auto l = std::minmax_element(begin(h), end(h), [](const P& a, const P& b) { return (a.second.first < b.second.first); });
    return range<C>(l.first->second.first,
                    l.second->second.first);
}
template<class C = size_t, class D = size_t>
inline range<C> recalc_limits(const std::vector<std::pair<C,D>>& h) {
    typedef std::pair<C,D> P;
    auto l = std::minmax_element(begin(h), end(h), [](const P& a, const P& b) { return (a.first < b.first); });
    return range<C>(*l.first.first,
                    *l.second.first);
}
template<std::size_t N, class C = size_t, class D = size_t>
inline range<C> recalc_limits(const std::array<std::pair<C,D>,N>& h) {
    typedef std::pair<C,D> P;
    auto l = std::minmax_element(begin(h), end(h), [](const P& a, const P& b) { return (a.first < b.first); });
    return range<C>(*l.first.first,
                    *l.second.first);
}

/// \}

/*! \name Garbage Collection. */
/// \{
template<class V, class C = size_t>
inline void map_gc(std::unordered_map<V,C>& map) {
    for (auto it = begin(map); it != end(map);) {
        if (it->second == 0) { it = map.erase(it); } else { ++it; }
    }
}
template<class C = size_t>                inline void map_gc(std::vector<C>& hmap) {} // do nothing for now
template<class C = size_t, std::size_t N> inline void map_gc(std::array<C,N>& hmap) {} // do nothing for now

template<class V, class C = size_t, class D = size_t>
inline void map_gc(std::unordered_map<V,std::pair<C,D>>& map) {
    for (auto it = begin(map); it != end(map);) {
        if (it->second.first == 0) { it = map.erase(it); } else { ++it; }
    }
}
template<class C = size_t, class D = size_t>                inline void map_gc(std::vector<std::pair<C,D>>& hmap) {} // do nothing for now
template<std::size_t N, class C = size_t, class D = size_t> inline void map_gc(std::array<std::pair<C,D>,N>& hmap) {} // do nothing for now
/// \}

/*! Generic Histogram.
 * \tparam V is Value Type.
 * \tparam C is Bin Count Type.
 * \tparam H is Value to Bin Count Container Mapper, typically a vector (dense) or unordered_map (sparse).
 */
template<class V,
         class C = size_t,
         class H = std::unordered_map<V,C>
         >
class generic
{
    friend class tree;
public:
    typedef typename H::value_type B; ///< Bin Type
public:
    generic(CADJ_t cadj_ = CADJ_DEFAULT) : m_cadj(cadj_) {}
    template<class It>
    generic(const It begin, const It end, CADJ_t cadj_ = CADJ_DEFAULT) : m_cadj(cadj_) { add(begin, end); }
    template<class Co> generic(const Co& x, CADJ_t cadj_ = CADJ_DEFAULT) : m_cadj(cadj_) { add_elements(x); }

    /*! Update Bin Count Limits. */
    void update_limits() const { if (m_map.empty()) { m_crange.clear(); } else { recalc_limits(m_map); } }

    /*! \name Get Bin Count of Bin Value \p e. */
    /// \{
    static C& count_of(C& e) { return e; }
    static C& count_of(typename std::unordered_map<V,C>::value_type& e) { return e.second; }
    /// \}

    /*! \name Normalization. */
    /// \{
    /*! Divide \em All Bin Values by \p n. */
    void div(uint n) {
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
        for (auto& e : m_map) { count_of(e) /= n; }
#else
        std::for_each(begin(m_map), end(m_map), [n](B& e) { count_of(e) /= n; });
#endif
    }
    /*! Normalize \em All Bin Value by dividing with \p n. */
    void normalize(uint n) {
        if (std::is_integral<C>::value) { // normalize bin counts of \em integral types only, not floating points
            div(n);
            m_crange.low() /= n;
            m_crange.high() /= n;
            gc();
        }
    }
    /// \}

    /*! \name Garbage Collection. */
    /// \{
    /*! Garbage Collect by removing empty bins. TODO: Move to sparse. */
    void gc() { if (pnw::is_min(m_crange.low())) { map_gc(m_map); update_limits(); } }
    /// \}

    // Number of elements that can be safely registered before bin count \em saturation occurrs.
    size_t count_capacity() const { return std::numeric_limits<C>::max() - m_crange.high(); }

    /*! \name Bin Count Increase. */
    /// \{
    void ninc_at(const V& e) { m_map[e]++; }
    void sinc_at(const V& e) { m_map[e] = sinc(m_map[e]); } // we might aswell do saturated arithmetic to allow \c C to have few bits, \todo replace with find and emplace?
    /// \}

    /*! \name Update Range Limits from Value \p e. */
    void update_range(const V& e) {
        m_crange.low() = std::min(m_crange.low(), m_map[e]);
        m_crange.high() = std::max(m_crange.high(), m_map[e]);
    }

    /*! \name Add Element(s). */
    /// \{
protected:
    /*! Normal Add Value \p e. */
    void nadd(const V& e) { ninc_at(e); update_range(e); }
    /*! Normal Add Values at [\p begin, \p end[. */
    template<class It> It nadd(It begin, const It end) { std::for_each(begin, end, [this](const V& e) { this->nadd(e); }); return end; }
    /*! Normal Add \p count Number of Values at \p begin. */
    template<class It> It nadd(It begin, size_t count) { return nadd(begin, begin+count); }

    /*! Saturated Add Value \p e. */
    void sadd(const V& e) { sinc_at(e); update_range(e); }
    /*! Saturated Add Values at [\p begin, \p end[. */
    template<class It> It sadd(It begin, const It end) { std::for_each(begin, end, [this](const V& e) { this->sadd(e); }); return end; }
    /*! Saturated Add \p count Number of Values at \p begin. */
    template<class It> It sadd(It begin, size_t count) { return sadd(begin, begin+count); }
public:
    /*! Add Value \p e. */
    H& add(const V& e) {
        if      (m_cadj == CADJ_SLIDE)    { nadd(e); }
        else if (m_cadj == CADJ_SATURATE) { sadd(e); }
        return m_map;
    }
    /*! Add Values in [\p begin, \p end[. */
    template<class It> H& add(It begin, const It end) {
        if (m_cadj == CADJ_SLIDE) {
            while (begin != end) {
                auto count = std::min<size_t>(end-begin, count_capacity());
                begin = nadd(begin, begin + count); // forward iterate
                if (pnw::is_max(m_crange.high())) { normalize(2); } // half range
            }
        } else if (m_cadj == CADJ_SATURATE) {
            begin = sadd(begin, end);
        }
        return m_map;
    }
    /*! Add Elements in Forward Access Container \p x . */
    //template<class Co> H& add(const typename std::enable_if<std::is_class<Co>::value, Co>::type& x) { return add(begin(x), end(x)); }
    template<class Co> H& add_elements(const Co& a) { return add(begin(a), end(a)); }
    /// \}

public:
    /*! Get Histogram Map. */
    H& get() { return m_map; }
    /*! Get Histogram Map. */
    const H& get() const { return m_map; }
    /*! Get Count Adjustment Type. */
    CADJ_t cadj() const { return m_cadj; }
    /*! Get Count Adjustment Type. */
    CADJ_t& cadj() { return m_cadj; }
protected:
    H                m_map;     ///< Maps Values to Bins.
private:
    mutable range<C> m_crange;  ///< Bin Count \em Range (Minimum and Maximum Value).
    //mutable range<C> m_cbounds; ///< Bin Count \em Bounds (Allowed Minimum and Maximum Value).
    CADJ_t           m_cadj;    ///< Bin Count \em Normalization Type.
};

/*! Generic Data Histogram.
 * \tparam V is Value Type.
 * \tparam C is Bin Count Type.
 * \tparam D is Bin Data (Node) Type.
 * \tparam H is Value to Bin Count Container Mapper, typically a vector (dense) or unordered_map (sparse).
 */
template<class V,
         class C = size_t,
         class D = size_t,
         class H = std::unordered_map<V,std::pair<C,D> >
         >
class d_generic
{
    friend class tree;
public:
    typedef typename H::value_type B; ///< Bin Type
public:
    d_generic(CADJ_t cadj_ = CADJ_DEFAULT) : m_cadj(cadj_) {}
    template<class It>
    d_generic(const It begin, const It end, CADJ_t cadj_ = CADJ_DEFAULT) : m_cadj(cadj_) { add(begin, end); }
    template<class Co> d_generic(const Co& x, CADJ_t cadj_ = CADJ_DEFAULT) : m_cadj(cadj_) { add_elements(x); }

    /*! Update Bin Count Limits. */
    void update_limits() const { if (m_map.empty()) { m_crange.clear(); } else { recalc_limits(m_map); } }

    /*! \name Get Bin Count of Bin Value \p e. */
    /// \{
    static C& count_of(std::pair<C,D>& e) { return e.first; }
    static C& count_of(typename std::unordered_map<V,std::pair<C,D>>::value_type& e) { return e.second.first; }
    /// \}

    /*! \name Normalization. */
    /// \{
    /*! Divide \em All Bin Values by \p n. */
    void div(uint n) {
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
        for (auto& e : m_map) { count_of(e) /= n; }
#else
        std::for_each(begin(m_map), end(m_map), [n](B& e) { count_of(e) /= n; });
#endif
    }
    /*! Normalize \em All Bin Value by dividing with \p n. */
    void normalize(uint n) {
        if (std::is_integral<C>::value) { // normalize bin counts of \em integral types only, not floating points
            div(n);
            m_crange.low() /= n;
            m_crange.high() /= n;
            gc();
        }
    }
    /// \}

    /*! \name Garbage Collection. */
    /// \{
    /*! Garbage Collect by removing empty bins. TODO: Move to sparse. */
    void gc() { if (pnw::is_min(m_crange.low())) { map_gc(m_map); update_limits(); } }
    /// \}

    // Number of elements that can be safely registered before bin count \em saturation occurrs.
    size_t count_capacity() const { return std::numeric_limits<C>::max() - m_crange.high(); }

    /*! \name Bin Count Increase. */
    /// \{
    void ninc_at(const V& e) { m_map[e].first++; }
    void sinc_at(const V& e) { m_map[e].first = sinc(m_map[e].first); } // we might aswell do saturated arithmetic to allow \c C to have few bits, \todo replace with find and emplace?
    /// \}

    /*! \name Update Range Limits from Value \p e. */
    void update_range(const V& e) {
        m_crange.low() = std::min(m_crange.low(), m_map[e].first);
        m_crange.high() = std::max(m_crange.high(), m_map[e].first);
    }

    /*! \name Add Element(s). */
    /// \{
protected:
    /*! Normal Add Value \p e. */
    void nadd(const V& e) { ninc_at(e); update_range(e); }
    /*! Normal Add Values at [\p begin, \p end[. */
    template<class It> It nadd(It begin, const It end) { std::for_each(begin, end, [this](const V& e) { this->nadd(e); }); return end; }
    /*! Normal Add \p count Number of Values at \p begin. */
    template<class It> It nadd(It begin, size_t count) { return nadd(begin, begin+count); }

    /*! Saturated Add Value \p e. */
    void sadd(const V& e) { sinc_at(e); update_range(e); }
    /*! Saturated Add Values at [\p begin, \p end[. */
    template<class It> It sadd(It begin, const It end) { std::for_each(begin, end, [this](const V& e) { this->sadd(e); }); return end; }
    /*! Saturated Add \p count Number of Values at \p begin. */
    template<class It> It sadd(It begin, size_t count) { return sadd(begin, begin+count); }
public:
    /*! Add Value \p e. */
    H& add(const V& e) {
        if      (m_cadj == CADJ_SLIDE)    { nadd(e); }
        else if (m_cadj == CADJ_SATURATE) { sadd(e); }
        return m_map;
    }
    /*! Add Values in [\p begin, \p end[. */
    template<class It> H& add(It begin, const It end) {
        if (m_cadj == CADJ_SLIDE) {
            while (begin != end) {
                auto count = std::min<size_t>(end-begin, count_capacity());
                begin = nadd(begin, begin + count); // forward iterate
                if (pnw::is_max(m_crange.high())) { normalize(2); } // half range
            }
        } else if (m_cadj == CADJ_SATURATE) {
            begin = sadd(begin, end);
        }
        return m_map;
    }
    /*! Add Elements in Forward Access Container \p x . */
    //template<class Co> H& add(const typename std::enable_if<std::is_class<Co>::value, Co>::type& a) { return add(begin(a), end(a)); }
    template<class Co> H& add_elements(const Co& a) { return add(begin(a), end(a)); }
    /// \}

public:
    /*! Get Histogram Map. */
    H& get() { return m_map; }
    /*! Get Histogram Map. */
    const H& get() const { return m_map; }
    /*! Get Count Adjustment Type. */
    CADJ_t cadj() const { return m_cadj; }
    /*! Get Count Adjustment Type. */
    CADJ_t& cadj() { return m_cadj; }
protected:
    H                m_map;     ///< Maps Values to Bins.
private:
    mutable range<C> m_crange;  ///< Bin Count \em Range (Minimum and Maximum Value).
    //mutable range<C> m_cbounds; ///< Bin Count \em Bounds (Allowed Minimum and Maximum Value).
    CADJ_t           m_cadj;    ///< Bin Count \em Normalization Type.
};

/*! Dense Histogram.
 * \tparam V is Value Type.
 * \tparam C is Bin Count Type.
 * \tparam H is Value to Bin Count Container Mapper, typically a vector (dense) or unordered_map (sparse).
 */
template<class V,
         class C = size_t,
         class H = typename std::vector<C>
         >
class dense : public generic<V,C,H>
{
public:
    dense(CADJ_t cadj_ = CADJ_DEFAULT) : generic<V,C,H>(cadj_) { prepare_dense<V,H>(this->get()); }
public:
    template<class Co> dense(const Co& x, CADJ_t cadj_ = CADJ_DEFAULT)
        : generic<typename Co::value_type,C,H>(cadj_) {
        prepare_dense<V,H>(this->get());
        this->add_elements(x);
    }
    template<class It> dense(const It begin, const It end, CADJ_t cadj_ = CADJ_DEFAULT)
        : generic<typename It::value_type,C,H>(cadj_) {
        prepare_dense<V,H>(this->get());
        this->add(begin, end);
    }
    void div_element(C& e, uint n) { e /= n; }
    void reset() { std::fill(this->get(), 0) ; }
};

/*! Partially Specialize Dense Histogram for 8-Bit Unsigned Integers using Statically Stored Bins in an std::array.
 * \tparam C is Bin Count Type.
 */
template<class C>
class dense<uint8_t, C, std::array<C, 256> > : public generic<uint8_t, C, std::array<C, 256> >
{
public:
    typedef uint8_t V;
    typedef std::array<C, 256> H;
public:
    dense(CADJ_t cadj = CADJ_DEFAULT) : generic<V,C,H>(cadj) { reset(); }
    template<class Co> dense(const Co& x, CADJ_t cadj_ = CADJ_DEFAULT)
        : generic<typename Co::value_type,C,H>(cadj_) {
        this->reset(); this->add_elements(x);
    }
    template<class It> dense(const It begin, const It end, CADJ_t cadj_ = CADJ_DEFAULT)
        : generic<typename It::value_type,C,H>(cadj_) {
        this->reset(); this->add(begin, end);
    }
    void div_element(C& e, uint n) { e /= n; }
    void reset() { std::fill(this->get(), 0) ; }
};

/*! Sparse Histogram.
 * \tparam V is Value Type.
 * \tparam C is Bin Count Type.
 */
template<class V, class C>
class sparse : public generic<V,
                              C,
                              std::unordered_map<V,C>
                              >
{
public:
    typedef std::unordered_map<V,C> H;
    sparse(CADJ_t cadj = CADJ_DEFAULT) : generic<V,C,H>(cadj) {}
    template<class Co> sparse(const Co& x, CADJ_t cadj_ = CADJ_DEFAULT)
        : generic<typename Co::value_type,C,H>(x, cadj_) { this->add_elements(x); }
    template<class It> sparse(const It begin, const It end, CADJ_t cadj_ = CADJ_DEFAULT)
        : generic<typename It::value_type,C,H>(begin, end, cadj_) { this->add(begin, end); }
    void reset() { this->get().clear(); }
};

#if 0
/*! Equalize Histogram.
 * \see [IPAMV] page 61.
 */
inline void
equalize(uchar *out, const uchar *in, size_t n)
{
    const size_t len = UINT8_MAX + 1;
    int hist[len];
    /* construct in-hist */
    intarray_zeros(hist, len);
    for (size_t i = 0; i < n; i++) {
        hist[in[i]]++;
    }
    /* form cumulative hist */
    for (size_t i = 1; i < len; i++) {
        hist[i] += hist[i - 1];
    }
    uchar tab[256];
    /* normalize */
    for (size_t i = 0; i < len; i++) {
        tab[i] = (double) hist[i] / n * UINT8_MAX;
    }
    /* transform image */
    uchararray_lookup(out, in, n, tab);
}
#endif

}}
