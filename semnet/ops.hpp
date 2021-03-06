/*!\file ops.hpp
 * \brief Provide Object Operations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-08-14 10:40
 */

#pragma once
#include "OP_enum.hpp"
#include "file.hpp"
#include <vector>

namespace semnet { namespace filesystem {

std::vector<OP_t> provide_ops(const std::vector<File*>& files);

}}
