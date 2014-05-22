/*! \file type_traits_x.hpp
 * \brief C++ Type Traits and Concepts Definitions.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-03-04 21:05
 * \see http://stackoverflow.com/questions/1708867/check-type-of-element-in-stl-container-c
 * \see http://stackoverflow.com/questions/9407367/determine-if-a-type-is-an-stl-container-at-compile-time/13162413#13162413
 * \see http://channel9.msdn.com/Series/C9-Lectures-Stephan-T-Lavavej-Advanced-STL/C9-Lectures-Stephan-T-Lavavej-Advanced-STL-6-of-n
 */

#pragma once
#include <type_traits>

#define LTC class               // LessThanComparable
#define RAI class               // RandomAccessIterator
#define RAC class               // RandomAccessContainer
#define FAC class               // ForwardAccessContainer
// #define InputContainer class
// #define OutputContainer class
//#define InputIterator class
//#define OutputIterator class
#define Addable class
#define Sortable class

/// SFINAE Type Trait to detect whether T::const_iterator exists.
template<typename T>
struct has_const_iterator
{
private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;
    template<typename C> static yes test(typename C::const_iterator*);
    template<typename C> static no  test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
};

/// SFINAE Type Trait to detect whether "T::const_iterator T::begin/end() const" exist.
template<typename T>
struct has_begin_end
{
    template<typename C> static char (&f(typename std::enable_if<
                                         std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
                                         typename C::const_iterator(C::*)() const>::value, void>::type*))[1];
    template<typename C> static char (&f(...))[2];
    template<typename C> static char (&g(typename std::enable_if<
                                         std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
                                         typename C::const_iterator(C::*)() const>::value, void>::type*))[1];
    template<typename C> static char (&g(...))[2];
    static bool const beg_value = sizeof(f<T>(0)) == 1;
    static bool const end_value = sizeof(g<T>(0)) == 1;
};

// Basic is_container template; specialize to derive from std::true_type for all desired container types
template<typename T> struct is_container : public ::std::integral_constant<bool,
                                                                           has_const_iterator<T>::value &&
                                                                           has_begin_end<T>::beg_value &&
                                                                           has_begin_end<T>::end_value> { };
template<typename T, std::size_t N> struct is_container<T[N]> : public ::std::true_type { };
template<std::size_t N> struct is_container<char[N]> : public ::std::false_type { };
// template<typename T> struct is_container< ::std::valarray<T>> : public ::std::true_type { };

/*! String Trait. \see http://stackoverflow.com/a/5116921/683710 */
template<class T> struct is_string : std::false_type {};
template<class charT, class traits, class Alloc>
struct is_string<std::basic_string<charT, traits, Alloc> > : std::true_type {};
