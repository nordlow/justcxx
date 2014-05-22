/*!
 * \file perm.h
 * \brief Permutate Arrays.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "reorg.h"

/*! \em Full 3-Permutation/Shift/Rotate to [\p b \p c \p a]. */
#define PERM3_231(a,b,c) { typeof(a) _t=a; a=b; b=c; c=_t; }
/*! \em Full 3-Permutation/Shift/Rotate to [\p c \p a \p b]. */
#define PERM3_312(a,b,c) { typeof(a) _t=a; a=c; c=b; b=_t; }

/*! \em 2-Cycle 4-Permutation/Shift/Rotate to [\p d \p c \p b \p a]. */
#define PERM4_4321(a,b,c,d) { SWAP(a,d); SWAP(b,c); }
/*! \em 2-Cycle 4-Permutation/Shift/Rotate to [\p b \p a \p d \p c]. */
#define PERM4_2143(a,b,c,d) { SWAP(a,b); SWAP(c,d); }
/*! \em Full 4-Permutation/Shift/Rotate to [\p c \p d \p a \p b]. */
#define PERM4_3412(a,b,c,d) { SWAP(a,c); SWAP(b,d); }
/*! \em Full 4-Permutation/Shift/Rotate to [\p b \p c \p d \p a]. */
#define PERM4_2341(a,b,c,d) { typeof(a) _t=a; a=b; b=c; c=d; d=_t; }
/*! \em Full 4-Permutation/Shift/Rotate to [\p b \p d \p a \p c]. */
#define PERM4_2413(a,b,c,d) { typeof(a) _t=a; a=b; b=d; d=c; c=_t;  }
/*! \em Full 4-Permutation/Shift/Rotate to [\p c \p a \p d \p b]. */
#define PERM4_3142(a,b,c,d) { typeof(a) _t=a; a=c; c=d; d=b; b=_t;  }
/*! \em Full 4-Permutation/Shift/Rotate to [\p c \p d \p b \p a]. */
#define PERM4_3421(a,b,c,d) { typeof(a) _t=a; a=c; c=b; b=d; d=_t; }
/*! \em Full 4-Permutation/Shift/Rotate to [\p d \p a \p b \p c]. */
#define PERM4_4123(a,b,c,d) { typeof(a) _t=a; a=d; d=c; c=b; b=_t;  }
/*! \em Full 4-Permutation/Shift/Rotate to [\p d \p c \p a \p b]. */
#define PERM4_4312(a,b,c,d) { typeof(a) _t=a; a=d; d=b; b=c; c=_t;  }

#ifdef __cplusplus
extern "C" {
#endif

void farray_perm(float *a, const float *b, const size_t *p, size_t n);

/*! Permutate b into \p a using the permutation vector c of length \p n.
 * Argument \p a and \p b can be equal.
 * \note \p p must be a permutation vector, otherwise results will be undefined.
 */
void darray_perm(double *a, const double *b, const size_t *p, size_t n);

#ifdef __cplusplus
}
#endif
