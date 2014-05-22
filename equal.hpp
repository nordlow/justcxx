/*! \file equal.hpp
 * \brief Contents Equality.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "type_traits_x.hpp"
#include "cc_features.h"

namespace std {

/*! Equal Contents (Elements) of \p a and \p b. */
template<class IC1,
         class IC2>
inline pure bool equal(const typename std::enable_if<is_container<IC1>::value, IC1>::type& a,
                       const typename std::enable_if<is_container<IC1>::value, IC1>::type& b) {
    return (a.size() == b.size() and
            equal(begin(a), end(a),
                  begin(b)));
}

/*! Equal Contents of \n first elements of \p a and \p b. */
template<class IC1,
         class IC2>
inline pure bool equal_n(const typename std::enable_if<is_container<IC1>::value, IC1>::type& a,
                         const typename std::enable_if<is_container<IC1>::value, IC1>::type& b, size_t n) {
    return (a.size() >= n and
            b.size() >= n and
            equal(begin(a), begin(a)+n,
                  begin(b)));
}

/*! Equal Contents (Elements) of \p a and \p b. */
template<class IC1,
         class IC2>
inline pure bool operator == (const typename std::enable_if<is_container<IC1>::value, IC1>::type& a,
                              const typename std::enable_if<is_container<IC1>::value, IC1>::type& b) { return equal(a, b); }

/*! Not Equal Contents (Elements) of \p a and \p b. */
// template<class IC1, class IC2>
// bool operator !=(const IC1& a, const IC2& b) { return not (a == b); }

}
