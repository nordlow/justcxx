/*! \file iostream_x.hpp
 * \brief Extensions to iostream.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-07-15 14:45
 */

#pragma once
#include <fstream>
#include <string>

/*! Relate an Existing Standard \c ofstream to a temporary file.
 * \see http://stackoverflow.com/questions/499636/how-to-create-a-stdofstream-to-a-temp-file
 */
std::string open_temporary(std::ofstream& f,
                           std::string tag = "",
                           std::string path = "/tmp/");

/*! Create a New Standard \c ofstream to a temporary file.
 * \todo Activate when "Missing move and swap operations" are added to (GCC) libstdc++
 * \see http://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html
 * \see http://stackoverflow.com/questions/4825851/how-to-return-an-fstream-c0x
 * \see http://stackoverflow.com/questions/4869684/c0x-library-that-implements-move-for-fstreams?lq=1
 */
#if 0
inline std::ofstream make_temporary_ofstream(std::string path) {
    std::ofstream os;
    open_temporary(os);
    return std::move(os);
}
#endif

void test_open_temp(std::string tag = "");
