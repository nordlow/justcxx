#include "dup.hpp"
#include "ob_cmp.hpp"
#include "../qsort_mt.h"

#if 0
void parr_update_duplicates(Ob ** obs, size_t obs_N, PDUP_t dupT, Refs& new_dups)
{
    REL_t pHUB = REL_PARENT;
    REL_t pEXA = REL_CHILD;

    cmp_function_t *cmp = nullptr;

    // determine \c cmp function (functional argument)
    switch (dupT) {
    case PDUP_NAME:
        cmp = voidp_ob_cmp_name;
        break;
    case PDUP_CONTENT:
        cmp = voidp_ob_cmp_std;
        break;
    default: PTODO("Cannot handle dupT:%d (yet)\n", dupT); return; break;
    }

    qsort_mt(obs, obs_N, sizeof(Ob*), cmp, 0,0);

    for (size_t i = 0; i < obs_N;) { // for all obs
        size_t j = i+1;
        // iterate j to beyond first non-duplicate
        for (; j < obs_N; j++) {	// for all obs following obs[i]
            if (cmp(&obs[i], &obs[j]) != 0) { break; }
        }
        const size_t dupN = j-i;    // multiplicity (2 or more for duplicates)
        if (dupN >= 2) {		// obsj[i ... i+dupN-1] are duplicates
            Dup * pdup = gen::dup(dupT);
            new_dups.app(pdup);
            for (size_t k = 0; k < dupN; k++) { // for all duplicates
                obs[i+k]->net_disconnectM(pHUB); // disconnect from all previous
                net_connectS(pEXA, obs[i+k], // connect to new
                             pHUB, pdup,
                             true);

            }
        } else {			// obs[i] has no duplicates
            obs[i]->net_disconnectM(pHUB); // disconnect from all previous
        }
        i = j;
    }
}

void parr_update_name_duplicates(Ob ** obs, size_t obs_N, Refs& new_dups)
{
    parr_update_duplicates(obs, obs_N, PDUP_NAME, new_dups);
}

void parr_update_content_duplicates(Ob ** obs, size_t obs_N, Refs& new_dups)
{
    parr_update_duplicates(obs, obs_N, PDUP_CONTENT, new_dups);
}
#endif
