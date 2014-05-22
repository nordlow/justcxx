/*! \file cstring.hpp
 * \brief Wrappers for Small Constant String Optimized for Small Strings.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-10-25 10:58
 *
 * \see http://stackoverflow.com/questions/10789740/passing-stdstring-by-value-or-reference
 * \see http://stackoverflow.com/questions/10231349/are-the-days-of-passing-const-stdstring-as-a-parameter-over
 *
 * \see http://conststring.sourceforge.net/
 * \note See: http://gameangst.com/?p=246 for details on \em Explicit Template Instantiation Declarations.
 *
 * \see GCC's \c __versa_string in ext/vstring.h
 */

#pragma once

// #include <boost/const_string/const_string.hpp>
// #include <boost/const_string/concatenation.hpp>
// #include <boost/const_string/io.hpp>
// #include <boost/functional/hash.hpp>
// #include <boost/const_string/format.hpp>

#include <ext/vstring.h>

// typedef class boost::const_string<char> csc;
// typedef class boost::const_string<wchar_t> csw;
// namespace std   { template<> struct hash<csc> { size_t operator()(const csc& x) const { return std::_Hash_impl::hash(x.data(), x.size()); } }; }

/*! Output \p a to \p os. */
#if 0
#include <ostream>
template<class T> inline std::ostream& operator << (std::ostream& os, const boost::const_string<T>& a) { return os.write(a.data(), a.size()); }
template<class T> inline int compare2(const boost::const_string<T>& a, const char* b, size_t nb) { return a.compare(0, a.size(), b, nb); }
template<class T> inline int compare2(const boost::const_string<T>& a, const char* b)            { return a.compare(0, a.size(), b); }
/*! Hashing. Alternative: return h(x.c_str(), x.size()); std::hash<const char*> h; */
namespace boost { template<> struct hash<csc> { size_t operator()(const csc& x) const { return std::_Hash_impl::hash(x.data(), x.size()); }}; }
#endif

// Note: We cannot yet instantiate all members of const_string because they are not defined (yet)!
//extern template class boost::const_string<char>;

#if defined(__GNUC__)
typedef __gnu_cxx::__versa_string<char> csc; ///< GCC: Pick Small-String-Optimized (SSO) String Implementation
#else
typedef std::string csc;        ///< Others: Pick Standard.
#endif

template<class C> inline bool equal(const C& a,
                                    const typename C::value_type b) { return (a.size() == 1 and
                                                                              *begin(a) == b); }
template<class C> inline bool equal(const C& a,
                                    const typename C::value_type& b0,
                                    const typename C::value_type& b1) { return (a.size() == 2 and
                                                                                *begin(a) == b0 and
                                                                                *(begin(a) + 1) == b1); }
template<class C> inline bool operator == (const C& a,
                                           const typename C::value_type& b) { return equal(a, b); }
