/*!
 * \file complex_x.hpp
 * \brief Extension to std::complex with Boost Type Traits.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-07-11 01:00
 */

#pragma once
#include <type_traits> // or <boost/type_traits/common_type.hpp>

namespace std
{

// /*! Addition */
// template<class T, class U, class V = typename std::common_type<T, U>::type >
// complex<V> operator + (complex<T> a, complex<U> b) { return complex<V>(a.real()+b.real(), a.imag()+b.imag()); }

/*! Addition */
template<class T, class U, class V = typename std::common_type<T, U>::type >
complex<V> operator + (const complex<T>&  a, const complex<U>& b) { return (complex<V>((V)a.real(), (V)a.imag()) +
                                                                            complex<V>((V)b.real(), (V)b.imag())); }
/*! Subtraction */
template<class T, class U, class V = typename std::common_type<T, U>::type >
complex<V> operator - (const complex<T>& a, const complex<U>& b) { return (complex<V>((V)a.real(), (V)a.imag()) -
                                                                           complex<V>((V)b.real(), (V)b.imag())); }
/*! Multiplication */
template<class T, class U, class V = typename std::common_type<T, U>::type >
complex<V> operator * (const complex<T>& a, const complex<U>& b) { return (complex<V>((V)a.real(), (V)a.imag()) *
                                                                           complex<V>((V)b.real(), (V)b.imag())); }
/*! Divison */
template<class T, class U, class V = typename std::common_type<T, U>::type >
complex<V> operator / (const complex<T>& a, const complex<U>& b) { return (complex<V>((V)a.real(), (V)a.imag()) /
                                                                           complex<V>((V)b.real(), (V)b.imag())); }
/*! Exponentiation */
template<class T, class U, class V = typename std::common_type<T, U>::type >
complex<V> pow (const complex<T>& a, const complex<U>& b) { return pow<complex<V>, complex<U>> (complex<V>((V)a.real(), (V)a.imag()), b); }
/*! Exponentiation */
template<class T, class U, class V = typename std::common_type<T, U>::type >
complex<V> pow (const complex<T>& a, const U& b) { return pow<complex<V>, U> (a, b); }
/*! Exponentiation */
template<class T, class U, class V = typename std::common_type<T, U>::type >
complex<V> pow (const T& a, const complex<U>& b) { return pow<T, complex<V>> (a, complex<V>(b.real(), b.imag())); }

}
