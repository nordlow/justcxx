/*! \file pany.hpp
 * \brief \em Any Pattern.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-05-29 21:36
 */

#pragma once
#include "patt.hpp"

namespace semnet {
namespace patterns {

/*! Abstract \em Any Kind of Pattern of Known Length Disregarding Contents.
 *
 * \todo Pattern Tree should be checked if they are completely general and be
 * converted to this \c Any if they are completely general.
 */
class Any : public Base {
public:
    /// Any Kind of Pattern.
    Any() : Base() {}
    Any* clone(size_t depth = 0);

    virtual ~Any() {}

    virtual bir sample_range() const { return bir(bitsize(),
                                                  bitsize()); }
    virtual size_t complexity(bir mults = bir::full()) const;

    virtual csc rand(bir ssr = bir::full()) const;

    virtual Base::Skips8& intersect_skips(Skips8& skips) const;

    virtual std::ostream& show(std::ostream& os) const {
        Base::show(os);
        os << " bitsize:" << bitsize();
        Base::show_rand(os);
        return os;
    }

    /// Get \em Bit Size.
    virtual bix bitsize() const = 0;

    /// Get \em Padded \em Byte Size.
    size_t bytesize() const { return to_padded_byte(bitsize()); }
protected:
    virtual pHit match_in_local(const char* buf, size_t len,
                                bir roi = bir::full()) const;
};

class AnyBitsDynamic : public Any
{
public:
    AnyBitsDynamic(bix bitsize_arg) : Any() { init(bitsize_arg); }
    virtual bix bitsize() const { return m_bitsize; }
private:
    void init(bix bitsize_arg) { m_bitsize = bitsize_arg; }
    bix m_bitsize;               ///< Number of bytes.
};

class AnyBit : public Any { public: virtual bix bitsize() const { return static_cast<bix>(1); } };
class AnyBits2 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(2); } };
class AnyBits3 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(3); } };
class AnyBits4 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(4); } };
class AnyBits5 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(5); } };
class AnyBits6 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(6); } };
class AnyBits7 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(7); } };
class AnyBits8 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(8); } };
class AnyBits16 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(16); } };
class AnyBits24 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(24); } };
class AnyBits32 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(32); } };
class AnyBits40 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(40); } };
class AnyBits48 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(48); } };
class AnyBits56 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(56); } };
class AnyBits64 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(64); } };
class AnyBits128 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(128); } };
class AnyBits256 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(256); } };
class AnyBits512 : public Any { public: virtual bix bitsize() const { return static_cast<bix>(512); } };

// /*! Any N Bits */
// template <bix N>
// class AnyBitsStatic : public Any
// {
// public:
//     AnyBitsStatic();
//     virtual bix bitsize() const { return N; }
// };
// class AnyBit   : public AnyBitsStatic<1> { public: AnyBit() : AnyBitsStatic<1>() {} };
// class AnyBits2 : public AnyBitsStatic<2> { public: AnyBits2() : AnyBitsStatic<2>() {} };
// class AnyBits3 : public AnyBitsStatic<3> { public: AnyBits3() : AnyBitsStatic<3>() {} };
// class AnyBits4 : public AnyBitsStatic<4> { public: AnyBits4() : AnyBitsStatic<4>() {} };
// class AnyBits5 : public AnyBitsStatic<5> { public: AnyBits5() : AnyBitsStatic<5>() {} };
// class AnyBits6 : public AnyBitsStatic<6> { public: AnyBits6() : AnyBitsStatic<6>() {} };
// class AnyBits7 : public AnyBitsStatic<7> { public: AnyBits7() : AnyBitsStatic<7>() {} };
// class AnyBits8 : public AnyBitsStatic<8> { public: AnyBits8() : AnyBitsStatic<8>() {} };
// class AnyBits16 : public AnyBitsStatic<16> { public: AnyBits16() : AnyBitsStatic<16>() {} };
// class AnyBits24 : public AnyBitsStatic<24> { public: AnyBits24() : AnyBitsStatic<24>() {} };
// class AnyBits32 : public AnyBitsStatic<32> { public: AnyBits32() : AnyBitsStatic<32>() {} };
// class AnyBits40 : public AnyBitsStatic<40> { public: AnyBits40() : AnyBitsStatic<40>() {} };
// class AnyBits48 : public AnyBitsStatic<48> { public: AnyBits48() : AnyBitsStatic<48>() {} };
// class AnyBits56 : public AnyBitsStatic<56> { public: AnyBits56() : AnyBitsStatic<56>() {} };
// class AnyBits64 : public AnyBitsStatic<64> { public: AnyBits64() : AnyBitsStatic<64>() {} };
// class AnyBits128 : public AnyBitsStatic<128> { public: AnyBits128() : AnyBitsStatic<128>() {} };
// class AnyBits256 : public AnyBitsStatic<256> { public: AnyBits256() : AnyBitsStatic<256>() {} };
// class AnyBits512 : public AnyBitsStatic<512> { public: AnyBits512() : AnyBitsStatic<512>() {} };

/*! Factor Function for Specialized Variants of Any Number of \em Bits. *
 *
 * \note Use to create \em bit-granularity \em paddings between adjacent elements in a \c Seq.
 */
Any * anybits(bix bitsize);

/*! Factor Function for Specialized Variants of Any Number of \em Bytes.
 *
 * \note Use to create \em byte-granularity \em paddings between adjacent elements in a \c Seq.
 */
inline Any * anybytes(size_t bytesize) { return anybits(static_cast<bix>(8) * bytesize); }

}
}
