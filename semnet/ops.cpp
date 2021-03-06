#include "ops.hpp"
#include "dir.hpp"
#include <vector>

namespace semnet { namespace filesystem {

std::vector<OP_t> provide_ops(const std::vector<File*>& files)
{
    std::vector<OP_t> ops;
    switch (files.size()) {
    case 1: {
        auto file = dynamic_cast<File*>(files[0]);
        if (file) {
            if (file->is_writable()) {
                ops.push_back(OP_DELETE);
            }
        }
        break;
    }
    case 2: {
        auto file = dynamic_cast<File*>(files[0]);
        auto dir = dynamic_cast<Dir*>(files[1]);
        if (file and dir) {
            if (dir->is_writable()) {
                ops.push_back(OP_COPY_TO);
                auto sub = dir->contains(file->get_pathL());
                if (sub) {
                    ops.push_back(OP_COPY_AND_OVERWRITE_TO);
                }
                if (file->is_deletable()) {
                    ops.push_back(OP_MOVE_TO);
                    if (sub) {
                        if (dir->contains(file->get_pathL())) {
                            ops.push_back(OP_MOVE_AND_OVERWRITE_TO);
                        }
                    }
                }
            }
        }
        break;
    }
    }
    return ops;
}

}}
