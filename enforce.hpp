/*! \file enforce.hpp
 * \brief Usability Improving Extensions to assert.h.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see http://stackoverflow.com/questions/37473/how-can-i-assert-without-using-abort
 * \todo Make use GCC's Variadic Macros.
 * \see http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 */

#pragma once
#include <csignal>
#include <iostream>
#include "equal.hpp"
#include "show_all.hpp"

# if defined __cplusplus ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#   define __ENFORCE_FUNCTION	__PRETTY_FUNCTION__
# else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __ENFORCE_FUNCTION	__func__
#  else
#   define __ENFORCE_FUNCTION	((__const char *) 0)
#  endif
# endif

/*! TODO: Use exceptions instead of raise(SIGTRAP)? */
struct Warning { };
struct Wrong { };
#ifdef NDEBUG
const bool CHECK_WRONG = false;
#else
const bool CHECK_WRONG = true;
#endif
/*! Assure that \p assertion is true.
 * \usage assure<Wrong>(2 + 2 == 5);
 */
template<class X = Wrong>
inline void assure(bool assertion) {
    if (CHECK_WRONG and not assertion) throw X();
}

#if defined(NDEBUG)
#  define enforce(expr)
#  define enforce_with(expr, sig)
#  define enforce_eq(expected, actual)
#  define enforce_eq3(expectedA, expectedB, actual)
#  define enforce_eq4(expectedA, expectedB, expectedC, actual)
#  define enforce_lt(lhs, rhs)
#  define enforce_gt(lhs, rhs)
#  define enforce_lte(lhs, rhs)
#  define enforce_gte(lhs, rhs)
#  define enforce_zero(expr)
#else
#  define enforce_bop_with(lhs, op, rhs, sig)                                    \
    std::cerr << std::endl << __FILE__ << ":" << __LINE__ << ": error:" << " in " << __ENFORCE_FUNCTION << ": " \
              << "Assertion failed for" << std::endl \
    << " (`" __STRING(lhs) << "' = " << lhs << ") " op << std::endl \
    << " (`" __STRING(rhs) << "' = " << rhs << ")" << std::endl; \
    raise(SIGTRAP);
#  define enforce_bop(lhs, op, rhs) enforce_bop_with(lhs, op, rhs, SIGTRAP);
#  define enforce_with(expr, sig)                                       \
    if (!(expr)) {                                                      \
        std::cerr << std::endl << __FILE__ << ":" << __LINE__ << ": error:" << " in " << __ENFORCE_FUNCTION << ": " \
                  << "Assertion `" << __STRING(expr) << "'" << " failed."; \
        raise(sig);                                                     \
    }
#  define enforce(expr) enforce_with(expr, SIGTRAP);
/*! Assure \p expected and \p actual have \em equal contents. */
#  define enforce_eq(expected, actual) \
    if (expected != actual) { enforce_bop(expected, "==", actual); }
/*! Assure \p expectedA, \p expectedB and \p expectedC actual have \em equal contents. */
#  define enforce_eq3(expectedA, expectedB, actual) \
    if (expectedA != actual) { enforce_bop(expectedA, "==", actual);} \
    if (expectedB != actual) { enforce_bop(expectedB, "==", actual);}
/*! Assure \p expectedA, \p expectedB, \p expectedc and \p expectedD actual have \em equal contents. */
#  define enforce_eq4(expectedA, expectedB, expectedC, actual)          \
    if (expectedA != actual) { enforce_bop(expectedA, "==", actual);}   \
    if (expectedB != actual) { enforce_bop(expectedB, "==", actual);}\
    if (expectedC != actual) { enforce_bop(expectedC, "==", actual);}
/*! Assure \p expected and \p actual have \em different contents. */
#  define enforce_neq(expected, actual) if (expected == actual) { enforce_bop(expected, "!=", actual); }
#  define enforce_lt(lhs, rhs) if (not (lhs < rhs)) { enforce_bop(lhs, "<", rhs); }
#  define enforce_gt(lhs, rhs) if (not (lhs > rhs)) { enforce_bop(lhs, ">", rhs); }
#  define enforce_lte(lhs, rhs) if (not (lhs <= rhs)) { enforce_bop(lhs, "<=", rhs); }
#  define enforce_gte(lhs, rhs) if (not (lhs >= rhs)) { enforce_bop(lhs, ">=", rhs); }
#  define enforce_zero(expr) enforce_eq(expr, 0)
#endif

// /*! Check if \p a and \p b have the same value. */
// template<class T> inline bool assert_equal(const T& a, const T& b) { assert(a == b); }
// /*! Check if \p a and \p b have the same value. */
// template<class T> inline bool assert_equal(const T * a, const T * b) { assert(*a == *b); }

// /*! Check if \p a and \p b are the same object. */
// template<class T> inline bool assert_same(const T& a, const T& b) { assert(&a == &b); }
// /*! Check if \p a and \p b are the same object. */
// template<class T> inline bool assert_same(const T * a, const T * b) { assert(a == b); }

// template<class T>
// inline bool assert_nullptr();

// template<class T>
// inline bool assert_nonnull() {  }

#if 0
/*! Assert that \c Exception is thrown when \p pre_fn is called and that
 *  argument of \p pre_fn is unchanged.
 * \todo Extend to variadic.
 */
template<class Exception, class T>
inline void enforce_pure_thrown(std::function<void(T&)>& pre_fn, // Or: If not C++11 available use: void (*pre_fn)(T&)
                                const T& a)
{
    const auto prev = a;           // backup value
    auto curr = prev;              // current value
    auto got = false;
    try {
        pre_fn(curr);
    } catch (const Exception& e) {
        got = true;
    }
    enforce(got);               // assure exception was thrown and caught
    enforce_eq(prev, curr);     // operation should preserve original value
}

/*! Assert that \c Exception is \em not thrown when \p pre_fn is called and that
 *  argument of \p pre_fn is unchanged.
 * \todo Extend to variadic.
 */
template<class Exception, class T>
inline void enforce_pure_unthrown(std::function<void(T&)> pre_fn, // Or: If not C++11 available use: void (*pre_fn)(T&)
                                  const T& a)
{
    auto curr = a;              // current value
    auto got = false;
    try { pre_fn(curr); } catch (const Exception& e) { got = true; }
    enforce(not got);           // assure exception was thrown and caught
}
#endif
