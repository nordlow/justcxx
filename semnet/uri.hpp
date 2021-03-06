/*! \file uri.hpp
 * \brief URI (Uniform Resource Index)
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-11-08 11:17
 */

#pragma once
#include "obmr.hpp"
#include "../csc.hpp"

namespace semnet {

/*! Uniform Resource Locator (URI) */
class URI : public ObMr
{
public:
    /// Constructor.
    URI(const csc& name) : m_name(name) {}

    /// Destructor.
    virtual ~URI() {}

    virtual OB_t get_type() const { return OB_URI; }

    /*!
     * Calculate Local Path of \p ob.
     * \return constant local path (\em pointer to internal storage,
     *         no explicit freeing needed)
     */
    virtual const csc& get_pathL() const { return m_name; }
protected:
private:
    csc m_name;		///< \em URI \em Name (Path).
};

}
