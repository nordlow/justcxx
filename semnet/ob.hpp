/*!
 * \file ob.hpp
 * \brief Base Object.
 */

#pragma once
#include <iosfwd>
#include <vector>
#include "rels.hpp"
#include "OB_enum.hpp"
#include "REL_enum.hpp"
#include "VATT_enum.hpp"
#include "OP_enum.hpp"
#include "GOREL_enum.hpp"
#include "GODIR_enum.hpp"
#include "../chash.hpp"
#include "../VREC_enum.hpp"
#include "../sign.hpp"
#include "../stdio_x.h"

namespace semnet {

typedef chash::openssl::SHA1_160 CHashF;
//typedef chash::nettle::SHA1_160 CHashF;
typedef chash::CDigest160 CDigestF;
extern chash::chashid g_default_hid;

class pReg;
class Ob;
typedef std::vector<Ob*> Obs;

typedef std::pair<REL_t, Ob*> TRel;
typedef std::vector<TRel> TRels;

/* ========================================================================== */

typedef uint32_t tdepth_t;      ///< Tree Depth. Root has depth 0.
#define TDEPTH_MIN (std::numeric_limits<tdepth_t>::min()) ///< Tree Depth Minimum.
#define TDEPTH_MAX (std::numeric_limits<tdepth_t>::max()) ///< Tree Depth Maximum.
#define TDEPTH_undefined_ (TDEPTH_MAX+1) ///< Tree Depth Undefined.

typedef uint32_t theight_t;      ///< Tree Height. Leaf has depth 0. File has height 1 and File Attribute height 0.
#define THEIGHT_MIN (std::numeric_limits<theight_t>::min()) ///< Tree Height Minimum.
#define THEIGHT_MAX (std::numeric_limits<theight_t>::max()) ///< Tree Height Maximum.
#define THEIGHT_undefined_ (THEIGHT_MAX+1) ///< Tree Depth Undefined.

typedef uint64_t tsize_t;                ///< Tree (Content) Size in Bytes.
#define TSIZE_MIN (UINT64_MIN)  ///< Tree Size Minimum.
#define TSIZE_MAX (UINT64_MAX-1) ///< Tree Size Maximum.
#define TSIZE_undefined_ (TSIZE_MAX+1) ///< Tree Size Undefined.

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Object Manners - The way an object should talk to the end user.
 */
typedef enum {
    VTALK_FRUST,                  ///< Frustration.
    VTALK_REMIND,                 ///< Reminder.
    VTALK_WARNING,                ///< Warning.
    VTALK_ERROR,                  ///< Error.
} __attribute__ ((packed)) VTALK_t;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * URL Path Format.
 */
typedef enum {
    PPATH_FMT_LOCAL,              ///<       Local. Example: xorg.conf
    PPATH_FMT_TYPED_LOCAL,        ///< Typed Local: Example: file:xorg.conf
    PPATH_FMT_FULL,               ///<        Full. Example: /etc/X11/xorg.conf
    PPATH_FMT_TYPED_FULL,         ///<  Typed Full: Example: file:/etc/X11/xorg.conf
} __attribute__ ((packed)) PPATH_FMT_t;

/* ---------------------------- Group Separator ---------------------------- */

extern csc g_default_ob_name; ///< Default Object name.

/*! General Object.
 */
class Ob {
    friend class pReg;
public:
    Ob();
    virtual ~Ob();

    // \todo These should be put in sub-classes.
#if 0
    void* operator new[](size_t num) {
        void *ret = ::operator new[] (num);
        g_oreg->addM(ret);
        return ret;
    }
    void operator delete[](void *p) {
        g_oreg->remM(p);
        ::operator delete[] (p);
    }
#endif

    virtual OB_t get_type() const = 0;
    const char * get_typename(size_t* name_len_ret = nullptr) const {
        const char * name = OB_getName(get_type());
        if (name_len_ret) {
            *name_len_ret = strlen(name);
        }
        return name;
    }
    virtual bool match_type(OB_t oM1) const { return OB_matchS(get_type(), oM1); }

    int check_preg_rmagic() const;

    virtual int cmp_otype(const Ob * pB) const {
        return pnw::cmpsgn(get_type(), pB->get_type());
    }

    /* ---------------------------- Group Separator ---------------------------- */

    /// Visual object should return true.
    virtual bool is_visual() const { return false; }

    /* ---------------------------- Group Separator ---------------------------- */

    virtual bool is_tag() const;
    virtual bool   tag();
    virtual bool untag();
    bool set_tag(bool a) { return (a ? tag() : untag()); }

    /*!
     * Set all tags of \p obs to \p tag.
     */
    friend void parr_set_tag(Ob ** obs, size_t obs_N, bool tag);

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Get \em Lock Status.
     */
    virtual bool is_lock() const;

    /*! Return true upon succcessful, false if locking was not possible. */
    virtual bool   lock();

    /*! Return true upon was succcessful, false if already unlocked. */
    virtual bool unlock();

    bool set_lock(bool a) { return (a ? lock() : unlock()); }

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Get \em Obseletion Status.
     */
    virtual bool is_obselete() const;

    /*! Make \em Obselete.
     */
    virtual bool make_obselete();

    /* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Calculate Local Path of \p ob.
     * \return constant local path (\em pointer to internal storage,
     *         no explicit freeing needed)
     */
    virtual const csc& get_pathL() const { return g_default_ob_name; }

    /*!
     * Calculate Shortest Path to \p oT through relations of type \p rel.
     * \return 1 upon succcess, 0 otherwise, -1 on error.
     */
    virtual csc get_pathX(REL_t rel, OB_t oT, PPATH_FMT_t path_fmt) const;

    /*!
     * Calculate Parent Path along Parents (\c REL_PARENT) of \p ob.
     * If \p ob is a file or directory this means parent directory.
     * \param[out] path_ret is resulting path.
     * \return 1 upon succcess, 0 otherwise, -1 on error.
     */
    virtual csc get_pathP() const {
        return get_pathX(REL_PARENT, OB_DIR, PPATH_FMT_LOCAL);
        /* PNOTE("Incorrect?: This functions seems to return local path instead global parent path\n"); */
        /* if (ret == 1) { PNOTE("path_ret:%.*s\n", (int)path_ret.size(), path_ret.data()); } */
    }

    /*!
     * Calculate Full Path along Parents (\c REL_PARENT) of \p ob.
     * \param[out] path_ret is resulting path.
     * \return 1 upon succcess, 0 otherwise, -1 on error.
     */
    virtual csc path() const {
        return get_pathX(REL_PARENT, OB_DIR, PPATH_FMT_FULL);
    }

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Get \em Content (currently all but Paths, Names, Attributes) Hash.
     * \return pointer to internal hash result if any, nullptr otherwise.
     */
    virtual uint32_t get_chash_size() const { return chashid_get_digest_size(chash::CHASH_SHA2_256); }

    /* ---------------------------- Group Separator ---------------------------- */

    /*! \name Network \em Metrics Helpers.
     *
     * Metric can be distances, sizes, etc. Can be used Layout Algorithms.
     *
     * Obs: Important: Note: Overridden by sub-classes such as \c Dir
     * and \c File that \em caches these values to improve @em
     * performance when Quering File System (Tree) \em Statistics.
     */

    /// \{
    /// Get Depth Under Top PARENT of type \p oT.
    virtual tdepth_t get_tree_depth(OB_t oT = OB_any_) const;
    /// Get Height Over Bottom CHILD of type \p oT.
    virtual tdepth_t get_tree_height(OB_t oT = OB_any_) const;
    /// Get Span Under& Over CHILD of type \p oT.
    virtual tdepth_t get_tree_span(OB_t oT = OB_any_) const {
        return (get_tree_depth(oT) + get_tree_height(oT));
    }

    friend tdepth_t parr_get_tree_depth_max(Ob ** obs, size_t obs_N);
    friend theight_t parr_get_tree_height_max(Ob ** obs, size_t obs_N);
    /// \}

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Called after a relation of type \p rT was added. */
    virtual void post_relate_cb(REL_t rT);

    /* ---------------------------- Group Separator ---------------------------- */

    /*! Print. */
    virtual std::ostream& show(std::ostream& os) const;
    /*! Print.
     * \param[in] att_rel_flag bit 0 set if we want to see the attributes, bit 1 set if want to see relations aswell.
     */
    std::ostream& show(std::ostream& os, bool att_rel_flag) const {
        show(os);
        if (att_rel_flag) { show_relations(os); return os; }
        return os;
    }

    /*! Print to \p os.
     * \param[in] att_rel_flag bit 0 set if we want to see the attributes,
     * bit 1 set if want to see relations aswell.
    virtual std::ostream& show(std::ostream& os) const;
     */

    virtual std::ostream& show_relations(std::ostream& os) const { return os; }

    // Wrapper for \c print.
    friend std::ostream& operator << (std::ostream& os, const Ob& a) { a.show(os); return os; }

    /* ---------------------------- Group Separator ---------------------------- */

    virtual bool is_related_to(REL_t rel, Ob * pA) const;

    /* ---------------------------- Group Separator ---------------------------- */

    virtual bool has_parent(Ob * pA) const { return Ob::is_related_to(REL_PARENT, pA); };
    virtual bool has_child(Ob * pA) const { return Ob::is_related_to(REL_CHILD, pA); };
    virtual bool has_sibling(Ob * pA) const { return Ob::is_related_to(REL_SIBLING, pA); };

    /* ---------------------------- Group Separator ---------------------------- */

    virtual size_t net_get_relation_count() const = 0;
    bool net_is_related() const { return net_get_relation_count() != 0; }

    // Check if network from \c this is \em cyclic.
    virtual bool net_is_cyclic() const;

    /* ---------------------------- Group Separator ---------------------------- */

public:
    /*!
     * \em Single-Direction \em Unrelate \c this from all objects of
     * type \p rT, optionally only at \p rD.
     *
     * \return number of relations removed
     */
    virtual size_t net_remM(REL_t rT = REL_any_, const Ob * rD = nullptr) const = 0;

    // Uniquify Relations.
    virtual size_t net_uniquifyM(REL_t rT = REL_any_, const Ob * rD = nullptr) const = 0;

    /*! \em Single-Direction \em Relate \c this to \p rD as \p rT.
     *
     * \return number of relations added
     */
    void net_addS(REL_t rT, const Ob * rD, bool unique_flag = false) const { net_addM(rT, &rD, 1, unique_flag); }

    /*! \em Single-Direction \em Relate \c this to \p rDN number of
     *  Objects at \p rDA all as \p rT.
     */
    virtual void net_addM(REL_t rT, const Ob * const * rDA, size_t rDN, bool unique_flag = false) const = 0;

    virtual void net_addM(const REL_t * rTA, const Ob * const * rDA, size_t rDN, bool unique_flag = false) const = 0;
    virtual void net_addM(const MRel& a, bool unique_flag = false) const;
    virtual void net_addM(const Refs& a, bool unique_flag = false) const;

    virtual size_t net_retargetM(const Ob * from, const Ob * to) const = 0;

    virtual size_t net_retypeM(REL_t from, REL_t to) const = 0;

/* ---------------------------- Group Separator ---------------------------- */

    virtual size_t
    net_count_rMoM(const REL_t * rM, size_t rM_N,
                   const OB_t * oM, size_t oM_N) const = 0;

    virtual size_t
    net_count_rToT(REL_t rT, OB_t oT) const { return net_count_rMoM(&rT,1, &oT, 1); }

    virtual size_t
    net_count_Rs(const REL_t rT) const { return net_count_rMoM(&rT,1, nullptr,0); }

    /* ---------------------------- Group Separator ---------------------------- */

    virtual Ob *
    net_first(REL_t rT = REL_any_,
              OB_t oT = OB_any_,
              const Ob * rD = nullptr) const {
        return net_nth_rToTdT(rT, oT, rD);
    }

    virtual Ob *
    net_first(OB_t oT, const Ob * rD = nullptr) const {
        return net_nth_rToTdT(REL_any_, oT, rD);
    }

    virtual Ob *
    net_first(REL_t rT, const Ob * rD) const {
        return net_nth_rToTdT(rT, OB_any_, rD);
    }

    /*!
     * Return First Relation of type \p rT or \c REL_any_
     * to check for any relations at all and the destinating object.
     *
     * \param[in] rM Relation Type Mask
     * \param[in] rM_N Relation Type Mask length
     */
    virtual Ob *
    net_first(const REL_t * rM, size_t rM_N) const { return net_nth_rMoMdT(rM,rM_N); }

/* ---------------------------- Group Separator ---------------------------- */

    friend int
    net_connectS_check(REL_t rT0, const Ob * ob0,
                       REL_t rT1, const Ob * ob1)
    {
#ifndef NDEBUG
        if (not (ob0 and ob1)) { PERR("ob0 or ob1 is nullptr\n"); return -1; }
        // if (self_check and ob0 == ob1) { PERR("Cannot self-relate\n"); return -1; }
        REL_duals_check(rT0, rT1);
#endif
        return 0;
    }

    friend int
    net_connectM_check(REL_t rT0, const Ob * ob0,
                       REL_t rT1, const Ob * const* obs1, size_t obs1_N)
    {
#ifndef NDEBUG
        if (not (ob0 and obs1)) { PERR("ob0 or obs1 is nullptr\n"); return -1; }
        REL_duals_check(rT0, rT1);
        for (size_t i = 0; i < obs1_N;  i++) {
            if (obs1[i] == nullptr) { PERR("Some obs1[i] is nullptr\n"); return -1; }
            // if (self_check and ob0 == obs1[i]) { PERR("Cannot self-relate\n"); return -1; }
        }
#endif
        return 0;
    }

/* ---------------------------- Group Separator ---------------------------- */

    /*!
     * \name Connecting Single Objects.
     */

    /* @{ */

    /*!
     * Connect \p ob0 with \p ob1, that is
     * relate \p ob1 to (\p rT1-ed) \p ob0 and
     *        \p ob0 to (\p rT0-ed) \p ob1.
     * \param unique_flag is true if we should assert that connect must unique.
     * \return 1 if relations were set, 0 upon noop, -1 upon invalid arguments.
     */
    friend int
    net_connectS(REL_t rT0, const Ob * ob0,
                 REL_t rT1, const Ob * ob1,
                 bool unique_flag = false)
    {
        if (net_connectS_check(rT0, ob0, rT1, ob1) < 0) { return -1; }
        ob1->net_addS(rT0, ob0, unique_flag);
        ob0->net_addS(rT1, ob1, unique_flag);
        return 1;
    }

    /*!
     * Connect \p ob0 with \p ob1 at specific relation index, that is
     * relate \p ob1 to (\p rT1-ed) \p ob0 and
     *        \p ob0 to (\p rT0-ed) \p ob1.
     *
     * \return 1 if relations were set, 0 upon noop, -1 upon invalid arguments.
     */
    friend int
    net_connectS_At(REL_t rT0, Ob * ob0,
                    REL_t rT1, Ob * ob1, REL_t rM1, size_t nth1,
                    bool unique_flag = false)
    {
        if (net_connectS_check(rT0, ob0, rT1, ob1) < 0) { return -1; }
        ob1->net_addS(rT0, ob0, unique_flag);
        ob0->net_insAt(rT1, rM1, ob1, nth1, unique_flag);
        return 1;
    }

    /* @} */

    /*!
     * \name Connecting Multiple Objects.
     */

    /* @{ */

    /*!
     * Connect \p ob0 with \p obs1_N number of Obs in \p obs1, that is
     * relate \p obs1[i] to (\p rT1-ed) \p ob0 and
     *        \p ob0 to (\p rT0-ed) \p obs1[i].
     *
     * \return 1 if relations were set, 0 upon noop, -1 upon invalid arguments.
     */
    friend int
    net_connectM(REL_t rT0, Ob * ob0,
                 REL_t rT1, Ob ** obs1, size_t obs1_N,
                 bool unique_flag = false)
    {
        for (size_t i = 0; i < obs1_N; i++) {
#ifndef NDEBUG
            if (net_connectS_check(rT0, ob0, rT1, obs1[i]) < 0) { return -1; }
#endif
            obs1[i]->net_addS(rT0, ob0, unique_flag);
        }
        ob0->net_addM(rT1, obs1, obs1_N, unique_flag);
        return 1;
    }
    friend int
    net_connectM(REL_t rT0, const Ob * ob0,
                 REL_t rT1, const Ob * const * obs1, size_t obs1_N,
                 bool unique_flag = false)
    {
        if (net_connectM_check(rT0, ob0, rT1, obs1, obs1_N) < 0) { return -1; }
        for (size_t i = 0; i < obs1_N; i++) {
            obs1[i]->net_addS(rT0, ob0, unique_flag);
        }
        ob0->net_addM(rT1, obs1, obs1_N, unique_flag);
        return 1;
    }

    friend int
    net_connectM(REL_t rT0, Ob * ob0,
                 REL_t rT1, Obs& obs1,
                 bool unique_flag = false);

    int
    net_connectM_At(REL_t rT0, Ob * ob0,
                    REL_t rT1, Ob ** obs1, size_t obs1_N,
                    REL_t rM1, size_t nth1,
                    bool unique_flag = false)
    {
        for (size_t i = 0; i < obs1_N; i++) {
#ifndef NDEBUG
            if (net_connectS_check(rT0, ob0, rT1, obs1[i]) < 0) { return -1; }
#endif
            obs1[i]->net_addS(rT0, ob0, unique_flag);
        }
        ob0->net_insMAt(rT1, rM1, obs1, obs1_N, nth1, unique_flag);
        return 1;
    }

    /* @} */

/* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Disconnect relation of type \p rTA at \p obA from relation of
     * type \p rTB at \p obB.
     */

    /*!
     * \em Remove all the \p rTA-relations of \p obA.
     *
     * \param[in] rTA Relation Type. \c REL_any_ removes all kinds of relations.
     * \param[out] obB destination object to unrelate to or \c nullptr if all
     * should be removed.
     * \param[in] rTB inverse relation type. \c REL_any_ removes all
     *       kinds of relations.
     * \param[in] obs_flag is non-zero if deletion of relation destination
     *       object(s) should be performed.
     *
     * \return Number of Dependencies that could not be removed.
     */
    friend int
    net_disconnectS(REL_t rTA, const Ob * obA,
                    REL_t rTB, const Ob * obB,
                    bool obs_flag);

    /*!
     * \em Disconnect \em Multiple Relations of type \p rT to targets
     *     of type \p oT optionally to specific target \p rD, and
     *     optionally if \p obs_flag is true delete target \p rD
     *     aswell.
     */
    virtual size_t
    net_disconnectM(REL_t rT = REL_any_,
                    OB_t oT = OB_any_,
                    const Ob * rD = nullptr,
                    bool obs_flag = false) const = 0;

/* ---------------------------- Group Separator ---------------------------- */

    virtual Ob * net_nth_rMoMdT(const REL_t * rM, size_t rM_N,
                                const OB_t * oM = nullptr, size_t oM_N = 0,
                                const Ob * rD_arg = nullptr, size_t nth = 0) const = 0;

    Ob * net_nth_rToTdT(REL_t rT = REL_any_,
                        OB_t oT = OB_any_,
                        const Ob * rD_arg = nullptr, size_t nth = 0) const
    {
        return net_nth_rMoMdT(&rT,1, &oT,1, rD_arg, nth);
    }

    /*!
     * (Cyclically) \em Relatively \em Index at position \p idx relative to
     * position of relation type \p rT with destination \p curr.
     */
    // virtual Ob * net_nthr(REL_t rT, const Ob * curr, int idx, bool cyclic_flag = false);

/* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Find/Search/Filter/Collect/Gather Path along Object Relations starting at \p ob.
     * \param[in] ob Path Start.
     *
     * \param[in] rM Mask of Relations to follow.
     * \param[in] rM_N Mask Length of Relations to follow.
     *
     * \param[in] oM Mask of Object Types to follow.
     * \param[in] oM_N Mask Length of Object Types to follow.
     */
    virtual Obs
    net_find_rMoM(const REL_t * rM, size_t rM_N,
                  const OB_t * oM = nullptr, size_t oM_N = 0,
                  VREC_t vrec = VREC_none) const = 0;

    /*!
     * Find/Search/Filter/Collect/Gather Path along Object Relations starting at \p ob.
     *
     * \param[in] ob Path Start.
     *
     * \param[in] rM Mask of Relations to follow.
     * \param[in] rM_N Mask Length of Relations to follow.
     *
     * \param[in] oT Mask of Object Type to follow.
     */
    Obs
    net_find_rMoT(const REL_t * rM, size_t rM_N,
                  OB_t oT = OB_any_,
                  VREC_t vrec = VREC_none) const
    {
        return net_find_rMoM(rM, rM_N, &oT, 1, vrec);
    }

    /*!
     * Find/Search/Filter/Collect/Gather Path along Object Relations starting at \p ob.
     *
     * \param[in] ob Path Start.
     * \param[in] rT Mask of Relation to follow.
     * \param[in] oT Mask of Object Type to follow.
     */
    Obs
    net_find_rToT(const REL_t rT,
                  OB_t oT = OB_any_,
                  VREC_t vrec = VREC_none) const {
        return net_find_rMoT(&rT,1, oT, vrec);
    }

    /* ---------------------------- Group Separator ---------------------------- */

    /*!
     * \name Adding/Changing Object Relations.
     */

    /* @{ */

    /*!
     * Insert relation from \p ob of type \p rT to \p rD between
     * relations matching \p rT at indexes \p nth and \p nth+1.
     * \return non-zero if the relation was added, 0 otherwise.
     */
    int
    net_insAt(REL_t rT, REL_t rM1,
              Ob * rD, size_t nth,
              bool unique_flag = false);

    int
    net_insMAt(REL_t rT, REL_t rM1,
               Ob ** rDA, size_t rDN, size_t nth,
               bool unique_flag = false);

    /* @} */

    /* ---------------------------- Group Separator ---------------------------- */

    /*!
     * \name Indexing Object \p ob's Relations.
     */

    /*!
     * Lookup \p ob's idx:th relation and return it or nullptr of none was found.
     */
    Ob *
    net_idx(size_t idx) const;

    /* ---------------------------- Group Separator ---------------------------- */
    /*!
     * Lookup \p ob's nth relation (with a non-zero destination) of
     * a type "matching" \p rT to \p rD.
     *
     * \param[in] rM Relation Type Mask
     * \param[in] rM_N Relation Type Mask length
     * \param[in] nth occurrence number of search.
     * \param[in] aM attributes to require from matching object.
     * \param[in] aM_N number of attributes to require.
     *
     * \return the relation at idx or 0 if none-was found.
     */
    Ob *
    net_nth_MrAm(const REL_t * rM, size_t rM_N,
                 size_t nth,
                 const VATT_BIT_t * aM, size_t aM_N) const;

    /*!
     * Lookup \p ob's nth relation (with a non-zero destination) of
     * a type "matching" \p rT to \p rD.
     *
     * \param[in] rT Relation Type. \c REL_any_ finds any kind of relation.
     * \param[in] nth occurrence number of search.
     *
     * \return the relation at idx or 0 if none-was found.
     */
    Ob *
    net_nth_Rs(REL_t rT, size_t nth) const;

    Ob *
    net_nth_RsAm(REL_t rT, size_t nth,
                 const VATT_BIT_t * aM, size_t aM_N) const;

    /*!
     * Same as \p net_nth_Rs() but skips objects that are to be
     * delete.
     *
     * \param[in] rM Relation Type Mask
     * \param[in] rM_N Relation Type Mask length
     */
    Ob *
    net_nth_Rm_noDEL(const REL_t * rM, size_t rM_N,
                     size_t nth) const;

    /*!
     * Same as \p net_nth_Rs() but skips objects that are tagged as
     * being deleted.
     */
    Ob *
    net_nth_Rs_noDEL(REL_t rT, size_t nth) const;

    /* ---------------------------- Group Separator ---------------------------- */
    /*!
     * Lookup \p ob's \p nth-last relation (with a non-zero destination) of
     * a type "matching" \p rT to \p rD from end.
     *
     * \param[in] rT Relation Type. \c REL_any_ finds any kind of relation.
     * \param[in] nth occurrence number of search.
     *
     * \return the relation at idx or 0 if none-was found.
     */
    Ob *
    net_nth_Rs_last(REL_t rT, size_t nth) const;

    /* ---------------------------- Group Separator ---------------------------- */
    /*! \name Finding Object \p ob's Relations.
     */
    /* @{ */

    /*! Find Index of \p nth relation to \p rD of type \p rM.
     *
     * \return hit-index to matching relation or -1 if none was found.
     */
    size_t
    net_ifind_Rs(REL_t rT, size_t nth);

    /*! Find Index of \p nth relation to \p rD of type \p rM.
     *
     * \param[in] rM Relation Type Mask
     * \param[in] rM_N Relation Type Mask length
     * \param[out] rD non-nullptr relating object or nullptr for any relating object.
     * \return hit-index to matching relation or -1 if none was found.
     */
    int
    net_ifind_RmDs(const REL_t * rM, size_t rM_N,
                   Ob * rD, size_t nth) const;

    /* @} */

    /* ---------------------------- Group Separator ---------------------------- */
    /*! \name Modifying Object \p ob's Relations.
     */

    /* @{ */

    int
    net_mbox_post_aopRmI(const REL_t * rM, size_t rM_N,
                         size_t off, size_t num,
                         OP_t vop, int tdelay, int tdur, VREC_t vrec = VREC_none) const;

    int
    net_mbox_post_aopRm(const REL_t * rM, size_t rM_N,
                        OP_t vop,
                        int tdelay, int tdur, VREC_t vrec = VREC_none) const;

    /* @} */

/* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Count Number of Relations Recursively (with \p depth) matching \p rM and
     * attribute.
     *
     * \param[in] rM Relation Type Mask
     * \param[in] rM_N Relation Type Mask length
     *
     * \param[in] vatt <em>destination</em> attribute type
     * \param[in] uint8_v <em>destination</em> attribytes value
     *
     * \param[in] depth recursion depth (use -1 for infinite recursion)
     * \return Number of Matching Relations
     */
    size_t
    net_count_RmAsOs_UINT8(const REL_t * rM, size_t rM_N,
                           const VATT_UINT8_t vatt, uint8_t uint8_v,
                           OB_t oT, int depth) const;

    /*!
     * Count Number of Relations recursively (with \p depth) matching \p rM.
     *
     * \param[in] rM Relation Type Mask
     * \param[in] rM_N Relation Type Mask length
     * \param[in] depth recursion depth (use -1 for infinite recursion)
     * \return Number of Matching Relations
     */
    size_t
    net_count_RmOs(const REL_t * rM, size_t rM_N,
                   OB_t oT, int depth) const;

    /*!
     * Count Number of Relations matching \p rM.
     *
     * \param[in] rM Relation Type Mask
     * \param[in] rM_N Relation Type Mask length
     * \return Number of Matching Relations
     */
    size_t
    net_count_Rm(const REL_t * rM, size_t rM_N) const;

    size_t
    net_count_rToT(const Ob * ob, const REL_t rT) const;

    size_t
    net_count_Rm_noDEL(const REL_t * rM, size_t rM_N) const;

    size_t
    net_count_Rs_noDEL(const REL_t rT) const;

/* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Order Object \p ob's Relations according to \c VisSRel::rO.
     * \see vrel_cmp() for details on how relations are sorted
     */
    void net_order();

    /*!
     * Sort all \p ob's \p rM-matching relations according to their destination
     * object types (of the matching relations).
     *
     * \param[in] rM Relation Type Mask
     * \param[in] rM_N Relation Type Mask length
     */
    void net_sort_oT(const REL_t * rM, size_t rM_N);

    /*!
     * Sort all \p ob's \p rM-matching relations according to their destination
     * object sizes (of the matching relations).
     *
     * \param[in] rM Relation Type Mask
     * \param[in] rM_N Relation Type Mask length
     */
    void net_sort_osize(const REL_t * rM, size_t rM_N);

/* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Match the object \c REL_LETTERs to the character string at \p cbuf of
     * length \p cbuflen.
     * \return number of matching characters, or -1 if an error occurred.
     */
    int net_match_cbuf(REL_t rT,
                       const uchar * cbuf, size_t cbuflen);

/* ---------------------------- Group Separator ---------------------------- */

    /*!
     * Modify \p ob \p nth relation to be of type \p rT.
     * \return 1 upon success, -1 upon error.
     */
    int
    net_modify_type_at(size_t nth, REL_t rT);


public:
    /*!
     * \name Navigation/Traversal/Stepping Between in Ob-Graph.
     * \todo Reuse \c net_find.*
     */
    /* @{ */
    /*!
     * Circularly Traverse \p idx steps in hierarchy at \p ob.
     */
    Ob * net_navSide(int idx, GOREL_t gorel, bool sel_flag,
                      REL_t vrel_sup, REL_t vrel_sub);
    /*!
     * Navigate/Go up in hierarchy at \p ob.
     */
    Ob * net_navUp(bool sel_flag);
    /*!
     * Navigate/Go down in hierarchy \p idx steps gorel-relative to \p ob.
     */
    Ob * net_navDown(int idx, GOREL_t gorel, bool sel_flag);
    /*!
     * Navigate/Go in direction \p godir with \p gorel-relative to current
     * position \p step_num steps hierarchy at \p ob.
     */
    Ob *
    net_nav(GOREL_t gorel, GODIR_t godir, size_t step_num,
            REL_t vrel_sup, REL_t vrel_sub);
    /* @} */

public:
    /*!
     * Callback called whenever an \p rM_N relations at \p rM are created,
     * removed or modified.
     */
    void net_post_changeM_cb(const REL_t * rM, size_t rM_N);

public:
    /*!
     * Compare two objects \p ob0 and \p ob1 for equal contents.
     */
    friend bool eq(const Ob * ob0, const Ob * ob1);

    /*!
     * Get number of bytes in \c this.
     */
    int get_PRODUCER_file_size() const;

public:

    /*! \name Feedback. Default stdout. */
    /// \{

    /*! Shake \c this to express frustration.  Used to indicate that an
     * operation could not be applied on \c this.
     * \param[in] msg message to the user explaining the reason for the frustration.
     */
    virtual void say_frust(const char * msg) { puts(msg); }

    /*! Bump \c this to remind user.  Used to remind the user of something.
     * \param[in] msg reminder to the user.
     */
    virtual void say_remind(const char * msg) { puts(msg); }

    // Cvirtual void say(const char * msg, VTALK_t vtalk){ puts(msg); }

public:
    //size_t rmsgN;		///< Received Messages Counter
    virtual bool inc_rmsg() { /* ++rmsgN; */ return false; }
    virtual bool dec_rmsg() { /* --rmsgN; */ return false; }
    virtual size_t get_rmsg() { /* return rmsgN; */ return 0; }

protected:

private:
};

}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             