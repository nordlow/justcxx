/*! \file: outer_box.hpp
 * \brief An Bounding Bounding.
 * \author: Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date: 2012-05-26 15:03
 * \see http://stackoverflow.com/questions/7531866/unresolved-overloaded-function-type-c
 */

#pragma once
#include <bits/allocator.h>
#include <algorithm>
#include "../type_traits_x.hpp"
#include "box.hpp"

/*! Outer Box (Union) of STL Container \tparam C \p a of Vectors. */
template<class T, std::size_t N,
         template<class X, class Allocator = std::allocator<vec<T,N> > > class C>
inline pure
typename std::enable_if<is_container<C<vec<T,N> > >::value,
                        box<T,N>>::type
outer_box(const C<vec<T,N> >& a) {
    auto b = a.size() ? static_cast<box<T,N> >(*begin(a)) : box<T,N>::unite_unit();
    std::for_each(begin(a), end(a), [&b](const vec<T,N>& e) { b = unite(b, e); });
    return b;
}

/*! Outer Box (Union) of STL Container \tparam C \p a of Boxes. */
#if 0
template<class T, std::size_t N,
          template<class X, class Allocator = std::allocator<box<T,N> > > class C>
inline pure box<T,N> outer_box(const C<box<T,N> >& a) {
    return std::accumulate(begin(a), end(a), box<T,N>::unite_unit(), (box<T,N>(*)(box<T,N>,
                                                                                  vec<T,N>))unite);
}

#endif
