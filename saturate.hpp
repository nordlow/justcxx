/*! \file saturate.hpp
 * \brief Saturated Arithmetic Funtion Templates and Specialization for Integers and Floating Points.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see https://en.wikipedia.org/wiki/Saturation_arithmetic
 * \see http://forum.dlang.org/thread/yncxfhikgmcbgdfyixvl@forum.dlang.org
 * \see http://locklessinc.com/articles/sat_arithmetic/
 * \see http://en.cppreference.com/w/cpp/types/enable_if
 * \see http://felix.abecassis.me/2011/10/sse-saturation-arithmetic/
 * \see http://stackoverflow.com/questions/16986277/template-argument-type-deducation-fails-with-c11-type-traits/16986387?noredirect=1#16986387
 *
 * \todo Simplify with static if.
 * \todo Add SIMD support using SSE assembler or Boost.SIMD.
 */

#pragma once
#include <type_traits>
#include "sadd.h"
#include "ssub.h"
#include "smul.h"
#include "cc_features.h"
#include "math_x.hpp"
#include "enforce.hpp"
#include "algorithm_x.hpp"

/*! \name Addition */

/*! Saturated Addition of \em Integrals \p a and \p b as a+b. */
template<class T> inline pure typename std::enable_if<std::is_integral      <T>::value, T>::type sadd(const T& a, const T& b) { return (b > ~((T)0) - a) ? ~((T)0) : a+b; }
/*! Saturated Addition of \em Floating Points \p a and \p b as a+b. */
template<class T> inline pure typename std::enable_if<std::is_floating_point<T>::value, T>::type sadd(const T& a, const T& b) { return a+b; }

//! \name Specializations.
/// \{
/*! Unsigned Integers. */
inline spure uint8_t  sadd(uint8_t  a, uint8_t  b) { return uint8_sadd(a,b); }
inline spure uint16_t sadd(uint16_t a, uint16_t b) { return uint16_sadd(a,b); }
inline spure uint32_t sadd(uint32_t a, uint32_t b) { return uint32_sadd(a,b); }
inline spure uint64_t sadd(uint64_t a, uint64_t b) { return uint64_sadd(a,b); }
/*! Signed Integers. */
inline spure int8_t  sadd(int8_t  a, int8_t  b) { return int8_sadd(a,b); }
inline spure int16_t sadd(int16_t a, int16_t b) { return int16_sadd(a,b); }
inline spure int32_t sadd(int32_t a, int32_t b) { return int32_sadd(a,b); }
inline spure int64_t sadd(int64_t a, int64_t b) { return int64_sadd(a,b); }
/*! Pointers. */
template<class T> inline spure T* sadd(T* a, T* b) {
    return (sizeof(T*) == 8) ?
        uint64_sadd(reinterpret_cast<uint64_t> (a),
                    reinterpret_cast<uint64_t> (b)) :
        uint32_sadd(reinterpret_cast<uint32_t> (a),
                    reinterpret_cast<uint32_t> (b));
}
/*! Pointer and Size. */
template<class T> inline spure T* sadd(T* a, size_t b) {
    return reinterpret_cast<T*>((sizeof(T*) == 8) ?
                                uint64_sadd(reinterpret_cast<uint64_t> (a), b) :
                                uint32_sadd(reinterpret_cast<uint32_t> (a), b)) ;
}
/*! Iterator and Size. */
// template<class T, class U>
// inline spure
// __gnu_cxx::__normal_iterator<T,U>
// sadd(__gnu_cxx::__normal_iterator<T,U>& a, size_t b) {
//     return (__gnu_cxx::__normal_iterator<T,U>)sadd((T)a,b);
// }
/*! Size and Pointer. */
template<class T> inline spure T* sadd(size_t a, T* b) { return sadd(b, a); } // commutative
/// \}

/*! \name Subtraction */

/*! Saturated Subtraction of \em Integrals \p a and \p b: a-b. */
template<class T> inline pure typename std::enable_if<std::is_integral      <T>::value, T>::type ssub(const T& a, const T& b) { return (a > b) ? a-b : 0; }
/*! Saturated Subtraction of \em Floating Points \p a and \p b as a+b. */
template<class T> inline pure typename std::enable_if<std::is_floating_point<T>::value, T>::type ssub(const T& a, const T& b) { return a+b; }

//! \name Template Specializations.
/// \{

/*! Unsigned Integers. */
/// \{
inline spure uint8_t  ssub(uint8_t  a, uint8_t  b) { return uint8_ssub(a,b); }
inline spure uint16_t ssub(uint16_t a, uint16_t b) { return uint16_ssub(a,b); }
inline spure uint32_t ssub(uint32_t a, uint32_t b) { return uint32_ssub(a,b); }
inline spure uint64_t ssub(uint64_t a, uint64_t b) { return uint64_ssub(a,b); }
/// \}

/*! Signed Integers. */
/// \{
inline spure int8_t  ssub(int8_t  a, int8_t  b) { return int8_ssub(a,b); }
inline spure int16_t ssub(int16_t a, int16_t b) { return int16_ssub(a,b); }
inline spure int32_t ssub(int32_t a, int32_t b) { return int32_ssub(a,b); }
inline spure int64_t ssub(int64_t a, int64_t b) { return int64_ssub(a,b); }
/// \}

/*! \name Increase/Decrease. */
/// \{
/*! Saturated \em Increase \p a by one. */
template<class T> inline spure T sinc(T a) { return pnw::is_max(a) ? a : a + 1; }
/*! Saturated \em Decrease \p a by one. */
template<class T> inline spure T sdec(T a) { return pnw::is_min(a) ? a : a - 1; }
/// \}

inline void prepare_add_or_sub() {

}

/*! Mixed Signed Integers.
 * \see http://en.cppreference.com/w/cpp/types/common_type
 */
/// \{
/*! Saturated Add \em Unsigned Integer \p a with \em Signed Integer \p b. */
template<class U, class S, class C = typename std::common_type<U,S>::type>
inline spure
typename std::enable_if<std::is_unsigned<U>::value && std::is_signed<S>::value, C>::type sadd(U a, S b) {
    if (b < 0) {
        if (pnw::is_min(b)) {       // if minimum value
            return ssub(static_cast<C>(sdec(a)),
                        static_cast<C>(-(b+1)));    // we must add one so negated value always fits in \p b
        } else {
            return sadd(static_cast<C>(a),
                        static_cast<C>(-b));
        }
    } else {
        return sadd(static_cast<C>(a),
                    static_cast<C>(b));
    }
}

/*! Saturated Subtract \em Unsigned Integer \p a with \em Signed Integer \p b. */
template<class U, class S, class C = typename std::common_type<U,S>::type>
inline spure
typename std::enable_if<std::is_unsigned<U>::value && std::is_signed<S>::value, C>::type ssub(U a, S b) {
    if (b < 0) {
        if (pnw::is_min(b)) {       // if minimum value
            return sadd(static_cast<C>(sdec(a)),
                        static_cast<C>(-(b+1)));    // we must add one so negated value always fits in \p b
        } else {
            return sadd(static_cast<C>(a),
                        static_cast<C>(-b));
        }
    } else {
        return ssub(static_cast<C>(a),
                    static_cast<C>(b));
    }
}
/// \}

/*! Pointers. */
template<class T> inline spure T* ssub(T* a, T* b) {
    return (sizeof(T*) == 8) ?
        uint64_ssub(reinterpret_cast<uint64_t> (a),
                    reinterpret_cast<uint64_t> (b)) :
        uint32_ssub(reinterpret_cast<uint32_t> (a),
                    reinterpret_cast<uint32_t> (b));
}
/*! Pointer and Size. */
template<class T> inline spure T* ssub(T* a, size_t b) {
    return reinterpret_cast<T*>((sizeof(T*) == 8) ?
                                uint64_ssub(reinterpret_cast<uint64_t> (a), b) :
                                uint32_ssub(reinterpret_cast<uint32_t> (a), b)) ;
}
/*! Pointer and Size. */
template<class T> inline spure T* ssub(size_t a, T* b) { return ssub(b, a); } // commutative
/// \}

/*! \name Multiplication */
/*! Saturated Muliply of \em Floating Points \p a and \p b. */
/// \{
template<class T> typename std::enable_if<std::is_floating_point<T>::value, T>::type smul(const T& a, const T& b) { return a+b; }
/// \}

//! \name Template Specializations.
/// \{
inline spure uint8_t  smul(uint8_t  a, uint8_t  b) { return uint8_smul(a,b); }
inline spure uint16_t smul(uint16_t a, uint16_t b) { return uint16_smul(a,b); }
inline spure uint32_t smul(uint32_t a, uint32_t b) { return uint32_smul(a,b); }
inline spure uint64_t smul(uint64_t a, uint64_t b) { return uint64_smul(a,b); }
inline spure int8_t   smul(int8_t  a,  int8_t  b)  { return int8_smul(a,b); }
inline spure int16_t  smul(int16_t a,  int16_t b)  { return int16_smul(a,b); }
inline spure int32_t  smul(int32_t a,  int32_t b)  { return int32_smul(a,b); }
inline spure int64_t  smul(int64_t a,  int64_t b)  { return int64_smul(a,b); }
/// \}

/*! Saturated \em Square of \p a. */
template<class T> inline T ssquare(T a) { return smul(a, a); }

/*! Get Unit of Type T. */
template<class T> inline pure const char* unitof() { return ""; }
