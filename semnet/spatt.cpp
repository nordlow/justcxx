#include "spatt.hpp"

namespace semnet {
namespace patterns {

bir Super::sample_range() const {
    if (not m_irange.defined()) {
        update_sample_range(); // delegate
    }
    return m_irange;
}

void Super::prepare() const {
    Base::prepare();
}

void Super::unprepare() const {
    Base::unprepare();
    m_irange.undefine();
    m_multI_cache_ok_flag = false;
}

}
}
