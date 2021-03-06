/*!
 * \file rels.hpp
 * \brief Set or Sets of Relations (C pointers) to Ob structures.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "OB_enum.hpp"
#include "REL_enum.hpp"
#include "VATT_enum.hpp"
#include "../qsort_mt.h"
#include <vector>
#include <algorithm>

// Number of \c Refs elements that fit solely (pack) on the heap.
#define REFS_PNUM (2)

namespace semnet {

class Ob;

/*! Abstract Relation of type \c rT. */
class Rel {
    friend class Refs;
public:
    REL_t get_type() const { return m_rT; }
protected:
    Rel(REL_t rT = REL_any_) : m_rT(rT) {}
    virtual ~Rel() {}
    virtual size_t size() const = 0;
    Rel& set_type(REL_t rT) { m_rT = rT; return *this; }
    virtual Ob* nth(size_t i) = 0;
private:
    REL_t m_rT;                 ///< Type.
};

typedef std::vector<Rel*> Rels;

/*! Single-Destination Relation of type \c rT to \c rD. */
class SRel : public Rel {
    friend class Refs;
protected:
    SRel(REL_t rT, Ob* rD) : Rel(rT), m_rD(rD) {}
    virtual size_t size() const { return 1; };
    Ob * get_destination() const { return m_rD; }
    SRel& set_destination(Ob * rD) { m_rD = rD; return *this; }
    virtual Ob* nth(size_t i) { return i == 0 ? m_rD : nullptr; }
private:
    Ob* m_rD;                   ///< Destination.
};

/*!
 * Multi-Destination Relation of type \c rT to objects of type \c oT.
 */
class MRel : public Rel {
    friend class Refs;
public:
    MRel(REL_t rT, OB_t oT = OB_any_) : Rel(rT), m_oT(oT) {}
    MRel(REL_t rT, OB_t oT, Ob* rDA[], size_t rDN) : Rel(rT), m_oT(oT), m_rDA(rDA, rDA+rDN) {}
    virtual size_t size() const { return m_rDA.size(); };
    virtual Ob** data() { return m_rDA.data(); };
    virtual Ob* const* data() const { return m_rDA.data(); };
    bool insert(Ob* rD, bool unique_flag = false) {
        bool ret = false;
        if (unique_flag) {
            const auto hit = std::find(begin(m_rDA), end(m_rDA), rD);
            if (hit == end(m_rDA)) {
                m_rDA.push_back(rD); ret = true;
            }
        } else {
            m_rDA.push_back(rD); ret = true;
        }
        return ret;
    }
    virtual Ob* nth(size_t i) { return m_rDA[i]; }
private:
    OB_t m_oT;         ///< Object Type Filter specifying types of Destinations.
    std::vector<Ob*> m_rDA;     ///< Destination Pointers.
};

// ============================================================================

/*!
 * Set of References (Pointers) to semnetDB Objects (\c Ob).
 */
class Refs {
    friend class pReg;
    friend class ObMr;

    typedef Rels::iterator rel_it;        ///< Relation Iterator.
    typedef Rels::const_iterator rel_cit; ///< Relation Constant Iterator.

    /*!
     * Relation Destination (Target) Object Iterator.
     */
    class dest_it {
    public:
        dest_it(Rels * refs, size_t idxR = 0, size_t idxL = 0)
            : m_rels(refs), m_idxR(idxR), m_idxL(idxL) {}
        dest_it& operator++() {
            if (m_idxL+1 < m_rels[m_idxR].size()) { // next destination left in relation
                m_idxL++;
            } else {            // we need to switch to next relations
                if (m_idxR+1 < m_rels->size()) { // if more relations left
                    m_idxR++;       // next relation
                    m_idxL = 0;     // first destination
                }
            }
            return *this;
        }
        dest_it& operator--() {
            if (m_idxL == 0) { // previous destination left in relation
                m_idxL--;
            } else {            // we need to switch to next relations
                m_idxR++;       // next relation
                m_idxL = 0;     // first destination
            }
            return *this;
        }
        bool operator<(dest_it& rhs) { return (m_idxL < rhs.m_idxL); }
    private:
        Rels *m_rels;
        size_t m_idxR;          ///< Relation index.
        size_t m_idxL;          ///< Local Index.
    };

    typedef dest_it iterator; ///< Default Iterator.

public:
    /// Public Iterator Members
    // iterator begin() { return iterator(&m_rels, 0); }
    // iterator end() { return iterator(&m_rels, size(), m_rels[size()-1]->size()); }

public:
    Refs() { init(); }
    Refs(size_t room) { init(room); }

    Refs(const Refs& in);

    /*! Construct \c this of types \p rT to \p obs of length \p obs_N. */
    Refs(REL_t rT, Ob* const* obs, size_t obs_N) {
        init(); app(rT, obs, obs_N);
    }

    /*! Empty/Clear Set of References */
    void clear();

    ~Refs() { clear(); }

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Get Pointer to \em Destinations Array. */
    Ob** dataD() { return (m_num <= REFS_PNUM) ? &d.b.d[0] : d.m.d; }
    /*! Get Pointer to \em Types Array. */
    REL_t* dataT() { return (m_num <= REFS_PNUM) ? &d.b.t[0] : d.m.t; }

    /*! Get Pointer to \em Destinations Array. */
    Ob* const* dataD() const { return (m_num <= REFS_PNUM) ? &d.b.d[0] : d.m.d; }
    /*! Get Pointer to \em Types Array. */
    const REL_t* dataT() const { return (m_num <= REFS_PNUM) ? &d.b.t[0] : d.m.t; }

    /*! Get \em First Element Destination */
    Ob* firstD() { return (m_num <= REFS_PNUM) ? d.b.d[0] : d.m.d[0]; }

    /*! Get \em Last Element Destination */
    Ob*  lastD() { return (m_num <= REFS_PNUM) ? d.b.d[m_num-1] : d.m.d[m_num-1]; }

    //! Index Relation \em Destination at \p i.
    const Ob*  nthD(size_t i)       const { return (m_num <= REFS_PNUM) ? d.b.d[i] : d.m.d[i]; }
    const Ob*  operator[](size_t i) const { return nthD(i); }
          Ob*& nthD(size_t i)             { return (m_num <= REFS_PNUM) ? d.b.d[i] : d.m.d[i]; }
          Ob*& operator[](size_t i)       { return nthD(i); }

    //! Index Relation \em Type at \p i.
    const REL_t  nthT(size_t i) const { return (m_num <= REFS_PNUM) ? d.b.t[i] : d.m.t[i]; }
          REL_t& nthT(size_t i)       { return (m_num <= REFS_PNUM) ? d.b.t[i] : d.m.t[i]; }

    /* ---------------------------- Group Separator ---------------------------- */

    //! Clamped-Index \em Object Pointer at \p i using either \em wrap-around or \em edge-stop.
    const Ob* clamped_nthD(size_t i, bool cyclic_flag = false) const {
        const size_t n = m_num;
        if  (cyclic_flag) { if (i < 0) { i = n-1; } else if (i >= n) { i = 0; } } // wrap-around
        else              { if (i < 0) { i = 0;   } else if (i >= n) { i = n-1; } } // edge-stop
        return this->nthD(i);
    }
    Ob*& clamped_nthD(size_t i, bool cyclic_flag = false) {
        const size_t n = m_num;
        if  (cyclic_flag) { if (i < 0) { i = n-1; } else if (i >= n) { i = 0; } } // wrap-around
        else              { if (i < 0) { i = 0;   } else if (i >= n) { i = n-1; } } // edge-stop
        return this->nthD(i);
    }

    //! Clamped-Index \em Object Pointer at \p i using either \em wrap-around or \em edge-stop.
    const REL_t clamped_nthT(size_t i, bool cyclic_flag = false) const {
        const size_t n = m_num;
        if  (cyclic_flag) { if (i < 0) { i = n-1; } else if (i >= n) { i = 0; } } // wrap-around
        else              { if (i < 0) { i = 0;   } else if (i >= n) { i = n-1; } } // edge-stop
        return this->nthT(i);
    }
    REL_t& clamped_nthT(size_t i, bool cyclic_flag = false) {
        const size_t n = m_num;
        if  (cyclic_flag) { if (i < 0) { i = n-1; } else if (i >= n) { i = 0; } } // wrap-around
        else              { if (i < 0) { i = 0;   } else if (i >= n) { i = n-1; } } // edge-stop
        return this->nthT(i);
    }

    /* ---------------------------- Group Separator ---------------------------- */

    /// Get \em Number of \em Elements stored in container.
    size_t size() const { return m_num; };

    /* ---------------------------- Group Separator ---------------------------- */

    /// \em Resize Container to \p num number of \em Elements.
    void resize(size_t num, REL_t rT = REL_any_, Ob * rD = nullptr);

    /// \em Expand Container to \p num number of \em Elements.
    void expand(size_t num, REL_t rT = REL_any_, Ob * rD = nullptr);

    /// \em Truncate Container to \p num number of \em Elements.
    void truncate(size_t num);

    /* ---------------------------- Group Separator ---------------------------- */

    Refs& app(REL_t rT, Ob* rD);
    Refs& app(Ob* rD);

    /*!
     * Append several objects \p obs of length \p rN to \c this all
     * using the same \p rT, without checking for duplicate appends.
     */
    Refs& app(REL_t rT, Ob* const* rDA, size_t rN);

    /*!
     * Append several objects \p rDA of length \p rN to \c this as
     * \p rTA, without checking for duplicates.
     */
    Refs& app(const REL_t * rTA, Ob* const* rDA, size_t rN);

    /*! Append \p refs to \c this, without checking for duplicates.
     */
    Refs& app(const Refs& refs);
    Refs& operator += (const Refs& refs) { return app(refs); }

    Refs& appOs(Ob * const* rD, size_t rN, OB_t oT = OB_any_);

    /*! Append several objects \p b, without checking for duplicates. */
    Refs& app_non_unique(const Refs * b);

    /* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Pack \c this by removing all its zero references (empty slots).
     * \return number of empty slots that were removed.
     */
    size_t pack();

    /* ---------------------------- Group Separator ---------------------------- */

    size_t remRm(const REL_t * rM, size_t rM_N, Ob* rD_arg);
    size_t remRs(REL_t rT = REL_any_, Ob* rD_arg = nullptr) { return remRm(&rT, 1, rD_arg); }
    size_t remRs(Ob* rD_arg) { return remRs(REL_any_, rD_arg); }

    /* ---------------------------- Group Separator ---------------------------- */

    /*! \em Uniquify by removing Duplicates.
    * \return number of removed duplicates */
    size_t uniquify(REL_t rT = REL_any_, Ob* rD = nullptr);

    /* ---------------------------- Group Separator ---------------------------- */

    /*!
     * \em ReTarget Destinations - \em Replace <em>all references</em>
     *     to object \p from with \p to in \c this.
     * \return number of matching slots that were replaced.
     */
    size_t retarget(Ob* from, Ob* to);

    size_t retype(REL_t from, REL_t to);

    /*!
     * Search \c this for \p key.
     * \return index at \c this where first hit of \p key was found, or
     *         -1 if no hit.
     */
    ssize_t find(const Ob* key) const;
    ssize_t find(REL_t rT, const Ob* key) const;

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Sort \em All Elements first on type then on destination.
     * \return true if sorting was needed. */
    bool sort();

    /* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Calculate Statistics (Previous) Selections and their Interrelational
     * Properties, such as if - they are of the same type.
     * \see parr_investigate()
     */
    void calc_stat(bool * homo_flag_ret);


protected:
    /*! Initialize empty. */
    void init() {
        m_num = 0;
        d.m.room = 2;
        d.m.d = nullptr;
        d.m.t = nullptr;
        m_sorted = true;        // zero elements are trivially sorted
        m_packed = true;        // zero elements are trivially packed
    }

    /*! Initialize with \p num number of elements. */
    void init(size_t num) {
        m_num = 0;
        d.m.room = num;
        if (num > REFS_PNUM) {
            d.m.d = new Ob*[d.m.room];
            d.m.t = new REL_t[d.m.room];
        }
        m_sorted = true; // all of same relation and destination are always sorted
        m_packed = true; // all of same relation and destination are always packed
    }

    void expand_room_from_b_to_m(size_t room);
    void expand_room_in_m(size_t room);

    /*! Set \p num number of elements \p rD of types \p rT without any
     *  pre-allocation \em assuming \c this is of size \p num. */
    Refs& set(Ob* const rD[], const REL_t rT[], size_t num);

    /*!
     * Check how Registry \p preg is sorted.
     *
     * \return 1 if registry is sorted, 2 if registry is sorted but contains NULLs,
     *         0 if registry is not sorted at all.
     */
    int is_sorted_as(cmp_function_t * cmp);

    /*! Check if \c this is \em sorted. */
    bool is_sorted() const { return m_sorted; }

    /*! Check if \c this is \em packed. */
    bool is_packed() const { return m_packed; }

    /*! Check if \c only contains unique elements. */
    bool is_unique() const;

private:
    union {
        struct {
            Ob** d;       ///< Relation Destinations.
            REL_t* t;      ///< Relation Types.
            size_t room;    ///< Allocated number of Elements.
        } m;                 ///< General case for 3 or more elements.

        /*!
         * Optimization for 1 or 2 elements.
         *
         * This case is common in tree-structures such as file-systems
         * where each file has a relation to its parent and to its
         * kind/type.
         */
        struct {
            Ob* d[REFS_PNUM];   ///< Relation Destinations.
            REL_t t[REFS_PNUM]; ///< Relation Types.
        } b;
    } d;             ///< Data Union.
    size_t  m_num;              ///< Set Element \em Count. Note: For 0,1,2 and use d.r2 otherwise use d.rM
    bool m_sorted:1; ///< Flags that \c this is sorted (by pointer value).
    bool m_packed:1; ///< Flags that \c this is packed (no empty (nullptr) slots).
};

}
