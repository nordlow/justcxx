/*! \file pathops.hpp
 * \brief Path Operations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-04-23 04:04
 * \todo Create alternative versions of path_add() for std::string that uses R-value references.
 */

#pragma once
#include <cstring>
#include <string>
#include "pathsep.h"
#include "cc_features.h"

/*! Get Value of \c PATH Environment Variable.
 */
template<class T>
inline T getpath() {
#if _WIN32
    // Handle Unicode, just remove if you don't want/need this. convert_to_utf8
    // uses WideCharToMultiByte in the Win32 API
    return convert_to_utf8( _wgetenv(L"PATH") );
#else
    return ::getenv("PATH");
#endif
}

/*! Append Paths \p cdir and \p sfil.
 * \param[in] canon_flag flags for \em canonicalized path.
 * \see Emacs' \c expand-file-name()
 * \see boost::filesystem::path::operator/
 * \todo Replace with boost::filesystem::path::operator/
 */
template<class T>
inline pure T path_add(const T& cdir,
                       const T& sfil,
                       bool canon_flag = true)
{
    const size_t clen = cdir.size();
    const size_t flen = sfil.size();
    size_t cskip = 0, fskip = 0;
    if (canon_flag) {
        size_t i;
        for (i = 0; i < clen and cdir[clen-1-i] == PATH_SEP; i++); cskip = i;
        for (i = 0; i < flen and sfil[       i] == PATH_SEP; i++); fskip = i;
    }

    const size_t cadj = clen-cskip; // adjusted length of \p cdir
    const size_t fadj = flen-fskip; // adjusted length of \p sfil

    const size_t full_len = cadj+1 + fadj;
#if 0
    char fdat[full_len+1];
    ::memcpy(fdat,          cdir.data(),       cadj); fdat[cadj] = PATH_SEP;
    ::memcpy(fdat + cadj+1, sfil.data()+fskip, fadj); fdat[full_len] = '\0';
    return T(fdat, full_len);                   // full path
#else  // \Note This is faster!
    T full(full_len, 0);                        // full path
    char * fdat = const_cast<char*>(full.data()); // a bit dirty, but fully functional
    ::memcpy(fdat,          cdir.data(),       cadj); fdat[cadj] = PATH_SEP;
    ::memcpy(fdat + cadj+1, sfil.data()+fskip, fadj); fdat[full_len] = '\0';
    return full;
#endif
    // TODO: Use T + operator
}

/*! Append Paths \p cdir, \p dsub and \p sfil.
 * \param[in] canon_flag flags for \em canonicalized path.
 * \see Emacs' \c expand-file-name()
 * \see boost::filesystem::path::operator/
 * \todo Replace with boost::filesystem::path::operator/
 */
template<class T>
inline pure T
path_add(const T& cdir,
         const T& dsub,
         const T& sfil,
         bool canon_flag = true)
{
    const size_t clen = cdir.size();
    const size_t slen = dsub.size();
    const size_t flen = sfil.size();
    size_t cskip = 0, sskip = 0, fskip = 0;
    if (canon_flag) {
        size_t i;
        for (i = 0; i < clen and cdir[clen-1-i] == PATH_SEP; i++); cskip = i;
        for (i = 0; i < slen and dsub[slen-1-i] == PATH_SEP; i++); sskip = i;
        for (i = 0; i < flen and sfil[       i] == PATH_SEP; i++); fskip = i;
    }

    const size_t cadj = clen-cskip;
    const size_t sadj = slen-sskip;
    const size_t fadj = flen-fskip;

    const size_t full_len = cadj+1 + sadj+1 + fadj;
    T full(full_len, 0);                        // full path
    char * fdat = const_cast<char*>(full.data()); // a bit dirty, but fully functional
    ::memcpy(fdat,                   cdir.data(),       cadj); fdat[cadj] = PATH_SEP;
    ::memcpy(fdat + cadj+1,          dsub.data(),       sadj); fdat[cadj+1 + sadj] = PATH_SEP;
    ::memcpy(fdat + cadj+1 + sadj+1, sfil.data()+fskip, fadj); fdat[full_len] = '\0';
    return full;
    // TODO: Use T + operator
}

template<class T> inline pure bool is_root_path(const T& path)     { return (path.size() == 1 and path[0] == PATH_SEP); }
template<class T> inline pure bool is_absolute_path(const T& path) { return (path.size() >= 1 and path[0] == PATH_SEP); }
template<class T> inline pure bool is_relative_path(const T& path) { return (path.size() >= 1 and path[0] == '.'); }

inline pure bool is_root_path(const char* path)     { return (path[0] == PATH_SEP and path[1] == '\0'); }
inline pure bool is_absolute_path(const char* path) { return (path[0] == PATH_SEP and path[1] != '\0'); }
inline pure bool is_relative_path(const char* path) { return (path[0] == '.'      and path[1] != '\0'); }

template<class T> inline pure bool is_local_path(const T& path) { return not (is_absolute_path(path) or
                                                                                               is_relative_path(path)); }

template<class T> inline pure bool is_DorDD_path(const T& path) {
    return ((path.size() == 1 and
             path[0] == '.') or
            (path.size() == 2 and
             path[0] == '.' and
             path[1] == '.'));
}

template<class T> inline pure bool is_DorDD_path(const char* path) {
    return ((path[0] == '.' and
             path[1] == '\0') or
            (path[0] == '.' and
             path[1] == '.' and
             path[2] == '\0'));
}
