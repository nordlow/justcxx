/*! \file obmr.hpp
 * \brief semnetDB Object with Multiple (Several) Relations
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-09-03 18:00
 */

#pragma once
#include "ob.hpp"
#include "rels.hpp"

namespace semnet {

/*! Object with Multiple (Several) Typed Relations at \c m_rels..
 */
class ObMr : public Ob {
public:
    ObMr() : Ob() {}

    virtual ~ObMr() {}

    virtual size_t net_get_relation_count() const { return m_rels.size(); }

    virtual size_t net_disconnectM(REL_t rT = REL_any_,
                                   OB_t oT = OB_any_,
                                   const Ob * rD = nullptr,
                                   bool obs_flag = false) const;

    virtual Ob *
    net_nth_rMoMdT(const REL_t * rM, size_t rM_N,
                   const OB_t * oM = nullptr, size_t oM_N = 0,
                   const Ob * rD_arg = nullptr, size_t nth = 0) const;

    virtual Obs
    net_find_rMoM(const REL_t * rM, size_t rM_N,
                  const OB_t * oM = nullptr, size_t oM_N = 0,
                  VREC_t vrec = VREC_none) const;

    virtual size_t
    net_count_rMoM(const REL_t * rM, size_t rM_N,
                   const OB_t * oM, size_t oM_N) const;

    virtual std::ostream & show_relations(std::ostream & os) const;
protected:
    virtual void net_self_relate(REL_t rT = REL_any_) const;

    virtual void net_addM(REL_t rT, const Ob * const * rDA, size_t rDN, bool unique_flag = false) const;
    virtual void net_addM(const REL_t * rTA, const Ob * const * rDA, size_t rDN, bool unique_flag = false) const;

    virtual size_t net_retargetM(const Ob * from, const Ob * to) const;
    virtual size_t net_retypeM(REL_t from, REL_t to) const;

    virtual size_t net_remM(REL_t rT = REL_any_, const Ob * rD = nullptr) const;
    virtual size_t net_uniquifyM(REL_t rT = REL_any_, const Ob * rD = nullptr) const;

    mutable Refs m_rels; ///< Relations.
};

}
