/*!
 * \file make_array.hpp
 * \brief Construct C++11, \c TR1 or \c Boost \c array using variable argument functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see http://stackoverflow.com/questions/2409819/c-constructor-initializer-for-arrays/6357019#6357019
 *
 * \todo Use rvalue constructors: due to the nature of the wording of rvalue
 * references, and to some modification to the wording for lvalue references
 * (regular references), rvalue references allow developers to provide perfect
 * function forwarding. When combined with variadic templates, this ability
 * allows for function templates that can perfectly forward arguments to another
 * function that takes those particular arguments. This is most useful for
 * forwarding constructor parameters, to create factory functions that will
 * automatically call the correct constructor for those particular arguments.
 */

#pragma once
#include <type_traits> // <boost/type_traits/common_type.hpp>
#include <array>

namespace std {
#ifdef BOOST_NO_VARIADIC_TEMPLATES
/*! Construct Array from \p a. */
template<class T>
array<T,1> make_array(const T& a) {
    return array<T,1>({{ a }});
}
/*! Construct Array from \p a, \p b. */
template<class Ta, class Tb, class C = typename std::common_type<Ta, Tb, Tc>::type>
array<C,2> make_array(const Ta& a, const Tb& b) {
    return array<C,2>({{ a, b }});
}
/*! Construct Array from \p a, \p b, \p c. */
template<class Ta, class Tb, class Tc, class C = typename std::common_type<Ta, Tb, Tc>::type>
array<C,3> make_array(const Ta& a, const Tb& b, const Tc& c) {
    return array<C,3>({{ a, b, c }});
}
#else
/*! Construct Array from at least \em one mandatory argument \p a plus optionally more in \p b. */
template<class T, class ... R, class C = typename std::common_type<T, R...>::type>
array<C,1+sizeof...(R)> make_array(const T& a, const R& ... b) {
    return array<C,1+sizeof...(R)>({{ a, b... }});
}
/*! Construct Array from at least \em one mandatory argument \p a plus optionally more in \p b. */
template<class T, class ... R, class C = typename std::common_type<T, R...>::type>
array<C,1+sizeof...(R)> make_array_of_common_type(const T& a, const R& ... b) {
    return array<C,1+sizeof...(R)>({{ static_cast<C>(a), static_cast<C>(b)... }});
}
#endif
}

#include <boost/array.hpp>

namespace boost {
#ifdef BOOST_NO_VARIADIC_TEMPLATES
/*! Construct Array from \p a. */
template<class T>
array<T,1> make_array(const T& a) {
    return array<T,1>({{ a }});
}
/*! Construct Array from \p a, \p b. */
template<class Ta, class Tb, class C = typename std::common_type<Ta, Tb, Tc>::type>
array<C,2> make_array(const Ta& a, const Tb& b) {
    return array<C,2>({{ a, b }});
}
/*! Construct Array from \p a, \p b, \p c. */
template<class Ta, class Tb, class Tc, class C = typename std::common_type<Ta, Tb, Tc>::type>
array<C,3> make_array(const Ta& a, const Tb& b, const Tc& c) {
    return array<C,3>({{ a, b, c }});
}
#else
/*! Construct Array from at least \em one mandatory argument \p a plus optionally more in \p b. */
template<class T, class ... R, class C = typename std::common_type<T, R...>::type>
array<C,1+sizeof...(R)> make_array(const T& a, const R& ... b) {
    return array<C,1+sizeof...(R)>({{ a, b... }});
}
/*! Construct Array from at least \em one mandatory argument \p a plus optionally more in \p b. */
template<class T, class ... R, class C = typename std::common_type<T, R...>::type>
array<C,1+sizeof...(R)> make_array_of_common_type(const T& a, const R& ... b) {
    return array<C,1+sizeof...(R)>({{ static_cast<C>(a), static_cast<C>(b)... }});
}
#endif
}
