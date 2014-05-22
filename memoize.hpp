/*! \file memoize.hpp
 * \brief (Persistent) Memoize Meta-Function.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-03-20 21:00
 *
 * \see http://cpptruths.blogspot.com/2012/01/general-purpose-automatic-memoization.html?utm_source=feedburner&utm_medium=feed&utm_campaign=Feed%3A+CppTruths+%28C%2B%2B+Truths%29
 * \see http://stackoverflow.com/questions/5353141/c0x-weird-code
 * \see http://slackito.com/2011/03/17/automatic-memoization-in-cplusplus0x/
 * \see http://www.reddit.com/r/cpp/comments/12zw1u/memoization_in_c11/
 * \see http://www.reddit.com/r/cpp/comments/15t7uv/memoization_in_c/
 *
 * Memoization is an optimization technique which consists in “remembering”
 * (i.e.: caching) the results of previous calls to a function, so that repeated
 * calls with the same parameters are resolved without repeating the original
 * computation.
 */

/* \todo Disk-Cache these \c kp and \c desc to \c
 * pathapp(dirof(path()),
 * ".cognia/path()/<INPUT-FUN-PARAM-OUTDATA>" where
 * - INPUT is either
 *   - filename itself as path()
 *   - filesystem hash of path()
 *   - or image color plus image data itself in \c mat.
 * - FUN is either SIFT or SURF
 * - OUTPUT is output in hex
 * FUN is cv::SIFT or cv::SURF and PARAM are their parameters,
 * using \c memoize() or convenince wrapper file_operation_memoize(dir, local_filename, ...)
 * and CRC on file name, contents and operation. Can we limit memoize() variadics to value_types only?
 *
 * \todo Type Trait is_base_of may be useful here?
 *
 * \todo Overload serialization similar to what is done in enforce.hpp
 *
 * */

#pragma once

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

// #define HAVE_UNORDERED_MAP 1
// #define HAVE_TR1_UNORDERED_MAP 1
// #define HAVE_BOOST_UNORDERED_MAP_HPP 1
// #define TBB_CONCURRENT_UNORDERED_MAP_H 1

#include <tuple>
#include "tuple_hash.hpp"

#include <functional>
#include <memory>

#include <type_traits> // <boost/type_traits/is_pointer.hpp>

// ----------------------------------------------------------------------------

#if defined(HAVE_UNORDERED_MAP)
# include <unordered_map>
using std::unordered_map;
using std::tuple;
#elif defined(HAVE_TR1_UNORDERED_MAP)
# include <tr1/unordered_map>
using std::tr1::unordered_map;
using std::tuple;
#elif defined(HAVE_BOOST_UNORDERED_MAP_HPP)
# include <boost/unordered_map.hpp>
# include "boost/tuple/tuple.hpp"
using boost::unordered_map;
using boost::tuple;
#else
# include <unordered_map>       // try anyway
using std::unordered_map;
using std::tuple;
#endif

// ----------------------------------------------------------------------------

/*! \em Memoize Function \p func and return it.
 * \tparam Ret is Return Type of Function to memoize.
 * \return memoized function including a closure for \p cache
// \see http://cpptruths.blogspot.com/2012/01/general-purpose-automatic-memoization.html
 */
template<class Ret, class... Args, class Map = typename std::unordered_map<std::tuple<Args...>, Ret>>
std::function<Ret (Args...)> memoize_helper(Ret (*func) (Args...))
{
    auto cache = std::make_shared<Map>(); // arguments to return value map
    return ([=](Args... args) mutable { // \note [=] is needed for cache to be \em copied into the scope the lambda (closure)
            tuple<Args...> t(args...);
            if (cache->find(t) == cache->end()) {
                (*cache)[t] = func(args...);
            }
            return (*cache)[t];
        });
}
// template<class Ret, class... Args>
// std::function<Ret (Args...)> memoize_helper(std::function<Ret (Args...)> func)
// {
//     return memoize_helper();
// }

enum class Cache : unsigned int {
    NO_RECLAIM,                 ///< Don't Reclaim Memory.
        RECLAIM                 ///< Reclaim Memory Allocated (after last call to function).
        };

/*! Memoized Call of Function.
 * \example memoized_recursion(fibonacci, Cache::RECLAIM);
// \see http://cpptruths.blogspot.com/2012/01/general-purpose-automatic-memoization.html
 */
template<class F_ret, class... F_args>
std::function<F_ret (F_args...)>
memoized_recursion(F_ret (*func)(F_args...), Cache c = Cache::NO_RECLAIM)
{
    typedef std::function<F_ret (F_args...)> FunctionType;
    static std::unordered_map<decltype(func), FunctionType> functor_map;
    if(Cache::RECLAIM == c)
        return functor_map[func] = memoize_helper(func);
    if(functor_map.find(func) == functor_map.end())
        functor_map[func] = memoize_helper(func);
    return functor_map[func];
}

// ----------------------------------------------------------------------------

template<class Sig, Sig funcptr>
struct static_memoizer;
template<class F_ret, class... F_args, F_ret (*func)(F_args...)>
struct static_memoizer<F_ret (*)(F_args...), func>
{
    static
    std::function<F_ret (F_args...)> &
    get(Cache c = Cache::NO_RECLAIM)
    {
        static std::function<F_ret (F_args...)> mfunc (memoize_helper(func));
        if (Cache::RECLAIM == c) { mfunc = memoize_helper(func); }
        return mfunc;
    }
};

/*! Simplifies the use of the static_memoizer. It extracts the type of the
 *  function pointer using decltype and passes it (the type) as the first
 *  parameter of the template. The second parameter is the actual function
 *  pointer. Passing the function pointer as a template parameter is important
 *  because many functions potentially share the same signature but never the
 *  same pointer.
 */
#define STATIC_MEMOIZER(func) static_memoizer<decltype(&func), &func>

// ----------------------------------------------------------------------------

/*! The static objects used by the static_memoizer are different from that of
 *  memoized_recursion. So we've to rewrite the fibonacci function to use the
 *  static_memoizer. */
inline unsigned long static_memoized_fibonacci(unsigned n)
{
    return (n < 2) ? n :
        STATIC_MEMOIZER(static_memoized_fibonacci)::get()(n - 1) +
        STATIC_MEMOIZER(static_memoized_fibonacci)::get()(n - 2);
}

// ----------------------------------------------------------------------------

#if defined(TBB_CONCURRENT_UNORDERED_MAP_H)
# include <tbb/concurrent_unordered_map.h>
#else
# include <mutex>
#endif

/*! \em Concurrent \em Memoize function \p func and return it.
 * \return concurrently memoized function including a closure for \p cache
 */
template<class Ret, class... Args>
std::function<Ret (Args...)> concurrent_memoize(std::function<Ret (Args...)> func)
{
#if defined(TBB_CONCURRENT_UNORDERED_MAP_H)
    tbb::concurrent_unordered_map<std::tuple<Args...>, Ret> cache;
#else
    std::unordered_map<std::tuple<Args...>, Ret> cache;
#endif
    return ([=](Args... args) mutable { // \note [=] is needed for cache to be \em copied into the scope the lambda (closure)
            std::tuple<Args...> t(args...);
#if ! defined(TBB_CONCURRENT_UNORDERED_MAP_H)
            static std::mutex mutex;
            std::lock_guard<std::mutex> guard(mutex);
#endif
            if (cache.find(t) == cache.end())
                cache[t] = func(args...);
            return cache[t];
        });
}

/*! \em Persistent \em Memoize function \p func and return it.
 * \return memoized function
 */
template<class Ret, class... Args>
std::function<Ret (Args...)> persistent_memoize(std::function<Ret (Args...)> func)
{
    /* \todo: Flush \c unordered_map to disk by using definitions for
     * serializing tuples to disk using hash-key of serializiation of
     * (FUNCTION-NAME TUPLE-1 ... TUPLE-N) */
    return memoize(func);
}

// Limit memoize \p args to non-pointers only or serialize *args if a point otherwise
template<class T> void memoize_serialize(const T& a) { }
