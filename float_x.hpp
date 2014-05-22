/*! \file float_x.h
 * \brief Half-Float.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://hbfs.wordpress.com/2013/02/12/float16/
 */

#pragma once

/*! Half 16-Bit Floating Point. */
typedef union {
    // hfloat v;
    struct {
        // type determines alignment!
        uint16_t m:10;              /**< Mantissa. */
        uint16_t e:5;               /**< Exponent. */
        uint16_t s:1;               /**< Sign. */
    } bits;
} hfloat;

/*! Normal 32-Bit Floating Point. */
typedef union {
    float v;
    struct {
        uint32_t m:23;              /**< Mantissa */
        uint32_t e:8;               /**< Exponent. */
        uint32_t s:1;               /**< Sign */
    } bits;
} float32_s;

inline hfloat spure float32to16(float x) {
    float32_s f32 = {x}; // C99
    hfloat f16;
    f16->bits.s = f32.bits.s;
    f16->bits.e = std::max(-15, std::min(16,(int)(f32.bits.e-127))) + 15;
    f16->bits.m = f32.bits.m >> 13;
    return f16;
}

inline float spure float16to32(hfloat f16) {
    float32_s f32;
    f32.bits.s = f16.bits.s;
    f32.bits.e = (f16.bits.e-15)+127; // safe in this direction
    f32.bits.m = ((uint32_t)f16.bits.m) << 13;
    return f32.v;
}
