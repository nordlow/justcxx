#include "number.hpp"

namespace semnet {
namespace patterns {

Number::Number(BINUM_t numT)
{
    m_numT = numT;

    switch (m_numT) {
    case BINUM_any_: break;

    case BINUM_UINT8: d.u8 = new uint8_t[m_numT]; break;
    case BINUM_UINT16: d.u16 = new uint16_t[m_numT]; break;
    case BINUM_UINT24: d.u24 = new uint8_t[m_numT*3]; break;
    case BINUM_UINT32: d.u32 = new uint32_t[m_numT]; break;
    case BINUM_UINT64: d.u64 = new uint64_t[m_numT]; break;

    case BINUM_INT8: d.s8 = new int8_t[m_numT]; break;
    case BINUM_INT16: d.s16 = new int16_t[m_numT]; break;
    case BINUM_INT24: d.s24 = new int8_t[m_numT*3]; break;
    case BINUM_INT32: d.s32 = new int32_t[m_numT]; break;
    case BINUM_INT64: d.s64 = new int64_t[m_numT]; break;

    case BINUM_F32: d.f32 = new f32_t[m_numT]; break;
    case BINUM_F64: d.f64 = new f64_t[m_numT]; break;
    case BINUM_F96: d.f96 = new long double[m_numT]; break;

    case BINUM_MPZ: d.mpz = new mpz_class[m_numT]; break;
    case BINUM_MPQ: d.mpq = new mpq_class[m_numT]; break;

    case BINUM_undefined_: PWARN("m_numT is undefined\n"); break;
    }

}

Number::~Number()
{
    switch (m_numT) {
    case BINUM_any_: break;

    case BINUM_UINT8: delete[] d.u8; break;
    case BINUM_UINT16: delete[] d.u16; break;
    case BINUM_UINT24: delete[] d.u24; break;
    case BINUM_UINT32: delete[] d.u32; break;
    case BINUM_UINT64: delete[] d.u64; break;

    case BINUM_INT8: delete[] d.s8; break;
    case BINUM_INT16: delete[] d.s16; break;
    case BINUM_INT24: delete[] d.s24; break;
    case BINUM_INT32: delete[] d.s32; break;
    case BINUM_INT64: delete[] d.s64; break;

    case BINUM_F32: delete[] d.f32; break;
    case BINUM_F64: delete[] d.f64; break;
    case BINUM_F96: delete[] d.f96; break;

    case BINUM_MPZ: delete[] d.mpz; break;
    case BINUM_MPQ: delete[] d.mpq; break;

    case BINUM_undefined_: PWARN("m_numT is undefined\n"); break;
    }
}

}
}
