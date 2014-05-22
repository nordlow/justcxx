/*! \file show_all.hpp
 * \brief Pretty Print/Output All.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-03-02 17:28
 * \todo Support libc++ header guard macro names.
 * (STL) Container Headers must be included *before* this header is included.
 */

#pragma once
#include "show.hpp"

#if defined (_GLIBCXX_TUPLE)    ///< If <tuple> was included
#  include "show_tuple.hpp"
#endif
#if defined (_GLIBCXX_STRING) || defined(_BASIC_STRING_H) ///< If <string> was included
#  include "show_string.hpp"
#endif
#if defined (_GLIBCXX_ARRAY) ///< If <array> was included
#  include "show_array.hpp"
#endif
#if defined (_GLIBCXX_VECTOR) || defined (_STL_VECTOR_H) ///< If <vector> was included
#  include "show_vector.hpp"
#endif
#if defined (_GLIBCXX_LIST) || defined (_STL_LIST_H) ///< If <list> was included
#  include "show_list.hpp"
#endif
#if defined (_GLIBCXX_FORWARD_LIST) || defined (_FORWARD_LIST_H) ///< If <forward_list> was included
#  include "show_forward_list.hpp"
#endif

#if defined (_GLIBCXX_BITSET)   ///< If <bitset> was included
#  include "show_bitset.hpp"
#endif
#if defined (_GLIBCXX_TR2_DYNAMIC_BITSET) ///< If <tr2/dynamic_bitset> was included
#  include "show_dynamic_bitset.hpp"
#endif
#if defined (BOOST_DYNAMIC_BITSET_HPP) ///< If <boost/dynamic_bitset.hpp> was included
#  include "show_boost_dynamic_bitset.hpp"
#endif

#if defined (_GLIBCXX_SET) || defined (_STL_SET_H) ///< If <set> was included
#  include "show_set.hpp"
#endif
#if defined (_GLIBCXX_UNORDERED_SET) || defined (_UNORDERED_SET_H) ///< If <unordered_set> was included
#  include "show_unordered_set.hpp"
#endif
#if defined (_GLIBCXX_MAP) || defined (_STL_MAP_H) ///< If <map> was included
#  include "show_map.hpp"
#endif
#if defined (_GLIBCXX_UNORDERED_MAP) || defined (_UNORDERED_MAP_H) ///< If <unordered_map> was included
#  include "show_unordered_map.hpp"
#endif

#if defined (_GLIBCXX_MEMORY) || defined (_SHARED_PTR_H) ///< If <memory> was included
template<class T> inline std::ostream& show(std::ostream& os, std::shared_ptr<T>& a) { return os << "*" << a.get(); }
#endif

#if defined (_GLIBCXX_MEMORY) || defined (_UNIQUE_PTR_H) ///< If <memory> was included
template<class T> inline std::ostream& show(std::ostream& os, std::unique_ptr<T>& a) { return os << "*" << a.get(); }
#endif

#if defined (_GLIBCXX_MEMORY) || defined (_MEMORY_H) ///< If <memory> was included
template<class T> inline std::ostream& show(std::ostream& os, std::weak_ptr<T>& a) { return os << "*" << a.get(); }
#endif

//#  include "show_units.hpp"
//#  include "show_ngram.hpp"
