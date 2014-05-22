/*! \file dot.h
 * \brief Dot/Scalar Product.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * When calculating dot-products:
 * - \c farray_dot_8stretched_plainC() should provide sufficient performance for
 *   most applications.
 * - \c farray_dot_8stretched_aligned16(),unaligned(),anyaligned() uses
 *   SSE-instructions if GCC and the target platform supports SSE.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "is_aligned.h"
#include "utils.h"

/*!
 * Calculate Dot (Scalar) Product of \p a times \p b both of length \p n.
 * \return resulting product.
 */
float
farray_dot(const float *a, const float *b, size_t n);

/*!
 * Calculate Dot (Scalar) Product of \p a times \p b both of length 16.
 * \return resulting product.
 */
float farray_dot_unrolled16(const float *a, const float *b);

/*!
 * Calculate Dot (Scalar) Product of \p a times \p b both of length 128.
 * \return resulting product.
 */
float
farray_dot_unrolled128(const float *a, const float *b);

/*!
 * Loop-Stretched (of degree 8) optimized \c farray_dot().
 * \return resulting product.
 */
float
farray_dot_8stretched_plainC(const float *a, const float *b, size_t n);

/*!
 * Loop-Stretched (of degree 8) optimized \c farray_dot_shv().
 * \return resulting product.
 */
float
farray_dot_shv_8stretched(const float *a, const short *b, size_t n);

/*!
 * Loop-Stretched (of degree 8) and SSE-assembler optimized \c farray_dot().
 * Arguments MUST BE 16-byte aligned in memory.
 * \return resulting product.
 */
float
farray_dot_8stretched_aligned16(farray_aligned16 a,
				    farray_aligned16 b,
				    size_t n);

/*!
 * Loop-Stretched (of degree 8) and SSE-assembler optimized \c farray_dot().
 * Arguments MUST NO BE 16-byte aligned in memory.
 * \return resulting product.
 */
float
farray_dot_8stretched_unaligned(const float * a,
                                const float * b,
                                size_t n);

/*!
 * Loop-Stretched (of degree 8) and SSE-assembler optimized \c farray_dot().
 * Arguments MUST NOT BUT SHOULD PREFERRABLY BE 16-byte aligned in memory.
 * \return resulting product.
 */
float
farray_dot_8stretched_anyaligned(const float * a,
                                 const float * b,
                                 size_t n);

#ifdef __cplusplus
}
#endif
