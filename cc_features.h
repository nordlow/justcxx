/*! \file cc_features.h
 * \brief Compiler Features Support.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-09 01:58
 *
 * \see https://wiki.apache.org/stdcxx/C++0xCompilerSupport
 * \see http://gcc.gnu.org/projects/cxx0x.html
 * \see http://stackoverflow.com/questions/10225451/c11-feature-checking
 * \see http://www.boost.org/doc/libs/1_49_0/libs/config/doc/html/index.html#config_config_script
 * \see http://clang.llvm.org/docs/LanguageExtensions.html
 * \see https://en.wikipedia.org/wiki/C%2B%2B11
 * \see http://woboq.com/blog/cpp11-in-qt5.html
 *
 * \todo Use alloca instead of \c HAVE_C99_VARIABLE_LENGTH_ARRAYS when not available.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ===== */
/* Clang */
/* ===== */
#if defined(__clang__)
#  define GCC_VERSION_AT_LEAST(x,y) (1) /**< Assume that Clang supports all GCC attributes */
// #include </usr/include/boost/config/compiler/clang.hpp>
/* C99 */
/*! C99: Variable Length Arrays.
 * \see https://binglongx.wordpress.com/2011/05/08/c-and-c-arrays-on-stack-and-heap/
 */
#define HAVE_C99_VARIABLE_LENGTH_ARRAYS (1)
/* C++11 */

/* C++11 */
#define HAVE_CXX11_DECLARED_TYPE_OF_AN_EXPRESSION __has_feature(cxx_decltype) // Keyword \c decltype.
#define HAVE_CXX11_VARIADIC_TEMPLATES __has_feature(cxx_variadic_templates)
#define HAVE_CXX11_UNIQUE_PTR (1) /*! Supported by default. */
#define HAVE_CXX11_RANGE_BASED_FOR_LOOP __has_feature(cxx_range_for)
#define HAVE_CXX11_LAMBDA_EXPRESSIONS __has_feature(cxx_lambdas)
#define HAVE_CXX11_TEMPLATE_ALIASES __has_feature(cxx_alias_templates)
#define HAVE_CXX11_NULL_POINTER_CONSTANT __has_feature(cxx_nullptr)
#define HAVE_CXX11_CONSTEXPR __has_feature(cxx_constexpr) /**< Generalized Constant Expressions \c constexpr */
#define HAVE_CXX11_STRONGLY_TYPED_ENUMS __has_feature(cxx_strong_enums)
#define HAVE_CXX11_AUTO_TYPED_VARIABLES __has_feature(cxx_auto_type)
#define HAVE_CXX11_EXTERN_TEMPLATES (1) /*! Supported by default. */
#define HAVE_CXX11_INITIALIZER_LISTS __has_feature(cxx_generalized_initializers)
#define HAVE_CXX11_DELEGATING_CONSTRUCTORS __has_feature(cxx_delegating_constructors)
#define HAVE_CXX11_INHERITING_CONSTRUCTORS __has_feature(cxx_inheriting_constructors)
#define HAVE_CXX11_USER_DEFINED_LITERALS __has_feature(cxx_user_literals)) // TODO: Clang (#defined (__has_feature) && __has_feature(cxx_user_literals)))
#define HAVE_CXX11_GENERALIZED_ATTRIBUTES __has_feature(cxx_attributes)
#define HAVE_CXX11_THREAD_LOCAL_STORAGE __has_feature(cxx_thread_local)
#define HAVE_CXX11_FUTURE (1) /*! Supported by default. */
#define HAVE_CXX11_TO_STRING (1) /* TODO: Library Feature? */
#define HAVE_CXX11_EXPLICIT __has_feature(cxx_explicit_conversions)
#define HAVE_CXX11_TRAILING_RETURN_TYPE __has_feature(cxx_trailing_return)
/* C++1y */
#define HAVE_CXX1Y_RETURN_TYPE_DEDUCTION __has_feature(cxx_return_type_deduction)
#define HAVE_CXX1Y_VARIABLE_LENGTH_ARRAYS __has_feature(cxx_variable_length_arrays) // (0)

/* === */
/* GCC */
/* === */
#elif defined(__GNUC__)
// #include </usr/include/boost/config/compiler/gcc.hpp>
#    define GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > x || __GNUC__ == x && __GNUC_MINOR__ >= y)
#define HAVE_C99_VARIABLE_LENGTH_ARRAYS GCC_VERSION_AT_LEAST(4,0)
/* C++11 */
#define HAVE_CXX11_DECLARED_TYPE_OF_AN_EXPRESSION (GCC_VERSION_AT_LEAST(4.3) && __cplusplus == 201103L) // Keyword \c decltype.
#define HAVE_CXX11_VARIADIC_TEMPLATES (GCC_VERSION_AT_LEAST(4,3) && __cplusplus == 201103L)
#define HAVE_CXX11_UNIQUE_PTR (GCC_VERSION_AT_LEAST(4,4) && __cplusplus == 201103L)
#define HAVE_CXX11_RANGE_BASED_FOR_LOOP (GCC_VERSION_AT_LEAST(4,6) && __cplusplus == 201103L)
#define HAVE_CXX11_LAMBDA_EXPRESSIONS (GCC_VERSION_AT_LEAST(4,5) && __cplusplus == 201103L)
#define HAVE_CXX11_TEMPLATE_ALIASES (GCC_VERSION_AT_LEAST(4,7) && __cplusplus == 201103L)
#define HAVE_CXX11_NULL_POINTER_CONSTANT (GCC_VERSION_AT_LEAST(4,6) && __cplusplus == 201103L)
#define HAVE_CXX11_CONSTEXPR (GCC_VERSION_AT_LEAST(4,6) && __cplusplus == 201103L) /**< Generalized Constant Expressions \c constexpr */
#define HAVE_CXX11_STRONGLY_TYPED_ENUMS (GCC_VERSION_AT_LEAST(4,4) && __cplusplus == 201103L)
#define HAVE_CXX11_AUTO_TYPED_VARIABLES (GCC_VERSION_AT_LEAST(4,4) && __cplusplus == 201103L)
#define HAVE_CXX11_EXTERN_TEMPLATES (1) /*! Supported by default. */
#define HAVE_CXX11_INITIALIZER_LISTS (GCC_VERSION_AT_LEAST(4,4) && __cplusplus == 201103L)
#define HAVE_CXX11_DELEGATING_CONSTRUCTORS (GCC_VERSION_AT_LEAST(4,7) && __cplusplus == 201103L)
#define HAVE_CXX11_INHERITING_CONSTRUCTORS (GCC_VERSION_AT_LEAST(4,8) && __cplusplus == 201103L)
#define HAVE_CXX11_USER_DEFINED_LITERALS (GCC_VERSION_AT_LEAST(4,7) && __cplusplus == 201103L) // TODO: Clang (#defined (__has_feature) && __cplusplus == 201103L && __has_feature(cxx_user_literals) && __cplusplus == 201103L) && __cplusplus == 201103L) && __cplusplus == 201103L
#define HAVE_CXX11_GENERALIZED_ATTRIBUTES (GCC_VERSION_AT_LEAST(4,8) && __cplusplus == 201103L)
#define HAVE_CXX11_THREAD_LOCAL_STORAGE (GCC_VERSION_AT_LEAST(4,8) && __cplusplus == 201103L)
#define HAVE_CXX11_FUTURE (GCC_VERSION_AT_LEAST(4,6) && __cplusplus == 201103L)
#define HAVE_CXX11_TO_STRING (GCC_VERSION_AT_LEAST(4,6) && __cplusplus == 201103L)
#define HAVE_CXX11_EXPLICIT (GCC_VERSION_AT_LEAST(4,5) && __cplusplus == 201103L)
#define HAVE_CXX11_TRAILING_RETURN_TYPE (GCC_VERSION_AT_LEAST(4,6) && __cplusplus == 201103L)
/* C++1y. \see http://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html for explanation of __cplusplus value */
#define HAVE_CXX1Y_RETURN_TYPE_DEDUCTION (GCC_VERSION_AT_LEAST(4,8) && __cplusplus > 201103L)
#define HAVE_CXX1Y_VARIABLE_LENGTH_ARRAYS (GCC_VERSION_AT_LEAST(4,9) && __cplusplus > 201103L)

/*! Visual Studio
 * - MSVC++ 11.0 _MSC_VER = 1700 (Visual Studio 2011)
 * - MSVC++ 10.0 _MSC_VER = 1600 (Visual Studio 2010)
 * - MSVC++ 9.0  _MSC_VER = 1500 (Visual Studio 2008)
 * - MSVC++ 8.0  _MSC_VER = 1400 (Visual Studio 2005)
 * - MSVC++ 7.1  _MSC_VER = 1310 (Visual Studio 2003)
 * - MSVC++ 7.0  _MSC_VER = 1300
 * - MSVC++ 6.0  _MSC_VER = 1200
 * - MSVC++ 5.0  _MSC_VER = 1100
 */
#elif defined(_MSC_VER)
#include </usr/include/boost/config/compiler/visualc.hpp>
#define HAVE_CXX11_VARIADIC_TEMPLATES (! BOOST_NO_VARIADIC_TEMPLATES)
#    define GCC_VERSION_AT_LEAST(x,y) 0
#endif

#if !HAVE_CXX11_CONSTEXPR
#  define constexpr
#endif

#if 0
#ifndef _always_inline
#if GCC_VERSION_AT_LEAST(3,1)
#    define _always_inline __attribute__((always_inline)) inline
#else
#    define _always_inline inline
#endif
#endif

#ifndef _noreturn
#if GCC_VERSION_AT_LEAST(2,5)
#    define _noreturn __attribute__((noreturn))
#else
#    define _noreturn
#endif
#endif

#ifndef _noinline
#if GCC_VERSION_AT_LEAST(3,1)
#    define _noinline __attribute__((noinline))
#else
#    define _noinline
#endif
#endif
#endif

#ifndef _pure
#if GCC_VERSION_AT_LEAST(3,1)
#    define _pure __attribute__((pure))
#else
#    define _pure
#endif
#endif

#ifndef _const
#if GCC_VERSION_AT_LEAST(2,6)
#    define _const __attribute__((const))
#else
#    define _const
#endif

#ifndef wuur
#if GCC_VERSION_AT_LEAST(3,1)
#    define wuur __attribute__((warn_unused_result))
#else
#    define wuur
#endif
#endif

#ifndef _cold
#if GCC_VERSION_AT_LEAST(4,3)
#    define _cold __attribute__((cold))
#else
#    define _cold
#endif
#endif

#ifndef _flatten
#if GCC_VERSION_AT_LEAST(4,1)
#    define _flatten __attribute__((flatten))
#else
#    define _flatten
#endif
#endif

#ifndef _deprecated
#if GCC_VERSION_AT_LEAST(3,1)
#    define _deprecated __attribute__((deprecated))
#else
#    define _deprecated
#endif
#endif

/**
 * Disable warnings about deprecated features
 * This is useful for sections of code kept for backward compatibility and
 * scheduled for removal.
 */
#ifndef AV_NOWARN_DEPRECATED
#if GCC_VERSION_AT_LEAST(4,6)
#    define AV_NOWARN_DEPRECATED(code)                            \
  _Pragma("GCC diagnostic push")                                  \
  _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"") \
  code                                                            \
  _Pragma("GCC diagnostic pop")
#else
#    define AV_NOWARN_DEPRECATED(code) code
#endif
#endif


#ifndef _unused
#if defined(__GNUC__)
#    define _unused __attribute__((unused))
#else
#    define _unused
#endif
#endif

/** Mark a variable as used and prevent the compiler from optimizing it away.
 * This is useful for variables accessed only from inline assembler without the
 * compiler being aware.
 */
#ifndef _used
#if GCC_VERSION_AT_LEAST(3,1)
#    define _used __attribute__((used))
#else
#    define _used
#endif
#endif

#ifndef _alias
#if GCC_VERSION_AT_LEAST(3,3)
#   define _alias __attribute__((may_alias))
#else
#   define _alias
#endif
#endif

#ifndef _uninit
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
#    define _uninit(x) x=x
#else
#    define _uninit(x) x
#endif
#endif

#ifdef __GNUC__
#    define __builtin_constant_p __builtin_constant_p
#    define av_printf_format(fmtpos, attrpos) __attribute__((__format__(__printf__, fmtpos, attrpos)))
#else
#    define av_builtin_constant_p(x) 0
#    define av_printf_format(fmtpos, attrpos)
#endif
#endif

/*! Stronly Pure Function Tag. */
#define pure _pure wuur
/*! Stronly Pure Function Tag. */
#define spure _const wuur

#ifdef __cplusplus
}
#endif
