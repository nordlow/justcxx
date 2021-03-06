/*! \file number.hpp
 * \brief Numerical Array/Vector.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Use liboctave for calculations.
 */

#pragma once
#include "obmr.hpp"
#include "../BINUM_enum.h"
#include "../binum_conv.h"
#include <gmpxx.h>

namespace semnet {
namespace patterns {

/*!
 * Numerical Array in Binary Form.
 */
class Number : public ObMr {
public:
    virtual OB_t get_type() const { return OB_NUMBER; };

    Number(BINUM_t numT);
    virtual ~Number();

    virtual std::ostream& show(std::ostream& os) const { return os << "numT:" << m_numT; }

    const void* data() const { return d.v; }
    size_t size() const { return m_size; }
private:
    union {
        void* v;

        uint8_t* u8;
        uint16_t* u16;
        uint8_t* u24;
        uint32_t* u32;
        uint64_t* u64;

        int8_t* s8;
        int16_t* s16;
        int8_t* s24;
        int32_t* s32;
        int64_t* s64;

        f32_t* f32;
        f64_t* f64;
        long double* f96;

        mpz_class* mpz;          ///< Actually a pointer.
        mpq_class* mpq;          ///< Actually a pointer.
    } __attribute__ ((aligned (16))) d; ///< Data Array. Aligned to 16-bytes for optimal SIMD-performance.
    size_t m_size;
    BINUM_t m_numT;              ///< Number Type.
};

namespace gen {
inline Number * pnum(BINUM_t numT)
{
    Number * pnum = new Number(numT);
    return pnum;
}
}

}
}
