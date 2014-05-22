/*! \file enforce.h
 * \brief Usability Improving Extensions to C's standard header assert.h.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-11-25 17:20
 * \see http://stackoverflow.com/questions/1721543/continue-to-debug-after-failed-assertion-on-linux-c-c/8271237#8271237
 * \see http://msdn.microsoft.com/en-us/library/ms679297%28VS.85%29.aspx
 * \todo Make use of __FILE__ __FUNCTION__ __LINE__ and format line so we can colorize and catch it.
 */

#pragma once
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(NDEBUG)
#  define enforce(expr)
#  define enforce_with(expr, sig)
#  define enforce_eq(expected, actual)
#  define enforce_lt(lhs, rhs)
#  define enforce_gt(lhs, rhs)
#  define enforce_lte(lhs, rhs)
#  define enforce_gte(lhs, rhs)
#  define enforce_zero(expr)
#else
#  define enforce_with(expr, sig)                                       \
  if (!(expr)) {                                                        \
    fprintf(stderr, "%s:%d: error: in %s: Assertion `%s' failed.",                \
            __FILE__, __LINE__, __ASSERT_FUNCTION, __STRING(expr)); raise(sig); \
  }
#  define enforce(expr) enforce_with(expr, SIGTRAP);
#  define enforce_eq(expected, actual)                                  \
  if (!(expected == actual)) {                                          \
    fprintf(stderr, "%s:%d: error: in %s: Assertion `%s' == `%s' failed.",        \
            __FILE__, __LINE__, __ASSERT_FUNCTION, __STRING(expected), __STRING(actual)); raise(SIGTRAP); \
  }
#  define enforce_neq(expected, actual)                                 \
  if (!(expected != actual)) {                                          \
    fprintf(stderr, "%s:%d: error: in %s: Assertion `%s' != `%s' failed.",        \
            __FILE__, __LINE__, __ASSERT_FUNCTION, __STRING(expected), __STRING(actual)); raise(SIGTRAP); \
  }
#  define enforce_lt(lhs, rhs)                                          \
  if (!(lhs < rhs)) {                                                   \
    fprintf(stderr, "%s:%d: error: in %s: Assertion `%s' < `%s' failed.",         \
            __FILE__, __LINE__, __ASSERT_FUNCTION, __STRING(lhs), __STRING(rhs)); raise(SIGTRAP); \
  }
#  define enforce_gt(lhs, rhs)                                          \
  if (!(lhs > rhs)) {                                                   \
    fprintf(stderr, "%s:%d: error: in %s: Assertion `%s' < `%s' failed.",         \
            __FILE__, __LINE__, __ASSERT_FUNCTION, __STRING(lhs), __STRING(rhs)); raise(SIGTRAP); \
  }
#  define enforce_lte(lhs, rhs)                                         \
  if (!(lhs <= rhs)) {                                                  \
    fprintf(stderr, "%s:%d: error: in %s: Assertion `%s' <= `%s' failed.",        \
            __FILE__, __LINE__, __ASSERT_FUNCTION, __STRING(lhs), __STRING(rhs)); raise(SIGTRAP); \
  }
#  define enforce_gte(lhs, rhs)                                         \
  if (!(lhs >= rhs)) {                                                  \
    fprintf(stderr, "%s:%d: error: in %s: Assertion `%s' >= `%s' failed.",        \
            __FILE__, __LINE__, __ASSERT_FUNCTION, __STRING(lhs), __STRING(rhs)); raise(SIGTRAP); \
  }
#  define enforce_zero(expr)                                            \
  if (!(expr == 0)) {                                                   \
    fprintf(stderr, "%s:%d: error: in %s: Assertion `%s' is zero failed.",        \
            __FILE__, __LINE__, __ASSERT_FUNCTION, __STRING(expr)); raise(SIGTRAP); \
  }
#endif

#ifdef __cplusplus
}
#endif
