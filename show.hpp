/*! \file show.hpp
 * \brief Pretty Print/Output.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see http://louisdx.github.com/cxx-prettyprint/ and git://github.com/louisdx/cxx-prettyprint.git
 * \see http://stackoverflow.com/questions/2330850/correct-way-to-set-maximum-double-precision-for-ostringstream
 * \see http://stackoverflow.com/questions/4850473/pretty-print-c-stl-containers
 * \see Use ostream::precision(std::numeric_limits<double>::digits10);
 *
 * \todo Merge with pretty_printer.cpp
 * \todo Import iostream-pretty_printer.cpp
 * \todo Don't force inclusion of container headers but check them or generalize instead.
 * \todo Use #include <boost/utility/enable_if.hpp> and boost::enable_if<IsContainer<C>>
 * to make code compacter.
 */

#pragma once
#include <ostream>
#include <iomanip>

#include <cwchar>
#include <string>

#include "math_x.h"
#include "cc_features.h"
#include "type_traits_x.hpp"
#include "typenameof.hpp"

namespace std { extern ostream cout; } ///< Instead of including \c iostream

inline void endl(std::ostream& os = std::cout) { os << std::endl; }

// ==========
// Multi-Show

/*! Show \p t to Standard Output (\c std::cout). */
template<class T> inline std::ostream& show_n(std::ostream& os, const T& t) { return os << t; }

#if HAVE_CXX11_VARIADIC_TEMPLATES
/*! @name Variadic Versions. */
/// \{
/*! Show \p v,r.... to \p os. */
template<class T, class... R> inline std::ostream& show_n(std::ostream& os, const T& t, const R& ... r) { show_n(os, t); show_n(os, r...); return os; }
/*! Show \p t,r.... to Standard Output (\c std::cout). */
template<class T, class... R> inline std::ostream& show_n(const T& t, const R&... r) { return show_n(std::cout, t, r...); }
/*! Show \p t,r.... to Standard Output (\c std::cout) ending with newline. */
template<class T, class... R> inline std::ostream& showln_n(const T& t, const R&... r) { show_n(std::cout, t, r...); std::cout << std::endl; return std::cout; }
/// \}
#endif

// Generic.
template<class T> inline std::ostream& show(std::ostream& os, const T& a) { return os << a; }

/// @name Built-In Types
/// \{

// Boolean.
inline std::ostream& show(std::ostream& os, bool a, char ch0='0', char ch1='1') { return os << (a ? ch1 : ch0); }
// Force single-quoted char format for characters.
template<> inline std::ostream& show<char>(std::ostream& os, const char& a) { return os << '\'' << a << '\''; }

// Force integer format.
template<> inline std::ostream& show<int8_t>(std::ostream& os, const int8_t& a) { return os << static_cast<int>(a); }
// Force integer format.
template<> inline std::ostream& show<uint8_t>(std::ostream& os, const uint8_t& a) { return os << static_cast<uint>(a); }
// Force integer format.
template<> inline std::ostream& show<int16_t>(std::ostream& os, const int16_t& a) { return os << a; }
// Force integer format.
template<> inline std::ostream& show<uint16_t>(std::ostream& os, const uint16_t& a) { return os << a; }
// Force integer format.
template<> inline std::ostream& show<int32_t>(std::ostream& os, const int32_t& a) { return os << a; }
// Force integer format.
template<> inline std::ostream& show<uint32_t>(std::ostream& os, const uint32_t& a) { return os << a; }
// Force integer format.
template<> inline std::ostream& show<int64_t>(std::ostream& os, const int64_t& a) { return os << a; }
// Force integer format.
template<> inline std::ostream& show<uint64_t>(std::ostream& os, const uint64_t& a) { return os << a; }
/// \}

/// @name Pointers
/// \{
// Force Double Quotes around C strings.
inline std::ostream& show(std::ostream& os, const char* a) { return os << "\"" << a << "\""; }
/// \}

/// @name STL Types.
/// \{
// Force single-quoted wide char format for characters.
template<> inline std::ostream& show<wchar_t>(std::ostream& os, const wchar_t& a) { return os << "w" << a; }
// Force Double Quotes around C++ strings.
template<> inline std::ostream& show<std::string>(std::ostream& os, const std::string& a) { return os << "\"" << a << "\""; }
/// \}

// Pairs.
template<class T, class U> inline std::ostream& show(std::ostream& os,
                                                     const std::pair<T,U> a,
                                                     const char* opening = "(",
                                                     const char* closing = ")",
                                                     const char* separator = ",") {
    return os << opening << a.first << separator << a.second << closing;
}

// ======
// String

/*! In-Place Show \p a to \p os and return \p a. */
template<class T> inline T in_show(const T& a, std::ostream& os = std::cout) { show(os, a); return a; }

// ====
// Pair

/*! Print Each Element in \p v to \p os. */
template<class T, class U> inline std::ostream& show_each(const std::pair<T,U>& v,
                                                          std::ostream& os = std::cout,
                                                          const char* opening = "(",
                                                          const char* closing = ")",
                                                          const char* separator = ",") {
    os << opening;
    show(os, v.first);
    os << separator;
    show(os, v.second);
    return os << closing;
}

/*! Print \p v to \p os. */
template<class T, class U>
inline std::ostream& operator << (std::ostream& os,
                                  const std::pair<T,U>& v) {
    return show_each(v, os);
}

/*! Print Each Element from \p begin to \p end. */
template<class II>
inline std::ostream& show_each(const II begin, const II end,
                               std::ostream& os = std::cout,
                               const char* opening = "[",
                               const char* closing = "]",
                               const char* separator = " ") {
    os << opening << ' ';
    for (auto it = begin; it != end; it++) {
        show(os, *it);
        os << separator;
        // os << *it << separator;
    }
    os << closing;
    return os;
}

/*! Print Each Element in STL Container \c C \p v to \p os. */
template<class C> inline
typename std::enable_if<is_container<C>::value,
                        std::ostream>::type&
show_each(const C& v,
          std::ostream& os = std::cout,
          const char* opening = "[",
          const char* closing = "]",
          const char* separator = " ") {
    return show_each(begin(v), end(v), os, opening, closing, separator);
}

/*! Print Each Element in the array \p begin of length \p n. */
template<class T>
inline std::ostream& show_n(const T* begin, size_t n,
                            std::ostream& os = std::cout,
                            const char* opening = "[",
                            const char* closing = "]",
                            const char* separator = " ") {
    os << opening << ' ';
    size_t c = 0;
    const auto end = begin + n;
    for (auto it = begin; it != end and c < n; it++, c++) {
        show(os, *it);
        os << separator;
    }
    os << closing;
    return os;
}

/*! Print Each Element at \p begin of length \p n. */
template<class C>
inline std::ostream& show_n(const typename C::const_iterator begin,
                            const typename C::const_iterator end,
                            std::ostream& os = std::cout,
                            const char* opening = "[",
                            const char* closing = "]",
                            const char* separator = " ") {
    os << opening << ' ';
    for (auto it = begin; it != end; it++) {
        show(os, *it);
        os << separator;
    }
    os << closing;
    return os;
}

#include <ctime>
/*! Print \p t to \p os. */
inline std::ostream& operator << (std::ostream& os, const struct timespec& t) {
    return os << t.tv_sec << '.' << std::setw(9) << std::setfill('0') << t.tv_nsec;
}

inline std::ostream& os_cvec(std::ostream& os, const uchar * buf, size_t len, const char* separator = " ") {
    os << '[';
    for (size_t j = 0; j < len; j++) {
        os << std::hex << separator << static_cast<uint>(buf[j]);
    }
    os << ']';
    return os;
}

/*! Show \p t to \p os and then newline. */
template<class T> inline std::ostream& showln(std::ostream& os, const T& t) { show(os, t); return os << std::endl; }
/*! Show \p t standard out and then newline. */
template<class T> inline std::ostream& showln(const T& t) { return showln(std::cout, t); }

/*! Show \p expr's \em expression and \em value. */
#define dshow(expr) { std::cout << __STRING(expr) << " => "; show(std::cout, expr); }
/*! Show \p expr's \em expression and \em value followed by newline. */
#define dshowln(expr) { dshow(expr); std::cout << std::endl; }
