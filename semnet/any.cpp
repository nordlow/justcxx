#include "../rangerand.h"
#include "../bitvec.h"
#include "any.hpp"

namespace semnet { namespace patterns {

pHit
Any::match_in_local(const char* buf, size_t len,
                    bir roi) const
{
    return (bitsize() <= roi.bitsize()) ? pHit(roi.low(), bitsize()) : pHit();
}

csc
Any::rand(bir ssr) const
{
    auto bs = bytesize();
    csc bufI(0, static_cast<size_t>(bs));
    uint8array_rangerand(reinterpret_cast<uint8_t*>(const_cast<char*>(bufI.data())), bs, 0, 255);
    return bufI;
}

Base::Skips8 &
Any::intersect_skips(Skips8 & skips) const
{
    return skips;
}

size_t
Any::complexity(bir mults) const
{
    const auto bits = bitsize().get();
    if (bitsize() < bits) {
        return static_cast<size_t>(1) << bits;
    } else {
        return pnw::max_of<size_t>();
    }
}

Any * anybits(bix bitsize)
{
    switch (bitsize.get()) {
    case   1: return new AnyBit(); break;
    case   2: return new AnyBits2(); break;
    case   3: return new AnyBits3(); break;
    case   4: return new AnyBits4(); break;
    case   5: return new AnyBits5(); break;
    case   6: return new AnyBits6(); break;
    case   7: return new AnyBits7(); break;
    case   8: return new AnyBits8(); break;
    case  16: return new AnyBits16(); break;
    case  24: return new AnyBits24(); break;
    case  32: return new AnyBits32(); break;
    case  40: return new AnyBits40(); break;
    case  48: return new AnyBits48(); break;
    case  56: return new AnyBits56(); break;
    case  64: return new AnyBits64(); break;
    case 128: return new AnyBits128(); break;
    case 256: return new AnyBits256(); break;
    case 512: return new AnyBits512(); break;
    default:  return new AnyBitsDynamic(bitsize); break;
    }
}
}
}
