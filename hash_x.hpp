/*! \file hash_x.hpp
 * \brief Hashing Extensions.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-03-26 22:47
 * \see http://stackoverflow.com/questions/8027368/are-there-no-specializations-of-stdhash-for-standard-containers
 * \see \c hash_combine from boost/functional/hash/hash.hpp
 */

#pragma once

namespace std {
#if defined (_GLIBCXX_ARRAY) || defined (_STL_ARRAY_H) ///< If <array> was included
template<class T, size_t N> struct hash<array<T, N> >
{
    typedef array<T, N> A; // argument type
    typedef size_t R; // result type
    R operator()(const A& a) const
    {
        hash<T> hasher;         // element hash
        R h = 0;
        for (R i = 0; i < N; ++i) {
            h = h * 31 + hasher(a[i]);
        }
        return h;
    }
};
#endif
}
