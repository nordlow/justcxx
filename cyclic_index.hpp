/*!
 * \file cyclic_index.hpp
 * \brief Cyclic Indexing
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-12-03 13:09
 */

#pragma once

namespace pnw
{

// ============================================================================

/*! Perform a cyclic element access in the structure \p a at index \p i. */
template<template<typename> class C, typename T>
inline T & cyclic_at(C<T> & a, size_t i)
{
    return a[i % a.size()];
}

template<template<typename> class C, typename T>
inline const T & cyclic_at(const C<T> & a, size_t i)
{
    return a[i % a.size()];
}

// ============================================================================

}
