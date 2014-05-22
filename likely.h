/*!
 * \file likely.h
 * \brief CPU Branch Predicition Compiler Hints.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Wrap \c likely(x) or \c unlikely(x) around an expression \c x if you, through
 * \em apriori knowledge, know that it is either \em likely or \em unlikely that
 * \c x evalutes to \c true (non-zero).
 *
 * \ref http://www.antoarts.com/
 */

#pragma once

//#undef likely
//#undef unlikely

#if __GNUC__ >= 3               /**< Supported by GCC 3 and later. */
#  define likely(x)   __builtin_expect (!!(x), 1) /**< Hint Compiler that \p x is \em likely to be \c true. */
#  define unlikely(x) __builtin_expect (!!(x), 0) /**< Hint Compiler that \p x is \em unlikely to be \c true. */
#else
#  define likely(x) (x)          /**< No Operation. */
#  define unlikely(x) (x)        /**< No Operation. */
#endif
