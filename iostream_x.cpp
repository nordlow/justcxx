#include "iostream_x.hpp"
#include <vector>
#include <cstdlib>
#include <iostream>
#include "show.hpp"

std::string open_temporary(std::ofstream& f,
                           std::string tag,
                           std::string path)
{
    if (tag.size()) {
        path += (tag + ".");
    }
    path += "XXXXXX";
    std::vector<char> dst_path(path.begin(),
                               path.end());
    dst_path.push_back('\0');
    const int fd = mkstemp(&dst_path[0]);
    if (fd != -1) {
        path.assign(dst_path.begin(),
                    dst_path.end() - 1);
        f.open(path.c_str(),
               std::ios_base::trunc |
               std::ios_base::out);
        close(fd);
    }
    return path;
}

void test_open_temp(std::string tag)
{
    std::ofstream os;
    open_temporary(os, tag);
    if (os.is_open()) {
        os << "hello, dude" << std::endl;
    }
}
