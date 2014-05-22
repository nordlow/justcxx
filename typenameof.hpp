/*! \file typenameof.hpp
 * \brief Lookup Names of Types as C strings or \c std::string.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-06 12:54
 * \todo Use typeid(value).name() for RTTI classes
 */

#pragma once
#include "cc_features.h"
#include <cstdint>
#include <type_traits>
#include <utility>
#include <string>
#include <cwchar>

template<class T> inline spure const char* typenameof() { return ""; }
// Special Types
template<> inline spure const char* typenameof<void>() { return "void"; }
template<> inline spure const char* typenameof<bool>() { return "bool"; }
template<> inline spure const char* typenameof<char>() { return "char"; }
template<> inline spure const char* typenameof<wchar_t>() { return "wchar_t"; }
// Signed Integral Types
template<> inline spure const char* typenameof<int8_t>() { return "int8_t"; }
template<> inline spure const char* typenameof<int16_t>() { return "int16_t"; }
template<> inline spure const char* typenameof<int32_t>() { return "int32_t"; }
template<> inline spure const char* typenameof<int64_t>() { return "int64_t"; }
// Unsigned Integral Types
template<> inline spure const char* typenameof<uint8_t>() { return "uint8_t"; }
template<> inline spure const char* typenameof<uint16_t>() { return "uint16_t"; }
template<> inline spure const char* typenameof<uint32_t>() { return "uint32_t"; }
template<> inline spure const char* typenameof<uint64_t>() { return "uint64_t"; }
// Floating Point Types
template<> inline spure const char* typenameof<float>() { return "float"; }
template<> inline spure const char* typenameof<double>() { return "double"; }
template<> inline spure const char* typenameof<long double>() { return "long double"; }

// String
template<> inline spure const char* typenameof<std::string>() { return "std::string"; }

//template<class T> inline spure const char* typenameof(const T & a) { return ""; }
template<class T> inline spure const char* typenameof(const T & a) { return typenameof<decltype(a)>(); }

template<class T> inline spure const char* typenameof(const typename std::enable_if<std::is_class<T>::value, T>::type & a) { return "class"; }
template<class T> inline spure const char* typenameof(const typename std::enable_if<std::is_enum<T>::value, T>::type & a) { return "enum"; }
template<class T> inline spure const char* typenameof(const typename std::enable_if<std::is_union<T>::value, T>::type & a) { return "union"; }
template<class T> inline spure const char* typenameof(const typename std::enable_if<std::is_function<T>::value, T>::type & a) { return "function"; }
/* POD Types */
template<class T> inline spure const char* typenameof(std::string & a) { return "std::string"; }
template<class T> inline spure const char* typenameof(const typename std::enable_if<std::is_integral<T>::value, T>::type & a) { return "integral"; }
template<class T> inline spure const char* typenameof(const typename std::enable_if<std::is_floating_point<T>::value, T>::type & a) { return "floating_point"; }

template<class T> inline spure std::string typenameof(const typename std::enable_if<std::is_array<T>::value, T>::type & a) { return std::string(typenameof(*a)) + "[]"; }
template<class T> inline spure std::string typenameof(const typename std::enable_if<std::is_pointer<T>::value, T>::type & a) { return std::string(typenameof(*a)) + "*"; }

// Pair
template<class T, class U> inline spure std::string typenameof(const std::pair<T,U>& a) { return (std::string("std::pair<") +
                                                                                                  typenameof<T>() + "," +
                                                                                                  typenameof<U>() + ">"); }
