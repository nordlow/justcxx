/*!
 * \file tuple_hash.hpp
 * \brief Hash Functions for Putting Types Maps.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-07 10:58
 *
 * Supported maps are in
 * - \c std::map,
 * - \c std::unordered_map and
 * - \c tbb::concurrent_unordered_map.
 *
 * Note: from http://www.boost.org/doc/libs/1_48_0/doc/html/hash/custom.html
 * "Extending boost::hash for a custom data type"
 *  boost::hash is implemented by calling the function hash_value. The namespace
 *  isn't specified so that it can detect overloads via argument dependant
 *  lookup. So if there is a free function hash_value in the same namespace as a
 *  custom type, it will get called.
 *
 * \see http://stackoverflow.com/questions/4948780/magic-numbers-in-boosthash-combine
 * \see http://stackoverflow.com/questions/7110301/generic-hash-for-tuples-in-unordered-map-unordered-set
 * \see http://stackoverflow.com/questions/3611951/building-an-unordered-map-with-tuples-as-keys
 * \see http://www.boost.org/doc/libs/1_48_0/doc/html/hash/custom.html
 */

#pragma once

#include <tuple>
# include <tbb/concurrent_unordered_map.h>
# include <boost/unordered_map.hpp>

// ----------------------------------------------------------------------------
// Code from boost
// \note Reciprocal of the golden ratio helps spread entropy and handles duplicates.
// See Mike Seymour in magic-numbers-in-boosthash-combine: http://stackoverflow.com/questions/4948780

namespace std {
namespace {                     // anonymous namespace

template<class T>
inline void hash_combine(std::size_t& seed, T const& v) {
    seed ^= hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// Recursive template code derived from Matthieu M.
template<class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl {
    static void apply(size_t& seed, Tuple const& tuple) {
        HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
        hash_combine(seed, get<Index>(tuple));
    }
};

template<class Tuple>
struct HashValueImpl<Tuple,0> {
    static void apply(size_t& seed, Tuple const& tuple) {
        boost::hash_combine(seed, get<0>(tuple));
    }
};
}

template<typename ... TT>
struct hash<std::tuple<TT...>> {
    size_t operator()(std::tuple<TT...> const& tt) const {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
        return seed;
    }

};
}

// ----------------------------------------------------------------------------
// TBB

#if defined(TBB_CONCURRENT_UNORDERED_MAP_H)
namespace tbb {
// Template class for hash compare
template<class... Args>
class tbb_hash
{
public:
    tbb_hash() {}
    size_t operator()(const std::tuple<Args...>& key) const { return interface5::tbb_hasher(key); }
};
namespace interface5 {
template<class... Args>
inline size_t tbb_hasher( const std::tuple<Args...> & p )
{
    // \todo: Implement
    return 0;
}
}
}
#endif

// ----------------------------------------------------------------------------
// STL/STD

// WARNING: This fails.
// namespace std {
// template<typename First, typename... Values>
// struct hash<std::tuple<First, Values...>> {
//     typedef size_t result_type;
//     typedef std::tuple<First, Values...> argument_type;
//     size_t operator()(std::tuple<First, Values...> const& e) const {
//         std::hash<First> left;
//         std::hash<std::tuple<Values...>> right;
//         return left() ^ right();
//         // Xor due to  Alexandre C's comment.
//     }
// };
// template<>
// struct hash<std::tuple<>> {
//     typedef size_t result_type;
//     typedef std::tuple<> argument_type;
//     std::size_t operator()(std::tuple<> const& e) const {
//         return 1;
//     }
// };
// }
