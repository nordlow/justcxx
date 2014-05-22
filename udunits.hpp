/*! \file udunits.hpp
 * \brief UDUNITS-2 C++ Wrapper.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2010-02-02 17:05
 * \see http://www.unidata.ucar.edu/software/udunits/udunits-2/udunits2lib.html
 */

#pragma once
#include <udunits2.h>
#include <iosfwd>
#include "csc.hpp"
#include "enforce.hpp"

namespace ud {

class Unit;

/*! Unit \em System - Wrapper class for UDUNITS-2 \c ut_unit structure.
 * Makes no sense to copy this for now.
 */
class System : private boost::noncopyable
{
    friend class Unit;
public:
    /// Construct a \em default (SI) Unit System.
    System() { m_sys = ut_read_xml(NULL); }
    System(System& s) = delete;
    /// Destruct Unit System.
    virtual ~System() { clear(); }
protected:
    /// Constructor using \c ut_unit \p unit.
    System(ut_system* system) { m_sys = system; }
    void clear() { if (m_sys) { ut_free_system(m_sys); } }
private:
    ut_system* m_sys;             ///< The UDUNITS-2 C API System Structure.
};

/// Get Default Unit System.
const ud::System& get_default_system();

// ============================================================================

/*! Construct Unit in \p system and name \p name.
 * \param name can be for example "meter".
 */

/*! \em Unit - Wrapper class for UDUNITS-2 \c ut_unit structure.
 */
class Unit
{
public:
    /*! Construct \em Dimensionless Unit in default system. */
    Unit() { m_ut = ut_new_dimensionless_unit(get_default_system().m_sys); }

    /*! Construct \em Dimensionless Unit in \p system. */
    Unit(const System& system) { m_ut = ut_new_dimensionless_unit(system.m_sys); }

    /*! Construct Unit in \p system and name \p name.
     * \param name can be for example "meter".
     */
    Unit(const System& system, const char *name) { m_ut = ut_get_unit_by_name(system.m_sys, name); }

    /// Copy Constructor from \p in.
    Unit(const Unit& in) { m_ut = ut_clone(in.m_ut); }

    /// \todo Move Constructor from \p in.

    /// Assignment Constructor.
    const Unit& operator =(Unit& in) { clear(); m_ut = ut_clone(in.m_ut); return *this; }

    /// Destructor.
    ~Unit() { clear(); }

    /// Arithmetic Unary Operators
    /// \{
    /// Scale \p a.
    friend Unit scale(double factor, const Unit& a) { return Unit(ut_scale(factor, a.m_ut)); }
    /// Offset \p a.
    friend Unit offset(const Unit& a, double offset) { return Unit(ut_offset(a.m_ut, offset)); }
    /// Offset \p a by time.
    friend Unit offset_by_time(const Unit& a, const double origin) { return Unit(ut_offset_by_time(a.m_ut, origin)); }
    /// Invert \p a.
    friend Unit invert(const Unit& a) { return Unit(ut_invert(a.m_ut)); }
    /// \em Take the \p root:th \em Root of Unit and return the result.
    Unit root(int l_root) const { return Unit(ut_root(m_ut, l_root)); }
    /// Raise \p a to \p power.
    friend Unit raise(const Unit& a, int power) { return Unit(ut_raise(a.m_ut, power)); }
    /// Logarithm of \p a.
    friend Unit log(const double base, const Unit& a) { return Unit(ut_log(base, a.m_ut)); }
    /// \}

    /// Arithmetic Binary Operators
    /// \{
    friend Unit operator * (const Unit& a, const Unit& b) { return Unit(ut_multiply(a.m_ut, b.m_ut)); }
    friend Unit operator / (const Unit& a, const Unit& b) { return Unit(ut_divide(a.m_ut, b.m_ut)); }
    /// \}

    /// Comparison Binary Operators
    /// \{
    friend  int cmp         (const Unit& a, const Unit& b) { return (ut_compare(a.m_ut, b.m_ut)); }
    friend bool operator == (const Unit& a, const Unit& b) { return (ut_compare(a.m_ut, b.m_ut) == 0); }
    /// \}

    /// Return \c true if \p a and \p b belongs to the same system.
    friend bool same_system(const Unit& a, const Unit& b) { return ut_same_system(a.m_ut, b.m_ut); }

    /// Return \c true if \c this is dimensionless unit.
    bool is_dimensionless() const { return ut_is_dimensionless(m_ut); }

    /*! Get Unit \em Name.
     * \param encoding can be either \c UT_ASCII, \c UT_ISO_8859_1, \c UT_LATIN1 or \c UT_UTF8.
     */
    const char*	get_name(ut_encoding encoding = UT_UTF8) const { return ut_get_name(m_ut, encoding); }

    /*! Get Unit \em Symbol.
     * \param encoding can be either \c UT_ASCII, \c UT_ISO_8859_1, \c UT_LATIN1 or \c UT_UTF8.
     */
    const char*	get_symbol(ut_encoding encoding = UT_UTF8) const { return ut_get_symbol(m_ut, encoding); }

    /*! Get Unit \em System.
     */
    const ut_system* get_system() const { return ut_get_system(m_ut); }

    /*! Set Unit \em Name to \p symbol.
     * \return \c true upon success
     */
    bool set_unit_name(const System& system, const char *name) {
        bool ret = false;
        ut_unit* unit = ut_get_unit_by_name(system.m_sys, name);
        enforce(unit);
        if (unit) {
            if (m_ut) { ut_free(m_ut); } // free previous unit if any
            m_ut = unit;
            ret = true;
        }
        return ret;
    }

    /*! Set Unit \em Symbol to \p symbol.
     * \return \c true upon success
     */
    bool set_symbol(const System& system, const char *symbol);

    /// Print \c this to \p os.
    int print(std::ostream& os) const;

    /// Output \p a to \p os.
    friend std::ostream& operator << (std::ostream& os, const Unit& a) { a.print(os); return os; }

protected:
    /// Constructor using \c ut_unit \p unit.
    Unit(ut_unit* unit) { m_ut = unit; }

    void clear() { if (m_ut) { ut_free(m_ut); } }
private:
    ut_unit* m_ut;            ///< The UDUNITS-2 C API Unit Structure.
};

/*! Lookup \c ud::Unit from \p name. */
Unit * get_unit(const csc& name);

}
