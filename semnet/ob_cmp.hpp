/*!
 * \file ob_cmp.hpp
 * \brief Object Comparisons
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-05-02 12:58
 */

#pragma once
#include "../cmp.h"

/*!
 * \em Sorting Category (for array of \c Ob).
 */
typedef enum {
    PSORT_TDEPTH,               ///< Tree Depth.
    PSORT_THEIGHT,              ///< Tree Height.
    PSORT_CREATION_TIME,        ///< Create Time.
    PSORT_STATUS_CHANGE_TIME,   ///< Status Change Time.
    PSORT_ACCESS_TIME,          ///< Access Time.
    PSORT_MOD_TIME              ///< Modification Time.
} PSORT_t;

#if 0
class Ob;

/*! Sort \p pA and \p pB according to their pointer value.
 */
int ob_cmp_name(const Ob * pA, const Ob * pB);
int ob_cmp_csize(const Ob * pA, const Ob * pB);

/* ---------------------------- Group Separator ---------------------------- */

int voidp_ob_cmp_std(const void * a, const void * b);
int voidp_ob_cmp_type_addr(const void * a, const void * b);
int voidp_ob_cmp_name(const void * a, const void * b);
void parr_sort(Ob ** obs, size_t obs_N, PSORT_t psort);
#endif
