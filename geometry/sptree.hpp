/*! \file sptree.hpp
 * \brief N-Dimensional Binary Space Partitioning (BSP) Tree.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-07-27 23:24
 *
 * \see http://stackoverflow.com/questions/2725674/can-my-loop-be-optimized-any-more
 * \see http://arborjs.org/docs/barnes-hut
 * \see http://beltoforion.de/barnes_hut/barnes_hut_en.html
 * \see https://docs.google.com/viewer?a=v&pid=sites&srcid=bmlybWF1bmkuYWMuaW58Y3VkYWNvZGVzfGd4OjU2ZDY4ZmI5ZjRhNTVhYzQ
 * \see http://rosettacode.org/wiki/K-d_tree
 */

#pragma once
#include <stdexcept>
#include <algorithm>
#include <array>
#include <boost/serialization/access.hpp>
#include "box.hpp"
#include "particle.hpp"
//#include "../memory_x.hpp"
#include "../ostream_x.hpp"

namespace pnw {
namespace ntree {

/*! Abstract Node. */
template<class T, size_t N>
class node {
public:
    typedef vec<T,N> V;         ///< Vector alias
    typedef particle<T,N> P;    ///< Particle alias

    virtual ~node() {};

    /// \em Insert Value Reference \p a.
    virtual node& insert(const P& a, bool do_restat = true) = 0;
    /// \em Insert Value Pointer \p a.
    node& insert(const P* a, bool do_restat = true) { enforce(a); return insert(*a, do_restat); }
    /// \em Insert \p n Number of Value References at \p a.
    virtual node<T,N>& insert_n(const P** a, size_t n, bool do_restat = true) = 0;

    /// \em Insert \p l values at \p a.
    node& insert_n(const P* a, size_t l, bool do_restat = true) {
        size_t i = 0;
        std::for_each(a, a+l, [&i, this, do_restat](const P& e){
                this->insert(e, do_restat);
                i++;
            } );
        return *this;
    }
    // node& insert_n(const P** a, size_t l, bool do_restat = true) {
    //     size_t i = 0;
    //     std::for_each(a, a+l, [&i, this, do_restat](const P& e){
    //             this->insert(e, do_restat);
    //             i++;
    //         } );
    //     return *this;
    // }

    /// \em Insert all values \p a.
    template<size_t M> node& insert(const std::array<      P,M>&  a, bool do_restat = true) { return insert_n(a.data(), a.size(), do_restat); }
    template<size_t M> node& insert(const std::array<const P*,M>& a, bool do_restat = true) { return insert_n(a.data(), a.size(), do_restat); }

    node& insert(const std::vector<      P>&  a, bool do_restat = true) { return insert_n(a.data(), a.size(), do_restat); }
    node& insert(const std::vector<const P*>& a, bool do_restat = true) { return insert_n(a.data(), a.size(), do_restat); }

    virtual std::ostream& show(std::ostream& os, uint indent = 0, const char* name = nullptr) const = 0;
};

template<class T, size_t N, bool AdaptDistribution> class branch;

/*! Leaves Node containing \c M Particle References. */
template<class T, size_t N, size_t M = 4>
class leaves : public node<T,N> {
public:
    typedef vec<T,N> V;         ///< Vector alias
    typedef particle<T,N> P;    ///< Particle alias

    virtual ~leaves() {};

    leaves(const P& p0) {
        init(); m_refs[0] = &p0;
    }
    leaves(const P& p0,
           const P& p1) {
        if (M >= 2) { init(); }
        m_refs[0] = &p0;
        m_refs[1] = &p1;
    }
    leaves(const P& p0,
           const P& p1,
           const P& p2,
           const P& p3) {
        if (M >= 4) { init(); }
        m_refs[0] = &p0;
        m_refs[1] = &p1;
        m_refs[2] = &p2;
        m_refs[3] = &p3;
    }

    void init() { std::fill(begin(m_refs), end(m_refs), nullptr); }

    /// \em Insert Value \p a.
    virtual node<T,N>& insert(const P& a, bool do_restat = true) override {
        auto hit = std::find(begin(m_refs), end(m_refs), nullptr);
        if (hit == end(m_refs)) {     // if all positions filled
            auto br = new branch<T,N,true>(a);
            for (auto e : m_refs) { br->insert(*e); } // br->insert(m_refs);
            delete this;
            return *br;
        } else {
            *hit = &a;
            return *this;
        }
    }
    virtual node<T,N>& insert_n(const P** a, size_t n, bool do_restat = true) override {
        return *this;
    }

    virtual std::ostream& show(std::ostream& os, uint indent = 0, const char* name = nullptr) const override {
        if (name) { os << name << ": "; }
        os << "refs≡" << m_refs << "" << std::endl;
        return os;
    }
protected:
    std::array<const P*,M> m_refs;         ///< Particle reference.
};

/*! Branch Node.
 *
 * \tparam AdaptDistribution Adapt Assumption of points distribution to being centered
 *  around first point inserted. Otherwise assume uniform distribution of
 *  inserted points when calculating bounding-boxes upon creation of internal sub-nodes.
 *
 * \todo For CPUs with more Cores \c N_CPUS than \c N_SUBS split top branch into \c N_CPUS.
 */
template<class T, size_t N, bool AdaptDistribution>
class branch : public node<T,N> {
public:
    typedef vec<T,N> V;         ///< Vector alias
    typedef box<T,N> B;         ///< Box alias
    typedef particle<T,N> P;    ///< Particle alias

    /*! Number of Sub-Spaces.
     * In 2-D: Quadrants.
     * In 3-D: Octants.
     */
    static constexpr size_t N_SUBS = 1 << N;

    /// \em Default Construct.
    branch() { init_subs(); }

    /// TODO: Make these variadic on number of particles and make m_crossing (mass_center(a....))
    branch(const P& p0)
        : m_bbox(p0.pos),
          m_crossing(p0.pos) { init_subs(); insert(p0); }
    branch(const P& p0, const P& p1)
        : m_bbox(bounding_box(p0.pos, p1.pos)),
          m_crossing(mean(p0.pos, p1.pos)) { init_subs(); insert(p0, p1); }
    branch(const V& crossing, const P& p0)
        : m_bbox(p0.pos),
          m_crossing(crossing) { init_subs(); insert(p0); }

    /// \em Point-Cloud \p points_ Construct.
    template<class C> branch(const C& pc) : branch() {
        init_subs();
        restat(pc);
        m_crossing = mass_center();
        node<T,N>::insert(pc, false); // insert them but no need to update statistics
    }

    /// Get Center of Mass.
    V mass_center() const { return m_mass_center_sum / m_total_mass; }

    /// Update Statistics According to \em Point-Cloud \p points_ Construct.
    template<class C> branch& restat(const C& pc) {
        for (const P& p : pc) {
            m_bbox = unite(m_bbox, p.pos);
            m_total_mass += p.mass;
            m_mass_center_sum += p.mass*p.pos;
        }
        return *this;
    }

    /// \em Copy Construct.
    branch(const branch& rhs) = delete;
    /// \em Move Construct.
    branch(const branch&& rhs) = delete;

    void init_subs() { std::fill(m_subs, m_subs+N_SUBS, nullptr); }

    /// \em Destruct.
    virtual ~branch() {
        std::for_each(m_subs, m_subs+N_SUBS,
                      [](node<T,N>*& sub) {
                          if (sub) { delete sub; }
#ifndef NDEBUG
                          sub = nullptr; // be extra careful in debug mode
#endif
                      } );
    }

    B& bbox() { return m_bbox; }
    const B& bbox() const { return m_bbox; }

    /// \em Insert Value \p a.
    virtual node<T,N>& insert(const P& a, bool do_restat = true) override {
        // switch (N) {
        // case 1: {
        //     const uint tix = a[0] < m_crossing[0] ? 0 : 1; // index
        //     return m_subs[tix] ? m_subs[tix]->insert(a) : *(m_subs[tix] = new branch(a));
        //     break;
        // }
        // case 2: {
        //     const uint ix0 = a[0] < m_crossing[0] ? 0 : 1; // index
        //     const uint ix1 = a[1] < m_crossing[1] ? 0 : 1; // index
        //     const uint tix = ix0 + ix1*2;
        //     return m_subs[tix] ? m_subs[tix]->insert(a) : *(m_subs[tix] = new branch(a));
        //     break;
        // }
        // case 3: {
        //     const uint ix0 = a[0] < m_crossing[0] ? 0 : 1; // index
        //     const uint ix1 = a[1] < m_crossing[1] ? 0 : 1; // index
        //     const uint ix2 = a[2] < m_crossing[2] ? 0 : 1; // index
        //     const uint tix = ix0 + ix1*2 + ix2*4;
        //     return m_subs[tix] ? m_subs[tix]->insert(a) : *(m_subs[tix] = new branch(a));
        //     break;
        // }
        // case 4: {
        //     const uint ix0 = a[0] < m_crossing[0] ? 0 : 1; // index
        //     const uint ix1 = a[1] < m_crossing[1] ? 0 : 1; // index
        //     const uint ix2 = a[2] < m_crossing[2] ? 0 : 1; // index
        //     const uint ix3 = a[3] < m_crossing[3] ? 0 : 1; // index
        //     const uint tix = ix0 + ix1*2 + ix2*4 + ix3*8;
        //     return m_subs[tix] ? m_subs[tix]->insert(a) : *(m_subs[tix] = new branch(a));
        //     break;
        // }
        // default: break;
        // }

        // TODO: Delegate call to iterative non-member function instead of this recursive one.
        uint tix = 0;                               // total subspace index
        V new_sub_crossing;
        for (size_t d = 0; d < N; d++) {           // for each dimension \c d
            const uint ix = a.pos[d] < m_crossing[d] ? 0 : 1; // index
            if (not AdaptDistribution) {
                new_sub_crossing[d] = m_bbox.l(d) + (static_cast<T>(0.25) +
                                                     ix * static_cast<T>(0.5)) * m_bbox.dim(d);
            }
            tix += ix << d;
        }
        auto& sub = m_subs[tix]; // sub-node pointer reference

        if (sub) {
            sub = &sub->insert(a);
        } else {
            return *(sub = new leaves<T,N>(a));
            // if (AdaptDistribution) {
            //     return *(sub = new branch(a));
            // } else {
            //     return *(sub = new branch(new_sub_crossing, a));
            // }
        }

        if (do_restat) {
            bbox() = unite(bbox(), a.pos);
            m_total_mass += a.mass;
        }
        return *this;
    }
    virtual node<T,N>& insert_n(const P** a, size_t n, bool do_restat = true) override {
        return *this;
    }

    /// \em (De)Serialization (Encode/Decode).
    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) {}

    // Show
    virtual std::ostream& show(std::ostream& os, uint indent = 0,
                               const char* name = nullptr) const override {
        if (name) { os << name << ": "; }
        os << "(bb≡" << this->bbox()
           << " c≡" << this->m_crossing
           << ")" << std::endl;
        size_t i = 0;
        std::for_each(this->m_subs, this->m_subs+N_SUBS, [&os, indent, &i](const node<T,N>* sub) {
                if (sub) {
                    os_spaces(os, indent+1);
                    std::cout << "-S#" << i << ": ";
                    sub->show(os, indent+1);
                }
                i++; } );
        return os;
    }
    friend std::ostream& operator << (std::ostream& os,
                                      const branch<T,N, AdaptDistribution>& a) { return a.show(os); }

protected:
private:
    B m_bbox = B::unite_unit(); ///< Bounding Box.
    V m_crossing;               ///< Crossing Point.
    V m_mass_center_sum = 0; ///< Sum of center of masses of contained particles.
    T m_total_mass = 0; ///< Total mass of contained particles.
    node<T,N>* m_subs[N_SUBS]; ///< Sub-Trees. TODO: Use std::unique_ptr<node> m_subs[N_SUBS].
};

#if HAVE_CXX11_TEMPLATE_ALIASES
template<class T, class I, bool AdaptDistribution> using bbranch = branch<T,1,AdaptDistribution>; ///< Binary Branching Node.
template<class T, class I, bool AdaptDistribution> using qbranch = branch<T,2,AdaptDistribution>; ///< Quad Branching Node.
template<class T, class I, bool AdaptDistribution> using obranch = branch<T,3,AdaptDistribution>;  ///< Oct Branching Node.
#endif

}
}
