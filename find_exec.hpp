/*! \file find_exec.hpp
 * \brief Find Executable Program.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-07-02 17:10
 * \see http://www.boost.org/doc/libs/1_49_0/doc/html/string_algo/usage.html#id3183933
 */

#pragma once
#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/range/algorithm/copy.hpp>
#include "pathops.hpp"

/*! Find Executable Command \p cmd using \c PATH Environment.
 * \see Emacs \c executable-find()
 */
template<class T>
inline pure T find_executable_by_name(const T& cmd, bool canon_flag = true)
{
    auto path = getpath<T>();
    typedef boost::split_iterator<typename T::iterator> ssi;
    for (auto i = make_split_iterator(path,
                                      boost::algorithm::first_finder(ENV_SEP,
                                                                     boost::algorithm::is_iequal())); i != ssi(); ++i) {
        auto dir = boost::copy_range<T>(*i);
        auto full = path_add(dir, cmd, canon_flag);
        if (boost::filesystem::exists(full)) { return full; }
    }
    return cmd;
}
//extern template inline pure std::string find_executable_by_name(const std::string& cmd);

inline pure std::string find_executable_by_name(const char* cmd, bool canon_flag = true) {
    return find_executable_by_name(std::string(cmd), canon_flag);
}
