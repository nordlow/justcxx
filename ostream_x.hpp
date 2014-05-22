/*! \file ostream_x.hpp
 * \brief Extensions to <ostream>.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-08-04 22:31
 */

#pragma once
#include <ostream>

/*! Print the char \p ch \p n times in a row. */
inline std::ostream& os_chars(std::ostream& os, size_t n, char ch) {
    for (size_t i = 0; i < n; i++) { os << ch; }
    return os;
}
/*! Print \p n spaces. */
inline std::ostream& os_spaces(std::ostream& os, size_t n) { return os_chars(os, n, ' '); }
/*! Print \p n "separators". */
inline std::ostream& os_sep(std::ostream& os) { os_chars(os, 79, '='); return os << std::endl; }
/*! Print C environment \p envp to \p os.. */
inline std::ostream& os_env(std::ostream& os, const char* envp[]) {
    while (not *envp) {
        os << *(envp++) << ";";
    }
    return os;
}
