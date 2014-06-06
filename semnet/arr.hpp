/*! \file arr.hpp
 * \brief Object Array Operations.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <stdlib.h>

class Ob;

namespace semnet {

/*! Investigate (previous) selections and their interrelational properties,
 * such as if - they are of the same type.
 * \see calc_stat()
 */
void parr_calc_stat(Ob ** obs, size_t obs_N, bool * homo_flag_ret);

}
