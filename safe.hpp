/*! \file safe.hpp
 * \brief Safe Numeric Wrapper.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-07-02 16:05
 */

#pragma once
#import "maybe.hpp"
#import "ranged.hpp"

/*! Make Safe Wrapper to \p arg.
 */
template<class T, bool Exceptional = false>
inline ranged<maybe<T,Exceptional>,Exceptional> make_safe(T&& arg,
                                                          const T& low,
                                                          const T& high) {
    typedef maybe<T,Exceptional> M;
    return ranged<M,Exceptional>(M(arg), M(low), M(high));
}
