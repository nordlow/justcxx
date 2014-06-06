#include "arr.hpp"
#include "ob.hpp"

namespace semnet {

void
parr_calc_stat(Ob ** obs, size_t obs_N, bool * homo_flag_ret)
{
    *homo_flag_ret = 1;	 /* flags that all objects have same oT  */

    OB_t prev_oT = OB_undefined_;
    for (size_t i = 0; i < obs_N; i++) {
        if (const Ob *ob = obs[i]) {	/* object exists */
            if (prev_oT != OB_undefined_ and /* not the first one */
                prev_oT != ob->get_type()) {
                *homo_flag_ret = 0;
            }
            prev_oT = ob->get_type();
        }
    }
}
}
