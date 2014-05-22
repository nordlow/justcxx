#include "udunits.hpp"
#include <boost/unordered_map.hpp>

namespace ud
{

static ud::System g_default_system;
const ud::System & get_default_system() { return g_default_system; }

bool
Unit::set_symbol(const System & system, const char *symbol)
{
    bool ret = false;
    ut_unit* unit = ut_get_unit_by_symbol(system.m_sys, symbol);
    if (unit) {
        if (m_ut) { ut_free(m_ut); } // free previous unit if any
        m_ut = unit;
        ret = true;
    }
    return ret;
}

int
Unit::print(std::ostream & os) const
{
    const size_t size = 128;
    char buf[size];
    int ret = ut_format(m_ut, buf, size, UT_ASCII | UT_DEFINITION);
    if (ret >= 1) { os.write(buf, ret); } // upon success write result to stream
    return ret;
}

typedef boost::unordered_map<const char*, Unit*> UnitsMap;
static UnitsMap g_units_hash;

Unit *
get_unit(const csc & name)
{
    typedef UnitsMap::value_type UnitsMapEntry;
    Unit* unit = nullptr;
    const auto hit = g_units_hash.find(name.c_str());
    if (hit == g_units_hash.end()) { // if unit not present
        unit = new Unit(g_default_system, name.c_str());  // add it
        g_units_hash.insert(UnitsMapEntry(name.c_str(), unit));
    } else {                          // otherwise
        unit = hit->second;           // just reference it
    }
    return unit;
}

}
