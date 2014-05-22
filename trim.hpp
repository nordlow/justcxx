/*! \file trim.hpp
 * \brief Trim/Strip Algorithms.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-03-02 11:06
 *
 * \todo Generalize to any predicate.
 *
 * \see http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
 * \see http://www.boost.org/doc/libs/1_53_0/doc/html/string_algo.html
 * \see boost/algorithm/string/trim.hpp
 */

#pragma once
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include "cc_features.h"

/*! In-Place Trim/Chop Space from Beginning (Left).
 * \alias lstrip
 */
template<class T> inline T& in_place_ltrim(T& s) {
    s.erase(s.begin(),
            std::find_if(s.begin(),
                         s.end(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}
/*! In-Place (Trailing) Trim/Chop Space from End (Right).
 * \alias rstrip
 */
template<class T> inline T& in_place_rtrim(T& s) {
    s.erase(std::find_if(s.rbegin(),
                         s.rend(),
                         std::not1(std::ptr_fun<int, int> (std::isspace))).base(),
            s.end());
    return s;
}
/*! In-Place Trim/Chop Space from Both Ends.
 * \alias strip
 */
template<class T> inline T& in_place_trim(T& s) {
    return in_place_ltrim<T>(in_place_rtrim<T>(s));
}

/*! Return Trim/Chop Space from Beginning (Left).
 * \alias lstrip
 */
template<class T> inline spure T ltrim(T a) { T b = a; in_place_ltrim(b); return b; }
/*! Return (Trailing) Trim/Chop Space from End (Right).
 * \alias rstrip
 */
template<class T> inline spure T rtrim(T a) { T b = a; in_place_rtrim(b); return b; }
/*! Return Trim/Chop Space from Both Ends.
 * \alias strip
 */
template<class T> inline spure T trim(T a) { T b = a; in_place_trim(b); return b; }

/*! Return \p s without its last element or \p s if already empty.
 */
template<class T> inline spure T chop(T s) {
    if (s.size() > 0) {
        //s.pop_back();           // C++11
        s.resize(s.size()-1);
    }
    return s;
}
