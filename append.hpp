/*!
 * \file append.hpp
 * \brief Append Algorithms
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

namespace pnw
{

/*!
 * Append the first argument \p a to the second \p b.
 */
template<template<typename> class C, typename T>
inline void append(const C<T> & a, C<T> & b)
{
    uint bn = b.size();
    b.resize(a.size() + bn);
    std::copy(begin(a), end(a), begin(b) + bn);
}

}
