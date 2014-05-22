/*! \file begin_end.hpp
 * \brief Non-Member begin() end() Support
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-09-29 01:42
 */

#pragma once

template<class C> auto begin(C& c) -> decltype(c.begin());
template<class C> auto begin(const C& c) -> decltype(c.begin());
template<class C> auto end(C& c) -> decltype(c.end());
template<class C> auto end(const C& c) -> decltype(c.end());
template<class T, size_t N> T* begin(T (&array)[N]);
template<class T, size_t N> T* end(T (&array)[N]);
